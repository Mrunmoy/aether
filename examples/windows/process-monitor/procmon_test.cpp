#include <gtest/gtest.h>
#include "ProcessMonitor.h"
#include "ClientBase.h"
#include "RunLoop.h"

#include <chrono>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

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

class TestProcessMonitor : public ProcessMonitor
{
public:
    using ProcessMonitor::ProcessMonitor;
    using ProcessMonitor::notifyProcessStarted;
    using ProcessMonitor::notifyProcessExited;

protected:
    int handleGetProcessCount(uint32_t *count) override
    {
        *count = static_cast<uint32_t>(processes().size());
        return IPC_SUCCESS;
    }

    int handleGetProcessInfo(uint32_t pid, ProcessInfo *info) override
    {
        for (const auto &p : processes())
        {
            if (p.pid == pid)
            {
                *info = p;
                return IPC_SUCCESS;
            }
        }
        return IPC_ERR_INVALID_ARGUMENT;
    }

    int handleGetProcessAt(uint32_t index, ProcessInfo *info) override
    {
        const auto &procs = processes();
        if (index >= procs.size())
        {
            return IPC_ERR_INVALID_ARGUMENT;
        }
        *info = procs[index];
        return IPC_SUCCESS;
    }

private:
    static ProcessInfo makeProc(uint32_t pid, uint32_t parent, const char *name,
                                uint32_t threads, uint64_t mem)
    {
        ProcessInfo p{};
        p.pid = pid;
        p.parentPid = parent;
        std::strncpy(p.name, name, sizeof(p.name) - 1);
        p.threadCount = threads;
        p.memoryBytes = mem;
        return p;
    }

