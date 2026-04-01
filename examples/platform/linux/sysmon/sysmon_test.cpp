#include <gtest/gtest.h>
#include "SysMon.h"
#include "ClientBase.h"
#include "RunLoop.h"

#include <chrono>
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

// ── Test server implementation ──────────────────────────────────────

class TestSysMon : public SysMon
{
public:
    using SysMon::SysMon;

    // Expose notify methods for testing.
    using SysMon::notifyHighCpuAlert;
    using SysMon::notifyLowMemoryAlert;

protected:
    int handleGetCpuUsage(CpuUsage *usage) override
    {
        // Read real /proc/stat data (two samples 50ms apart for a quick delta)
        struct CpuJiffies
        {
            uint64_t user = 0, nice = 0, system = 0, idle = 0;
            uint64_t iowait = 0, irq = 0, softirq = 0, steal = 0;
            uint64_t total() const
            {
                return user + nice + system + idle + iowait + irq + softirq + steal;
            }
        };

        auto readJiffies = [](CpuJiffies *j) -> bool
        {
            FILE *f = std::fopen("/proc/stat", "r");
            if (!f)
                return false;
            int n = std::fscanf(f, "cpu %lu %lu %lu %lu %lu %lu %lu %lu",
                                &j->user, &j->nice, &j->system, &j->idle,
                                &j->iowait, &j->irq, &j->softirq, &j->steal);
            std::fclose(f);
            return n >= 4;
        };

        CpuJiffies prev{}, curr{};
        if (!readJiffies(&prev))
            return -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (!readJiffies(&curr))
            return -1;

        uint64_t dTotal = curr.total() - prev.total();
        if (dTotal == 0)
            dTotal = 1;

        float scale = 100.0f / static_cast<float>(dTotal);
        usage->user = static_cast<float>(
            (curr.user + curr.nice) - (prev.user + prev.nice)) * scale;
        usage->system = static_cast<float>(
            (curr.system + curr.irq + curr.softirq)
            - (prev.system + prev.irq + prev.softirq)) * scale;
        usage->idle = static_cast<float>(curr.idle - prev.idle) * scale;
        usage->iowait = static_cast<float>(curr.iowait - prev.iowait) * scale;
        usage->total = usage->user + usage->system + usage->iowait;

        return IPC_SUCCESS;
    }

    int handleGetMemoryInfo(MemoryInfo *info) override
    {
        FILE *f = std::fopen("/proc/meminfo", "r");
        if (!f)
            return -1;

        char line[256];
        int found = 0;
        while (std::fgets(line, sizeof(line), f) && found < 5)
        {
            uint64_t val = 0;
            if (std::sscanf(line, "MemTotal: %lu kB", &val) == 1)
            {
                info->totalKb = val;
                ++found;
            }
            else if (std::sscanf(line, "MemFree: %lu kB", &val) == 1)
            {
                info->freeKb = val;
                ++found;
            }
            else if (std::sscanf(line, "MemAvailable: %lu kB", &val) == 1)
            {
                info->availableKb = val;
                ++found;
            }
            else if (std::sscanf(line, "Buffers: %lu kB", &val) == 1)
            {
                info->buffersKb = val;
                ++found;
            }
            else if (std::sscanf(line, "Cached: %lu kB", &val) == 1)
            {
                info->cachedKb = val;
                ++found;
            }
        }
        std::fclose(f);

        if (info->totalKb > 0)
        {
            info->usedKb = info->totalKb - info->availableKb;
            info->usedPercent = 100.0f * static_cast<float>(info->usedKb)
                                / static_cast<float>(info->totalKb);
        }
        return (found == 5) ? IPC_SUCCESS : -1;
    }

