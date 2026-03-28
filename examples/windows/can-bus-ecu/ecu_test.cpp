#include <gtest/gtest.h>
#include "CanBusEcu.h"
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

// ── Test server implementation ─────────────────────────────────────

class TestCanBusEcu : public CanBusEcu
{
public:
    using CanBusEcu::CanBusEcu;

    // Expose notification methods for testing.
    using CanBusEcu::notifyCheckEngine;
    using CanBusEcu::notifyDtcCleared;

protected:
    int handleReadPid(uint8_t pid, PidReading *reading) override
    {
        reading->pid = pid;
        switch (pid)
        {
        case 0x0C:
            reading->value = 800.0f;
            std::strncpy(reading->unit, "RPM", sizeof(reading->unit) - 1);
            break;
        case 0x0D:
            reading->value = 0.0f;
            std::strncpy(reading->unit, "km/h", sizeof(reading->unit) - 1);
            break;
        case 0x05:
            reading->value = 90.0f;
            std::strncpy(reading->unit, "\xC2\xB0""C", sizeof(reading->unit) - 1);
            break;
        case 0x11:
            reading->value = 0.0f;
            std::strncpy(reading->unit, "%", sizeof(reading->unit) - 1);
            break;
        case 0x2F:
            reading->value = 72.0f;
            std::strncpy(reading->unit, "%", sizeof(reading->unit) - 1);
            break;
        default:
            return 1; // unknown PID
        }
        return IPC_SUCCESS;
    }

    int handleGetDtcCount(uint8_t *count) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *count = static_cast<uint8_t>(m_dtcs.size());
        return IPC_SUCCESS;
    }

    int handleGetDtc(uint8_t index, DtcEntry *dtc) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (index >= m_dtcs.size())
            return 1; // invalid index
        *dtc = m_dtcs[index];
        return IPC_SUCCESS;
    }

    int handleClearDtc(uint8_t *clearedCount) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *clearedCount = static_cast<uint8_t>(m_dtcs.size());
        m_dtcs.clear();
        return IPC_SUCCESS;
    }

    int handleGetVehicleInfo(VehicleInfo *info) override
    {
        std::strncpy(info->vin, "1HGBH41JXMN109186", sizeof(info->vin) - 1);
        info->modelYear = 2024;
        std::strncpy(info->make, "Honda", sizeof(info->make) - 1);
        std::strncpy(info->model, "Civic", sizeof(info->model) - 1);
        return IPC_SUCCESS;
    }

public:
    void addDtc(const char *code, DtcSeverity severity, const char *desc)
    {
        DtcEntry dtc{};
        std::strncpy(dtc.code, code, sizeof(dtc.code) - 1);
        dtc.severity = severity;
        std::strncpy(dtc.description, desc, sizeof(dtc.description) - 1);
        std::lock_guard<std::mutex> lock(m_mutex);
        m_dtcs.push_back(dtc);
    }

private:
    std::mutex m_mutex;
    std::vector<DtcEntry> m_dtcs;
};

