#include <gtest/gtest.h>
#include "ServiceStatus.h"
#include "ClientBase.h"
#include "RunLoop.h"

#include <chrono>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <thread>

using namespace aether::ipc;

#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

static void settle()
{
#if defined(_WIN32)
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
#endif
}

// ── Test server ─────────────────────────────────────────────────────

class TestServiceStatus : public ServiceStatus
{
public:
    using ServiceStatus::ServiceStatus;
    using ServiceStatus::notifyServiceStateChanged;

protected:
    int handleGetServiceStatus(const char *serviceName, ServiceInfo *info) override
    {
        for (const auto &s : m_services)
        {
            if (std::strcmp(s.serviceName, serviceName) == 0)
            {
                *info = s;
                return IPC_SUCCESS;
            }
        }
        return IPC_ERR_INVALID_ARGUMENT;
    }

    int handleGetServiceCount(uint32_t *count) override
    {
        *count = static_cast<uint32_t>(m_services.size());
        return IPC_SUCCESS;
    }

    int handleGetServiceAt(uint32_t index, ServiceInfo *info) override
    {
        if (index >= m_services.size())
        {
            return IPC_ERR_INVALID_ARGUMENT;
        }
        *info = m_services[index];
        return IPC_SUCCESS;
    }

private:
    static ServiceInfo makeInfo(const char *name, const char *display,
                                ServiceState state, uint32_t pid)
    {
        ServiceInfo info{};
        std::strncpy(info.serviceName, name, 64);
        std::strncpy(info.displayName, display, 128);
        info.state = state;
        info.pid = pid;
        return info;
    }

    std::vector<ServiceInfo> m_services = {
        makeInfo("Spooler", "Print Spooler", Running, 1200),
        makeInfo("BITS", "Background Intelligent Transfer", Running, 800),
        makeInfo("wuauserv", "Windows Update", Stopped, 0),
        makeInfo("WinDefend", "Windows Defender", Running, 2100),
        makeInfo("Dhcp", "DHCP Client", Running, 900),
    };
};

// ── Notification-receiving client ───────────────────────────────────

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
// GetServiceCount — returns a positive count
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceStatusTest, GetServiceCount_ReturnsPositive)
{
    TestServiceStatus svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(ServiceStatus::kServiceId, ServiceStatus::kGetServiceCount,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint32_t));

    uint32_t count;
    std::memcpy(&count, response.data(), sizeof(count));
    EXPECT_GT(count, 0u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetServiceAt — valid index returns non-empty service name
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceStatusTest, GetServiceAt_ValidIndex)
{
    TestServiceStatus svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t index = 0;
    std::vector<uint8_t> request(sizeof(index));
    std::memcpy(request.data(), &index, sizeof(index));

    std::vector<uint8_t> response;
    int rc = client.call(ServiceStatus::kServiceId, ServiceStatus::kGetServiceAt,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(ServiceInfo));

    ServiceInfo info;
    std::memcpy(&info, response.data(), sizeof(info));
    EXPECT_GT(std::strlen(info.serviceName), 0u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetServiceAt — invalid index returns error
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceStatusTest, GetServiceAt_InvalidIndex)
{
    TestServiceStatus svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t index = 999;
    std::vector<uint8_t> request(sizeof(index));
    std::memcpy(request.data(), &index, sizeof(index));

    std::vector<uint8_t> response;
    int rc = client.call(ServiceStatus::kServiceId, ServiceStatus::kGetServiceAt,
                         request, &response);

    EXPECT_EQ(rc, IPC_ERR_INVALID_ARGUMENT);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetServiceStatus — known service returns valid info
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceStatusTest, GetServiceStatus_KnownService)
{
    TestServiceStatus svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Marshal "Spooler" as a char[65] string.
    char serviceName[65] = {};
    std::strncpy(serviceName, "Spooler", 64);
    std::vector<uint8_t> request(65);
    std::memcpy(request.data(), serviceName, 65);

    std::vector<uint8_t> response;
    int rc = client.call(ServiceStatus::kServiceId, ServiceStatus::kGetServiceStatus,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(ServiceInfo));

    ServiceInfo info;
    std::memcpy(&info, response.data(), sizeof(info));
    EXPECT_STREQ(info.serviceName, "Spooler");
    EXPECT_STREQ(info.displayName, "Print Spooler");
    EXPECT_EQ(info.state, Running);
    EXPECT_EQ(info.pid, 1200u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetServiceStatus — unknown service returns error
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceStatusTest, GetServiceStatus_UnknownService)
{
    TestServiceStatus svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    char serviceName[65] = {};
    std::strncpy(serviceName, "NoSuchService", 64);
    std::vector<uint8_t> request(65);
    std::memcpy(request.data(), serviceName, 65);

    std::vector<uint8_t> response;
    int rc = client.call(ServiceStatus::kServiceId, ServiceStatus::kGetServiceStatus,
                         request, &response);

    EXPECT_EQ(rc, IPC_ERR_INVALID_ARGUMENT);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// ServiceStateChanged — notification fires with correct payload
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceStatusTest, ServiceStateChanged_NotificationFires)
{
    TestServiceStatus svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Build a ServiceInfo for notification.
    ServiceInfo info{};
    std::strncpy(info.serviceName, "Spooler", 64);
    std::strncpy(info.displayName, "Print Spooler", 128);
    info.state = Stopped;
    info.pid = 0;

    ASSERT_EQ(svc.notifyServiceStateChanged(info, Running), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(2000),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, ServiceStatus::kServiceId);
    EXPECT_EQ(client.lastMessageId, ServiceStatus::kServiceStateChanged);

    // Verify payload: ServiceInfo followed by ServiceState (oldState).
    ASSERT_EQ(client.lastPayload.size(), sizeof(ServiceInfo) + sizeof(ServiceState));

    ServiceInfo received;
    std::memcpy(&received, client.lastPayload.data(), sizeof(received));
    EXPECT_STREQ(received.serviceName, "Spooler");
    EXPECT_EQ(received.state, Stopped);

    ServiceState oldState;
    std::memcpy(&oldState, client.lastPayload.data() + sizeof(ServiceInfo), sizeof(oldState));
    EXPECT_EQ(oldState, Running);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop mode — server dispatch via RunLoop works
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceStatusTest, RunLoop_Dispatch)
{
    ms::RunLoop loop;
    loop.init("SrvRL");

    TestServiceStatus svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(ServiceStatus::kServiceId, ServiceStatus::kGetServiceCount,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint32_t));

    uint32_t count;
    std::memcpy(&count, response.data(), sizeof(count));
    EXPECT_EQ(count, 5u);

    client.disconnect();
    svc.stop();
}