    int handleGetLoadAverage(LoadAverage *load) override
    {
        FILE *f = std::fopen("/proc/loadavg", "r");
        if (!f)
            return -1;

        unsigned running = 0, total = 0;
        int n = std::fscanf(f, "%f %f %f %u/%u",
                            &load->avg1, &load->avg5, &load->avg15,
                            &running, &total);
        std::fclose(f);

        load->runningProcs = running;
        load->totalProcs = total;
        return (n >= 3) ? IPC_SUCCESS : -1;
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

// ── Notification-aware raw client ───────────────────────────────────

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

// ═════════════════════════════════════════════════════════════════════
// GetCpuUsage — returns valid CPU usage data
// ═════════════════════════════════════════════════════════════════════

TEST(SysMonTest, GetCpuUsage_ReturnsValidData)
{
    TestSysMon svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(SysMon::kServiceId, SysMon::kGetCpuUsage, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(CpuUsage));

    CpuUsage usage;
    std::memcpy(&usage, response.data(), sizeof(usage));
    EXPECT_GE(usage.total, 0.0f);
    EXPECT_LE(usage.total, 100.0f);
    EXPECT_GE(usage.idle, 0.0f);
    EXPECT_LE(usage.idle, 100.0f);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetMemoryInfo — returns valid memory data
// ═════════════════════════════════════════════════════════════════════

TEST(SysMonTest, GetMemoryInfo_ReturnsValidData)
{
    TestSysMon svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(SysMon::kServiceId, SysMon::kGetMemoryInfo, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(MemoryInfo));

    MemoryInfo info;
    std::memcpy(&info, response.data(), sizeof(info));
    EXPECT_GT(info.totalKb, 0u);
    EXPECT_GT(info.freeKb, 0u);
    EXPECT_GT(info.availableKb, 0u);
    EXPECT_LE(info.usedKb, info.totalKb);
    EXPECT_GE(info.usedPercent, 0.0f);
    EXPECT_LE(info.usedPercent, 100.0f);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetLoadAverage — returns valid load data
// ═════════════════════════════════════════════════════════════════════

TEST(SysMonTest, GetLoadAverage_ReturnsValidData)
{
    TestSysMon svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(SysMon::kServiceId, SysMon::kGetLoadAverage, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(LoadAverage));

    LoadAverage load;
    std::memcpy(&load, response.data(), sizeof(load));
    EXPECT_GE(load.avg1, 0.0f);
    EXPECT_GE(load.avg5, 0.0f);
    EXPECT_GE(load.avg15, 0.0f);
    EXPECT_GT(load.totalProcs, 0u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// HighCpuAlert notification fires
// ═════════════════════════════════════════════════════════════════════

TEST(SysMonTest, HighCpuAlert_Fires)
{
    TestSysMon svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    float cpuPercent = 95.5f;
    ASSERT_EQ(svc.notifyHighCpuAlert(cpuPercent), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, SysMon::kServiceId);
    EXPECT_EQ(client.lastMessageId, SysMon::kHighCpuAlert);
    ASSERT_EQ(client.lastPayload.size(), sizeof(float));

    float received;
    std::memcpy(&received, client.lastPayload.data(), sizeof(received));
    EXPECT_FLOAT_EQ(received, 95.5f);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// LowMemoryAlert notification fires
// ═════════════════════════════════════════════════════════════════════

TEST(SysMonTest, LowMemoryAlert_Fires)
{
    TestSysMon svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    float usedPercent = 88.0f;
    uint64_t availableKb = 512000;
    ASSERT_EQ(svc.notifyLowMemoryAlert(usedPercent, availableKb), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, SysMon::kServiceId);
    EXPECT_EQ(client.lastMessageId, SysMon::kLowMemoryAlert);
    ASSERT_EQ(client.lastPayload.size(), sizeof(float) + sizeof(uint64_t));

    float recvPercent;
    uint64_t recvAvail;
    std::memcpy(&recvPercent, client.lastPayload.data(), sizeof(recvPercent));
    std::memcpy(&recvAvail, client.lastPayload.data() + sizeof(recvPercent), sizeof(recvAvail));
    EXPECT_FLOAT_EQ(recvPercent, 88.0f);
    EXPECT_EQ(recvAvail, 512000u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Invalid method returns error
// ═════════════════════════════════════════════════════════════════════

TEST(SysMonTest, InvalidMethod_ReturnsError)
{
    TestSysMon svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(SysMon::kServiceId, 99, request, &response);

    EXPECT_EQ(rc, IPC_ERR_INVALID_METHOD);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop mode — generated server dispatch works
// ═════════════════════════════════════════════════════════════════════

TEST(SysMonTest, RunLoop_Dispatch)
{
    ms::RunLoop loop;
    loop.init("SrvRL");

    TestSysMon svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // GetLoadAverage via RunLoop-driven server (fast, no sleep)
    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(SysMon::kServiceId, SysMon::kGetLoadAverage, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(LoadAverage));

    LoadAverage load;
    std::memcpy(&load, response.data(), sizeof(load));
    EXPECT_GE(load.avg1, 0.0f);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop mode — notification delivery
// ═════════════════════════════════════════════════════════════════════

TEST(SysMonTest, RunLoop_Notification)
{
    ms::RunLoop loop;
    loop.init("SrvRL");

    TestSysMon svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    ASSERT_EQ(svc.notifyHighCpuAlert(99.0f), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastMessageId, SysMon::kHighCpuAlert);

    float received;
    std::memcpy(&received, client.lastPayload.data(), sizeof(received));
    EXPECT_FLOAT_EQ(received, 99.0f);

    client.disconnect();
    svc.stop();
}
