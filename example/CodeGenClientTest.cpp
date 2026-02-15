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
    int testNotifyDeviceConnected(DeviceInfo info)
    {
        std::vector<uint8_t> payload(sizeof(info));
        std::memcpy(payload.data(), &info, sizeof(info));
        return sendNotify(DeviceMonitor::kServiceId, DeviceMonitor::kDeviceConnected,
                          payload.data(), static_cast<uint32_t>(payload.size()));
    }

    int testNotifyDeviceDisconnected(uint32_t deviceId)
    {
        std::vector<uint8_t> payload(sizeof(deviceId));
        std::memcpy(payload.data(), &deviceId, sizeof(deviceId));
        return sendNotify(DeviceMonitor::kServiceId, DeviceMonitor::kDeviceDisconnected,
                          payload.data(), static_cast<uint32_t>(payload.size()));
    }

protected:
    int onRequest(uint32_t messageId, const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override
    {
        switch (messageId)
        {
        case DeviceMonitor::kGetDeviceCount:
        {
            uint32_t count = 3;
            response->resize(sizeof(count));
            std::memcpy(response->data(), &count, sizeof(count));
            return IPC_SUCCESS;
        }
        case DeviceMonitor::kGetDeviceInfo:
        {
            uint32_t deviceId;
            std::memcpy(&deviceId, request.data(), sizeof(deviceId));
            DeviceInfo info{};
            info.id = deviceId;
            info.type = (deviceId == 42) ? USB : Unknown;
            info.vendorId = 0x1234;
            info.productId = 0x5678;
            info.serial = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
            std::strncpy(info.name, "TestDevice", 64);
            response->resize(sizeof(info));
            std::memcpy(response->data(), &info, sizeof(info));
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

    DeviceInfo lastConnectedInfo{};
    uint32_t lastDisconnectedId = 0;
    std::atomic<int> connectCount{0};
    std::atomic<int> disconnectCount{0};

protected:
    void onDeviceConnected(DeviceInfo info) override
    {
        std::lock_guard<std::mutex> lock(mtx);
        lastConnectedInfo = info;
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
// GetDeviceInfo — typed client call with [in] and [out] struct
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenClientTest, GetDeviceInfo)
{
    DeviceMonitorStub svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    DeviceMonitor client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    DeviceInfo info{};
    int rc = client.GetDeviceInfo(42, &info);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(info.id, 42u);
    EXPECT_EQ(info.type, USB);
    EXPECT_EQ(info.vendorId, 0x1234u);
    EXPECT_EQ(info.productId, 0x5678u);
    EXPECT_EQ(info.serial[0], 0xAAu);
    EXPECT_EQ(info.serial[5], 0xFFu);

    // Unknown device
    rc = client.GetDeviceInfo(99, &info);
    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(info.id, 99u);
    EXPECT_EQ(info.type, Unknown);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// DeviceConnected notification via typed callback (struct payload)
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenClientTest, DeviceConnectedNotification)
{
    DeviceMonitorStub svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    TestDeviceClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    DeviceInfo info{7, USB, 0x1111, 0x2222, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06}};
    ASSERT_EQ(svc.testNotifyDeviceConnected(info), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.connectCount > 0; }));
    }

    EXPECT_EQ(client.lastConnectedInfo.id, 7u);
    EXPECT_EQ(client.lastConnectedInfo.type, USB);

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
