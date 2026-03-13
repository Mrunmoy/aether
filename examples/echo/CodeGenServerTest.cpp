#include <gtest/gtest.h>
#include "DeviceMonitor.h"
#include "ClientBase.h"
#include "RunLoop.h"

#include <chrono>
#include <cstring>
#include <thread>

using namespace ms::ipc;

#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

static void settle()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

// ── Concrete service implementation ─────────────────────────────────

class TestDeviceMonitor : public DeviceMonitor
{
public:
    using DeviceMonitor::DeviceMonitor;

    // Expose notify methods for testing.
    using DeviceMonitor::notifyDeviceConnected;
    using DeviceMonitor::notifyDeviceDisconnected;

protected:
    int handleGetDeviceCount(uint32_t *count) override
    {
        *count = 3;
        return IPC_SUCCESS;
    }

    int handleGetDeviceInfo(uint32_t deviceId, DeviceInfo *info) override
    {
        info->id = deviceId;
        info->type = (deviceId == 42) ? USB : Unknown;
        info->vendorId = 0x1234;
        info->productId = 0x5678;
        info->serial = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
        std::strncpy(info->name, "TestDevice", 64);
        return IPC_SUCCESS;
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
// GetDeviceCount — raw client sends method 1, gets count back
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenServerTest, GetDeviceCount)
{
    TestDeviceMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Empty request for GetDeviceCount (no [in] params)
    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(DeviceMonitor::kServiceId, DeviceMonitor::kGetDeviceCount, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint32_t));

    uint32_t count;
    std::memcpy(&count, response.data(), sizeof(count));
    EXPECT_EQ(count, 3u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetDeviceInfo — raw client sends deviceId=42, gets DeviceInfo back
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenServerTest, GetDeviceInfo)
{
    TestDeviceMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Marshal deviceId=42
    uint32_t deviceId = 42;
    std::vector<uint8_t> request(sizeof(deviceId));
    std::memcpy(request.data(), &deviceId, sizeof(deviceId));

    std::vector<uint8_t> response;
    int rc = client.call(DeviceMonitor::kServiceId, DeviceMonitor::kGetDeviceInfo, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(DeviceInfo));

    DeviceInfo info;
    std::memcpy(&info, response.data(), sizeof(info));
    EXPECT_EQ(info.id, 42u);
    EXPECT_EQ(info.type, USB);
    EXPECT_EQ(info.vendorId, 0x1234u);
    EXPECT_EQ(info.productId, 0x5678u);
    EXPECT_EQ(info.serial[0], 0xAAu);
    EXPECT_EQ(info.serial[5], 0xFFu);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// DeviceConnected notification — raw client receives it
// ═════════════════════════════════════════════════════════════════════

class NotifyClient : public ClientBase
{
public:
    using ClientBase::ClientBase;

    std::mutex mtx;
    std::condition_variable cv;
    uint32_t lastServiceId = 0;
    uint32_t lastMessageId = 0;
    std::vector<uint8_t> lastPayload;
    int notifyCount = 0;

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        std::lock_guard<std::mutex> lock(mtx);
        lastServiceId = serviceId;
        lastMessageId = messageId;
        lastPayload = payload;
        notifyCount++;
        cv.notify_all();
    }
};

TEST(CodeGenServerTest, DeviceConnectedNotification)
{
    TestDeviceMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    DeviceInfo info{7, USB, 0x1111, 0x2222, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06}};
    ASSERT_EQ(svc.notifyDeviceConnected(info), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, DeviceMonitor::kServiceId);
    EXPECT_EQ(client.lastMessageId, DeviceMonitor::kDeviceConnected);
    ASSERT_EQ(client.lastPayload.size(), sizeof(DeviceInfo));

    DeviceInfo received;
    std::memcpy(&received, client.lastPayload.data(), sizeof(received));
    EXPECT_EQ(received.id, 7u);
    EXPECT_EQ(received.type, USB);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// DeviceDisconnected notification
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenServerTest, DeviceDisconnectedNotification)
{
    TestDeviceMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    ASSERT_EQ(svc.notifyDeviceDisconnected(42), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastMessageId, DeviceMonitor::kDeviceDisconnected);

    uint32_t deviceId;
    std::memcpy(&deviceId, client.lastPayload.data(), sizeof(deviceId));
    EXPECT_EQ(deviceId, 42u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Invalid method returns error
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenServerTest, InvalidMethod)
{
    TestDeviceMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(DeviceMonitor::kServiceId, 99, request, &response);

    EXPECT_EQ(rc, IPC_ERR_INVALID_METHOD);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop mode — generated server dispatch works
// ═════════════════════════════════════════════════════════════════════

TEST(CodeGenServerTest, RunLoop_Dispatch)
{
    ms::RunLoop loop;
    loop.init("SrvRL");

    TestDeviceMonitor svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // GetDeviceCount via RunLoop-driven server
    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(DeviceMonitor::kServiceId, DeviceMonitor::kGetDeviceCount, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint32_t));

    uint32_t count;
    std::memcpy(&count, response.data(), sizeof(count));
    EXPECT_EQ(count, 3u);

    client.disconnect();
    svc.stop();
}
