#include <gtest/gtest.h>
#include "DeviceMonitor.h"
#include "ServiceBase.h"
#include "RunLoop.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <thread>

using namespace ms::ipc;

#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

static void settle()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

// ── Stub service that speaks the DeviceMonitor protocol ─────────────
// This manually implements the server-side marshal/unmarshal so we can
// test the generated client without linking the generated server code.

class DeviceMonitorStub : public ServiceBase
{
public:
    using ServiceBase::ServiceBase;

    // Expose sendNotify for testing notifications.
    int testNotifyDeviceConnected(uint32_t deviceId)
    {
        std::vector<uint8_t> payload(sizeof(deviceId));
        std::memcpy(payload.data(), &deviceId, sizeof(deviceId));
        return sendNotify(DeviceMonitor::kServiceId, 1, payload.data(),
                          static_cast<uint32_t>(payload.size()));
    }

    int testNotifyDeviceDisconnected(uint32_t deviceId)
    {
        std::vector<uint8_t> payload(sizeof(deviceId));
        std::memcpy(payload.data(), &deviceId, sizeof(deviceId));
        return sendNotify(DeviceMonitor::kServiceId, 2, payload.data(),
                          static_cast<uint32_t>(payload.size()));
    }

protected:
    int onRequest(uint32_t messageId, const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override
    {
        switch (messageId)
        {
        case 1: // GetDeviceCount
        {
            uint32_t count = 3;
            response->resize(sizeof(count));
            std::memcpy(response->data(), &count, sizeof(count));
            return IPC_SUCCESS;
        }
        case 2: // GetDeviceStatus
        {
            uint32_t deviceId;
            std::memcpy(&deviceId, request.data(), sizeof(deviceId));
            uint32_t status = (deviceId == 42) ? 1u : 0u;
            response->resize(sizeof(status));
            std::memcpy(response->data(), &status, sizeof(status));
            return IPC_SUCCESS;
        }
        default:
            return IPC_ERR_INVALID_METHOD;
        }
    }
};

// ── Notification-receiving client ───────────────────────────────────

class TestDeviceClient : public DeviceMonitor
{
public:
    using DeviceMonitor::DeviceMonitor;

    std::mutex mtx;
    std::condition_variable cv;

    uint32_t lastConnectedId = 0;
    uint32_t lastDisconnectedId = 0;
    std::atomic<int> connectCount{0};
    std::atomic<int> disconnectCount{0};

protected:
    void onDeviceConnected(uint32_t deviceId) override
    {
        std::lock_guard<std::mutex> lock(mtx);
        lastConnectedId = deviceId;
        connectCount++;
        cv.notify_all();
    }

    void onDeviceDisconnected(uint32_t deviceId) override
    {
        std::lock_guard<std::mutex> lock(mtx);
        lastDisconnectedId = deviceId;
        disconnectCount++;
        cv.notify_all();
    }
};

// ── RunLoop RAII guard ──────────────────────────────────────────────

struct RunLoopGuard
{
    ms::RunLoop &loop;
    std::thread thread;

    explicit RunLoopGuard(ms::RunLoop &l) : loop(l), thread([&l] { l.run(); }) {}

    ~RunLoopGuard()
    {
        loop.stop();
        if (thread.joinable())
            thread.join();
    }
};

// ═════════════════════════════════════════════════════════════════════
// GetDeviceCount — typed client call
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenClientTest, GetDeviceCount)
{
    DeviceMonitorStub svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    DeviceMonitor client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t count = 0;
    int rc = client.GetDeviceCount(&count);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(count, 3u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetDeviceStatus — typed client call with [in] and [out]
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenClientTest, GetDeviceStatus)
{
    DeviceMonitorStub svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    DeviceMonitor client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t status = 0;
    int rc = client.GetDeviceStatus(42, &status);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(status, 1u);

    // Unknown device
    rc = client.GetDeviceStatus(99, &status);
    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(status, 0u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// DeviceConnected notification via typed callback
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenClientTest, DeviceConnectedNotification)
{
    DeviceMonitorStub svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    TestDeviceClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    ASSERT_EQ(svc.testNotifyDeviceConnected(7), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.connectCount > 0; }));
    }

    EXPECT_EQ(client.lastConnectedId, 7u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// DeviceDisconnected notification via typed callback
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenClientTest, DeviceDisconnectedNotification)
{
    DeviceMonitorStub svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    TestDeviceClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    ASSERT_EQ(svc.testNotifyDeviceDisconnected(42), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.disconnectCount > 0; }));
    }

    EXPECT_EQ(client.lastDisconnectedId, 42u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop mode — typed client call works
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenClientTest, RunLoop_TypedCall)
{
    DeviceMonitorStub svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ms::RunLoop loop;
    loop.init("CliRL");

    DeviceMonitor client(SVC_NAME, &loop);
    ASSERT_TRUE(client.connect());

    RunLoopGuard guard(loop);
    settle();

    uint32_t count = 0;
    int rc = client.GetDeviceCount(&count);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(count, 3u);

    client.disconnect();
    svc.stop();
}