    static const std::vector<ProcessInfo> &processes()
    {
        static const std::vector<ProcessInfo> procs = {
            makeProc(1, 0, "System", 4, 1ULL * 1024 * 1024),
            makeProc(100, 1, "explorer.exe", 12, 80ULL * 1024 * 1024),
            makeProc(200, 1, "svchost.exe", 8, 40ULL * 1024 * 1024),
            makeProc(300, 100, "chrome.exe", 25, 350ULL * 1024 * 1024),
            makeProc(400, 100, "notepad.exe", 3, 5ULL * 1024 * 1024),
        };
        return procs;
    }
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
// GetProcessCount — returns a positive count
// ═════════════════════════════════════════════════════════════════════

TEST(ProcessMonitorTest, GetProcessCount_ReturnsPositive)
{
    TestProcessMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(ProcessMonitor::kServiceId, ProcessMonitor::kGetProcessCount,
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
// GetProcessAt — valid index returns info with pid > 0
// ═════════════════════════════════════════════════════════════════════

TEST(ProcessMonitorTest, GetProcessAt_ValidIndex)
{
    TestProcessMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t index = 0;
    std::vector<uint8_t> request(sizeof(index));
    std::memcpy(request.data(), &index, sizeof(index));

    std::vector<uint8_t> response;
    int rc = client.call(ProcessMonitor::kServiceId, ProcessMonitor::kGetProcessAt,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(ProcessInfo));

    ProcessInfo info;
    std::memcpy(&info, response.data(), sizeof(info));
    EXPECT_GT(info.pid, 0u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetProcessAt — invalid index returns error
// ═════════════════════════════════════════════════════════════════════

TEST(ProcessMonitorTest, GetProcessAt_InvalidIndex)
{
    TestProcessMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t index = 999;
    std::vector<uint8_t> request(sizeof(index));
    std::memcpy(request.data(), &index, sizeof(index));

    std::vector<uint8_t> response;
    int rc = client.call(ProcessMonitor::kServiceId, ProcessMonitor::kGetProcessAt,
                         request, &response);

    EXPECT_EQ(rc, IPC_ERR_INVALID_ARGUMENT);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetProcessInfo — known PID returns correct data
// ═════════════════════════════════════════════════════════════════════

TEST(ProcessMonitorTest, GetProcessInfo_KnownPid)
{
    TestProcessMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t pid = 1;
    std::vector<uint8_t> request(sizeof(pid));
    std::memcpy(request.data(), &pid, sizeof(pid));

    std::vector<uint8_t> response;
    int rc = client.call(ProcessMonitor::kServiceId, ProcessMonitor::kGetProcessInfo,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(ProcessInfo));

    ProcessInfo info;
    std::memcpy(&info, response.data(), sizeof(info));
    EXPECT_EQ(info.pid, 1u);
    EXPECT_EQ(info.parentPid, 0u);
    EXPECT_STREQ(info.name, "System");
    EXPECT_EQ(info.threadCount, 4u);
    EXPECT_EQ(info.memoryBytes, 1ULL * 1024 * 1024);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetProcessInfo — unknown PID returns error
// ═════════════════════════════════════════════════════════════════════

TEST(ProcessMonitorTest, GetProcessInfo_UnknownPid)
{
    TestProcessMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t pid = 9999;
    std::vector<uint8_t> request(sizeof(pid));
    std::memcpy(request.data(), &pid, sizeof(pid));

    std::vector<uint8_t> response;
    int rc = client.call(ProcessMonitor::kServiceId, ProcessMonitor::kGetProcessInfo,
                         request, &response);

    EXPECT_EQ(rc, IPC_ERR_INVALID_ARGUMENT);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// ProcessStarted — notification fires with correct payload
// ═════════════════════════════════════════════════════════════════════

TEST(ProcessMonitorTest, ProcessStarted_NotificationFires)
{
    TestProcessMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    ProcessInfo spawned{};
    spawned.pid = 500;
    spawned.parentPid = 100;
    std::strncpy(spawned.name, "app_0500.exe", sizeof(spawned.name) - 1);
    spawned.threadCount = 6;
    spawned.memoryBytes = 10ULL * 1024 * 1024;

    ASSERT_EQ(svc.notifyProcessStarted(spawned), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(2000),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, ProcessMonitor::kServiceId);
    EXPECT_EQ(client.lastMessageId, ProcessMonitor::kProcessStarted);
    ASSERT_EQ(client.lastPayload.size(), sizeof(ProcessInfo));

    ProcessInfo received;
    std::memcpy(&received, client.lastPayload.data(), sizeof(received));
    EXPECT_EQ(received.pid, 500u);
    EXPECT_EQ(received.parentPid, 100u);
    EXPECT_STREQ(received.name, "app_0500.exe");

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// ProcessExited — notification fires with correct payload
// ═════════════════════════════════════════════════════════════════════

TEST(ProcessMonitorTest, ProcessExited_NotificationFires)
{
    TestProcessMonitor svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    ASSERT_EQ(svc.notifyProcessExited(300, 42), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(2000),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, ProcessMonitor::kServiceId);
    EXPECT_EQ(client.lastMessageId, ProcessMonitor::kProcessExited);
    ASSERT_EQ(client.lastPayload.size(), sizeof(uint32_t) + sizeof(uint32_t));

    uint32_t pid;
    std::memcpy(&pid, client.lastPayload.data(), sizeof(pid));
    EXPECT_EQ(pid, 300u);

    uint32_t exitCode;
    std::memcpy(&exitCode, client.lastPayload.data() + sizeof(pid), sizeof(exitCode));
    EXPECT_EQ(exitCode, 42u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop mode — server dispatch via RunLoop works
// ═════════════════════════════════════════════════════════════════════

TEST(ProcessMonitorTest, RunLoop_Dispatch)
{
    ms::RunLoop loop;
    loop.init("SrvRL");

    TestProcessMonitor svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(ProcessMonitor::kServiceId, ProcessMonitor::kGetProcessCount,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint32_t));

    uint32_t count;
    std::memcpy(&count, response.data(), sizeof(count));
    EXPECT_EQ(count, 5u);

    client.disconnect();
    svc.stop();
}
