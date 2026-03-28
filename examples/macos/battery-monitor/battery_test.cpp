#include <gtest/gtest.h>
#include "BatteryMonitor.h"
#include "ClientBase.h"
#include "RunLoop.h"

#include <atomic>
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

// ── Test device that allows setting state directly ─────────────────

class TestBatteryDevice : public BatteryMonitor
{
public:
    using BatteryMonitor::BatteryMonitor;

    // Expose notify methods for testing.
    using BatteryMonitor::notifyBatteryLevelChanged;
    using BatteryMonitor::notifyPowerSourceChanged;

    void setCharge(float percent)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_status.chargePercent = percent;
        m_status.voltageV = 3.7f + (4.2f - 3.7f) * (percent / 100.0f);
        if (percent >= 100.0f)
            m_status.state = Full;
        else if (percent <= 20.0f)
            m_status.state = Charging;
        else
            m_status.state = Discharging;
    }

    void setPowerSource(PowerSource src)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_powerSource = src;
    }

protected:
    int handleGetBatteryStatus(BatteryStatus *status) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *status = m_status;
        return IPC_SUCCESS;
    }

    int handleGetPowerSource(PowerSource *source) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *source = m_powerSource;
        return IPC_SUCCESS;
    }

    int handleGetBatteryHealth(float *healthPercent, uint32_t *cycleCount) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *healthPercent = m_status.healthPercent;
        *cycleCount = m_status.cycleCount;
        return IPC_SUCCESS;
    }

private:
    std::mutex m_mutex;
    PowerSource m_powerSource = Battery;
    BatteryStatus m_status{
        Discharging, // state
        85.0f,       // chargePercent
        0,           // timeToEmptyMin
        0,           // timeToFullMin
        4.0f,        // voltageV
        500,         // cycleCount
        92.0f,       // healthPercent
    };
};

// ── Notification-receiving client ──────────────────────────────────

class TestBatteryClient : public ClientBase
{
public:
    TestBatteryClient(const char *name) : ClientBase(name) {}

    std::mutex mtx;
    std::condition_variable cv;

    float lastChargePercent = 0;
    ChargeState lastChargeState = NotCharging;
    PowerSource lastPowerSource = Unknown;
    std::atomic<int> levelChangedCount{0};
    std::atomic<int> sourceChangedCount{0};

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        if (serviceId != BatteryMonitor::kServiceId)
            return;

        std::lock_guard<std::mutex> lock(mtx);
        switch (messageId)
        {
        case BatteryMonitor::kBatteryLevelChanged:
        {
            if (payload.size() < sizeof(float) + sizeof(ChargeState))
                break;
            std::memcpy(&lastChargePercent, payload.data(), sizeof(lastChargePercent));
            std::memcpy(&lastChargeState, payload.data() + sizeof(lastChargePercent),
                        sizeof(lastChargeState));
            levelChangedCount++;
            cv.notify_all();
            break;
        }
        case BatteryMonitor::kPowerSourceChanged:
        {
            if (payload.size() < sizeof(PowerSource))
                break;
            std::memcpy(&lastPowerSource, payload.data(), sizeof(lastPowerSource));
            sourceChangedCount++;
            cv.notify_all();
            break;
        }
        default:
            break;
        }
    }
};

// ── RunLoop RAII guard ─────────────────────────────────────────────

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

// ═══════════════════════════════════════════════════════════════════
// GetBatteryStatus — returns valid data
// ═══════════════════════════════════════════════════════════════════

TEST(BatteryMonitorTest, GetBatteryStatus_ReturnsValidData)
{
    TestBatteryDevice svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(BatteryMonitor::kServiceId, BatteryMonitor::kGetBatteryStatus,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(BatteryStatus));

    BatteryStatus status;
    std::memcpy(&status, response.data(), sizeof(status));
    EXPECT_GE(status.chargePercent, 0.0f);
    EXPECT_LE(status.chargePercent, 100.0f);
    EXPECT_GE(status.voltageV, 3.0f);
    EXPECT_LE(status.voltageV, 4.5f);
    EXPECT_GE(status.healthPercent, 0.0f);
    EXPECT_LE(status.healthPercent, 100.0f);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// GetPowerSource — returns a known enum value
// ═══════════════════════════════════════════════════════════════════

TEST(BatteryMonitorTest, GetPowerSource_ReturnsKnownValue)
{
    TestBatteryDevice svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(BatteryMonitor::kServiceId, BatteryMonitor::kGetPowerSource,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(PowerSource));

    PowerSource source;
    std::memcpy(&source, response.data(), sizeof(source));
    EXPECT_TRUE(source == Battery || source == AC || source == UPS || source == Unknown);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// GetBatteryHealth — returns reasonable values
// ═══════════════════════════════════════════════════════════════════

TEST(BatteryMonitorTest, GetBatteryHealth_ReturnsReasonableValues)
{
    TestBatteryDevice svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(BatteryMonitor::kServiceId, BatteryMonitor::kGetBatteryHealth,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(float) + sizeof(uint32_t));

    float healthPercent;
    uint32_t cycleCount;
    std::memcpy(&healthPercent, response.data(), sizeof(healthPercent));
    std::memcpy(&cycleCount, response.data() + sizeof(healthPercent), sizeof(cycleCount));

    EXPECT_GE(healthPercent, 0.0f);
    EXPECT_LE(healthPercent, 100.0f);
    EXPECT_GE(cycleCount, 0u);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// BatteryLevelChanged notification fires
// ═══════════════════════════════════════════════════════════════════

TEST(BatteryMonitorTest, BatteryLevelChanged_NotificationFires)
{
    TestBatteryDevice svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    TestBatteryClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    ASSERT_EQ(svc.notifyBatteryLevelChanged(42.0f, Discharging), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(2000),
                                        [&] { return client.levelChangedCount > 0; }));
    }

    EXPECT_FLOAT_EQ(client.lastChargePercent, 42.0f);
    EXPECT_EQ(client.lastChargeState, Discharging);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// PowerSourceChanged notification fires
// ═══════════════════════════════════════════════════════════════════

TEST(BatteryMonitorTest, PowerSourceChanged_NotificationFires)
{
    TestBatteryDevice svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    TestBatteryClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    ASSERT_EQ(svc.notifyPowerSourceChanged(AC), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(2000),
                                        [&] { return client.sourceChangedCount > 0; }));
    }

    EXPECT_EQ(client.lastPowerSource, AC);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// RunLoop dispatch — basic call works in RunLoop mode
// ═══════════════════════════════════════════════════════════════════

TEST(BatteryMonitorTest, RunLoop_Dispatch)
{
    ms::RunLoop loop;
    loop.init("BatRL");

    TestBatteryDevice svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(BatteryMonitor::kServiceId, BatteryMonitor::kGetBatteryStatus,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(BatteryStatus));

    BatteryStatus status;
    std::memcpy(&status, response.data(), sizeof(status));
    EXPECT_GE(status.chargePercent, 0.0f);
    EXPECT_LE(status.chargePercent, 100.0f);

    client.disconnect();
    svc.stop();
}