// ── Notification-receiving raw client ──────────────────────────────

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
// ReadPid_EngineRpm — PID 0x0C returns value 0-8000
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, ReadPid_EngineRpm)
{
    TestCanBusEcu svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint8_t pid = 0x0C;
    std::vector<uint8_t> request(sizeof(pid));
    std::memcpy(request.data(), &pid, sizeof(pid));

    std::vector<uint8_t> response;
    int rc = client.call(CanBusEcu::kServiceId, CanBusEcu::kReadPid, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(PidReading));

    PidReading reading;
    std::memcpy(&reading, response.data(), sizeof(reading));
    EXPECT_EQ(reading.pid, 0x0C);
    EXPECT_GE(reading.value, 0.0f);
    EXPECT_LE(reading.value, 8000.0f);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// ReadPid_VehicleSpeed — PID 0x0D returns value >= 0
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, ReadPid_VehicleSpeed)
{
    TestCanBusEcu svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint8_t pid = 0x0D;
    std::vector<uint8_t> request(sizeof(pid));
    std::memcpy(request.data(), &pid, sizeof(pid));

    std::vector<uint8_t> response;
    int rc = client.call(CanBusEcu::kServiceId, CanBusEcu::kReadPid, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(PidReading));

    PidReading reading;
    std::memcpy(&reading, response.data(), sizeof(reading));
    EXPECT_EQ(reading.pid, 0x0D);
    EXPECT_GE(reading.value, 0.0f);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// ReadPid_Unknown — PID 0xFF returns error
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, ReadPid_Unknown)
{
    TestCanBusEcu svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint8_t pid = 0xFF;
    std::vector<uint8_t> request(sizeof(pid));
    std::memcpy(request.data(), &pid, sizeof(pid));

    std::vector<uint8_t> response;
    int rc = client.call(CanBusEcu::kServiceId, CanBusEcu::kReadPid, request, &response);

    EXPECT_NE(rc, IPC_SUCCESS);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// GetVehicleInfo_ReturnsValidVin — VIN is non-empty
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, GetVehicleInfo_ReturnsValidVin)
{
    TestCanBusEcu svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(CanBusEcu::kServiceId, CanBusEcu::kGetVehicleInfo, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(VehicleInfo));

    VehicleInfo info;
    std::memcpy(&info, response.data(), sizeof(info));
    EXPECT_GT(std::strlen(info.vin), 0u);
    EXPECT_STREQ(info.vin, "1HGBH41JXMN109186");
    EXPECT_EQ(info.modelYear, 2024);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// GetDtcCount_ReturnsKnown — count >= 0
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, GetDtcCount_ReturnsKnown)
{
    TestCanBusEcu svc(SVC_NAME);
    svc.addDtc("P0301", Warning, "Cylinder 1 Misfire");
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(CanBusEcu::kServiceId, CanBusEcu::kGetDtcCount, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint8_t));

    uint8_t count;
    std::memcpy(&count, response.data(), sizeof(count));
    EXPECT_GE(count, 0);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// GetDtc_ValidIndex — returns non-empty code
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, GetDtc_ValidIndex)
{
    TestCanBusEcu svc(SVC_NAME);
    svc.addDtc("P0301", Warning, "Cylinder 1 Misfire");
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint8_t index = 0;
    std::vector<uint8_t> request(sizeof(index));
    std::memcpy(request.data(), &index, sizeof(index));

    std::vector<uint8_t> response;
    int rc = client.call(CanBusEcu::kServiceId, CanBusEcu::kGetDtc, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(DtcEntry));

    DtcEntry dtc;
    std::memcpy(&dtc, response.data(), sizeof(dtc));
    EXPECT_GT(std::strlen(dtc.code), 0u);
    EXPECT_STREQ(dtc.code, "P0301");
    EXPECT_EQ(dtc.severity, Warning);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// GetDtc_InvalidIndex — returns error
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, GetDtc_InvalidIndex)
{
    TestCanBusEcu svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint8_t index = 99;
    std::vector<uint8_t> request(sizeof(index));
    std::memcpy(request.data(), &index, sizeof(index));

    std::vector<uint8_t> response;
    int rc = client.call(CanBusEcu::kServiceId, CanBusEcu::kGetDtc, request, &response);

    EXPECT_NE(rc, IPC_SUCCESS);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// ClearDtc_ClearsAll — clear, verify count=0
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, ClearDtc_ClearsAll)
{
    TestCanBusEcu svc(SVC_NAME);
    svc.addDtc("P0301", Warning, "Cylinder 1 Misfire");
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Clear DTCs.
    {
        std::vector<uint8_t> request;
        std::vector<uint8_t> response;
        int rc = client.call(CanBusEcu::kServiceId, CanBusEcu::kClearDtc, request, &response);
        ASSERT_EQ(rc, IPC_SUCCESS);
        ASSERT_EQ(response.size(), sizeof(uint8_t));

        uint8_t cleared;
        std::memcpy(&cleared, response.data(), sizeof(cleared));
        EXPECT_EQ(cleared, 1u);
    }

    // Verify count is now 0.
    {
        std::vector<uint8_t> request;
        std::vector<uint8_t> response;
        int rc = client.call(CanBusEcu::kServiceId, CanBusEcu::kGetDtcCount, request, &response);
        ASSERT_EQ(rc, IPC_SUCCESS);

        uint8_t count;
        std::memcpy(&count, response.data(), sizeof(count));
        EXPECT_EQ(count, 0u);
    }

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// CheckEngine_NotificationFires — trigger a DTC, verify notification
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, CheckEngine_NotificationFires)
{
    TestCanBusEcu svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    DtcEntry dtc{};
    std::strncpy(dtc.code, "P0420", sizeof(dtc.code) - 1);
    dtc.severity = Critical;
    std::strncpy(dtc.description, "Catalyst Efficiency", sizeof(dtc.description) - 1);

    ASSERT_EQ(svc.notifyCheckEngine(dtc), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, CanBusEcu::kServiceId);
    EXPECT_EQ(client.lastMessageId, CanBusEcu::kCheckEngine);
    ASSERT_GE(client.lastPayload.size(), sizeof(DtcEntry));

    DtcEntry received;
    std::memcpy(&received, client.lastPayload.data(), sizeof(received));
    EXPECT_STREQ(received.code, "P0420");
    EXPECT_EQ(received.severity, Critical);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// DtcCleared_NotificationFires — clear DTCs, verify notification
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, DtcCleared_NotificationFires)
{
    TestCanBusEcu svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    ASSERT_EQ(svc.notifyDtcCleared(3), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(500),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, CanBusEcu::kServiceId);
    EXPECT_EQ(client.lastMessageId, CanBusEcu::kDtcCleared);
    ASSERT_GE(client.lastPayload.size(), sizeof(uint8_t));

    uint8_t clearedCount;
    std::memcpy(&clearedCount, client.lastPayload.data(), sizeof(clearedCount));
    EXPECT_EQ(clearedCount, 3u);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// RunLoop_Dispatch — RunLoop-driven server handles requests
// ═══════════════════════════════════════════════════════════════════

TEST(CanBusEcuTest, RunLoop_Dispatch)
{
    ms::RunLoop loop;
    loop.init("SrvRL");

    TestCanBusEcu svc(SVC_NAME, &loop);
    svc.addDtc("P0301", Warning, "Cylinder 1 Misfire");
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // GetDtcCount via RunLoop-driven server.
    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(CanBusEcu::kServiceId, CanBusEcu::kGetDtcCount, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint8_t));

    uint8_t count;
    std::memcpy(&count, response.data(), sizeof(count));
    EXPECT_EQ(count, 1u);

    client.disconnect();
    svc.stop();
}
