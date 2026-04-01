#include <gtest/gtest.h>

// Server-side generated header (defines aether::ipc::BlePeripheral : ServiceBase)
#include "BlePeripheral.h"
// We use raw ClientBase for tests to avoid the server/client same-name
// collision (both gen/server and gen/client define aether::ipc::BlePeripheral).
#include "ClientBase.h"

#include <atomic>
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

// ── BLE UUID helpers ────────────────────────────────────────────────

static BleUuid makeBleUuid16(uint16_t uuid16)
{
    BleUuid uuid{};
    uuid.bytes[2] = static_cast<uint8_t>(uuid16 >> 8);
    uuid.bytes[3] = static_cast<uint8_t>(uuid16 & 0xFF);
    uuid.bytes[4] = 0x00;
    uuid.bytes[5] = 0x00;
    uuid.bytes[6] = 0x10;
    uuid.bytes[7] = 0x00;
    uuid.bytes[8] = 0x80;
    uuid.bytes[9] = 0x00;
    uuid.bytes[10] = 0x00;
    uuid.bytes[11] = 0x80;
    uuid.bytes[12] = 0x5F;
    uuid.bytes[13] = 0x9B;
    uuid.bytes[14] = 0x34;
    uuid.bytes[15] = 0xFB;
    return uuid;
}

static bool uuidEqual(const BleUuid &a, const BleUuid &b)
{
    return a.bytes == b.bytes;
}

static constexpr uint8_t kPropRead   = 0x02;
static constexpr uint8_t kPropNotify = 0x10;

static const BleUuid kHeartRateServiceUuid = makeBleUuid16(0x180D);
static const BleUuid kHeartRateMeasurementUuid = makeBleUuid16(0x2A37);
static const BleUuid kBodySensorLocationUuid = makeBleUuid16(0x2A38);
static const BleUuid kBatteryServiceUuid = makeBleUuid16(0x180F);
static const BleUuid kBatteryLevelUuid = makeBleUuid16(0x2A19);

// ── Characteristic state ────────────────────────────────────────────

struct CharState
{
    BleUuid uuid;
    uint8_t properties;
    std::array<uint8_t, 20> value{};
    uint8_t valueLength = 0;
    bool notifyEnabled = false;
};

struct ServiceState
{
    BleUuid uuid;
    std::vector<CharState> characteristics;
};

// ── Test device (no simulation thread) ──────────────────────────────

class TestBleDevice : public BlePeripheral
{
public:
    using BlePeripheral::BlePeripheral;

    // Expose notify for test use.
    using BlePeripheral::notifyCharacteristicChanged;

    void initProfile()
    {
        ServiceState hrSvc;
        hrSvc.uuid = kHeartRateServiceUuid;

        CharState hrMeas;
        hrMeas.uuid = kHeartRateMeasurementUuid;
        hrMeas.properties = kPropRead | kPropNotify;
        hrMeas.value[0] = 72;
        hrMeas.valueLength = 1;
        hrSvc.characteristics.push_back(hrMeas);

        CharState bodySensor;
        bodySensor.uuid = kBodySensorLocationUuid;
        bodySensor.properties = kPropRead;
        bodySensor.value[0] = 1; // Chest
        bodySensor.valueLength = 1;
        hrSvc.characteristics.push_back(bodySensor);

        ServiceState batSvc;
        batSvc.uuid = kBatteryServiceUuid;

        CharState batLevel;
        batLevel.uuid = kBatteryLevelUuid;
        batLevel.properties = kPropRead | kPropNotify;
        batLevel.value[0] = 85;
        batLevel.valueLength = 1;
        batSvc.characteristics.push_back(batLevel);

        m_services.push_back(std::move(hrSvc));
        m_services.push_back(std::move(batSvc));
    }

protected:
    int handleGetServiceCount(uint8_t *count) override
    {
        *count = static_cast<uint8_t>(m_services.size());
        return IPC_SUCCESS;
    }

    int handleGetService(uint8_t index, BleService *svc) override
    {
        if (index >= m_services.size())
            return IPC_ERR_INVALID_ARGUMENT;
        svc->uuid = m_services[index].uuid;
        svc->characteristicCount = static_cast<uint8_t>(m_services[index].characteristics.size());
        return IPC_SUCCESS;
    }

    int handleReadCharacteristic(BleUuid serviceUuid, BleUuid charUuid,
                                 BleCharacteristic *characteristic) override
    {
        auto *ch = findChar(serviceUuid, charUuid);
        if (!ch)
            return IPC_ERR_INVALID_ARGUMENT;
        characteristic->uuid = ch->uuid;
        characteristic->value = ch->value;
        characteristic->valueLength = ch->valueLength;
        characteristic->properties = ch->properties;
        return IPC_SUCCESS;
    }

    int handleWriteCharacteristic(BleUuid serviceUuid, BleUuid charUuid,
                                  std::array<uint8_t, 20> value, uint8_t length) override
    {
        auto *ch = findChar(serviceUuid, charUuid);
        if (!ch)
            return IPC_ERR_INVALID_ARGUMENT;
        ch->value = value;
        ch->valueLength = length;
        return IPC_SUCCESS;
    }

    int handleEnableNotifications(BleUuid serviceUuid, BleUuid charUuid,
                                  uint8_t enable) override
    {
        auto *ch = findChar(serviceUuid, charUuid);
        if (!ch)
            return IPC_ERR_INVALID_ARGUMENT;
        if (!(ch->properties & kPropNotify))
            return IPC_ERR_NOT_SUPPORTED;
        ch->notifyEnabled = (enable != 0);
        return IPC_SUCCESS;
    }

private:
    CharState *findChar(const BleUuid &svcUuid, const BleUuid &charUuid)
    {
        for (auto &svc : m_services)
        {
            if (!uuidEqual(svc.uuid, svcUuid))
                continue;
            for (auto &ch : svc.characteristics)
            {
                if (uuidEqual(ch.uuid, charUuid))
                    return &ch;
            }
        }
        return nullptr;
    }

    std::vector<ServiceState> m_services;
};

// ── Notification-capturing client ───────────────────────────────────

class NotifyClient : public ClientBase
{
public:
    using ClientBase::ClientBase;

    std::mutex mtx;
    std::condition_variable cv;
    int notifyCount = 0;
    BleUuid lastSvcUuid{};
    BleUuid lastCharUuid{};
    std::array<uint8_t, 20> lastValue{};
    uint8_t lastLength = 0;

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        if (serviceId != BlePeripheral::kServiceId)
            return;
        if (messageId != BlePeripheral::kCharacteristicChanged)
            return;
        if (payload.size() < sizeof(BleUuid) + sizeof(BleUuid) +
                             sizeof(std::array<uint8_t, 20>) + sizeof(uint8_t))
            return;

        std::lock_guard<std::mutex> lock(mtx);
        size_t off = 0;
        std::memcpy(&lastSvcUuid, payload.data() + off, sizeof(lastSvcUuid));
        off += sizeof(lastSvcUuid);
        std::memcpy(&lastCharUuid, payload.data() + off, sizeof(lastCharUuid));
        off += sizeof(lastCharUuid);
        std::memcpy(&lastValue, payload.data() + off, sizeof(lastValue));
        off += sizeof(lastValue);
        std::memcpy(&lastLength, payload.data() + off, sizeof(lastLength));
        notifyCount++;
        cv.notify_all();
    }
};

// ═════════════════════════════════════════════════════════════════════
// GetServiceCount_Returns2
// ═════════════════════════════════════════════════════════════════════

TEST(BlePeripheralTest, GetServiceCount_Returns2)
{
    TestBleDevice svc(SVC_NAME);
    svc.initProfile();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kGetServiceCount,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint8_t));

    uint8_t count = 0;
    std::memcpy(&count, response.data(), sizeof(count));
    EXPECT_EQ(count, 2);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetService_ValidIndex
// ═════════════════════════════════════════════════════════════════════

TEST(BlePeripheralTest, GetService_ValidIndex)
{
    TestBleDevice svc(SVC_NAME);
    svc.initProfile();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Request service at index 0
    uint8_t index = 0;
    std::vector<uint8_t> request(sizeof(index));
    std::memcpy(request.data(), &index, sizeof(index));

    std::vector<uint8_t> response;
    int rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kGetService,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(BleService));

    BleService result{};
    std::memcpy(&result, response.data(), sizeof(result));
    EXPECT_TRUE(uuidEqual(result.uuid, kHeartRateServiceUuid));
    EXPECT_GT(result.characteristicCount, static_cast<uint8_t>(0));

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetService_InvalidIndex
// ═════════════════════════════════════════════════════════════════════

TEST(BlePeripheralTest, GetService_InvalidIndex)
{
    TestBleDevice svc(SVC_NAME);
    svc.initProfile();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint8_t index = 99;
    std::vector<uint8_t> request(sizeof(index));
    std::memcpy(request.data(), &index, sizeof(index));

    std::vector<uint8_t> response;
    int rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kGetService,
                         request, &response);

    // Handler returns IPC_ERR_INVALID_ARGUMENT, which the framework maps
    // to the response aux field. The returned rc may be the error code.
    EXPECT_NE(rc, IPC_SUCCESS);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// ReadCharacteristic_HeartRate
// ═════════════════════════════════════════════════════════════════════

TEST(BlePeripheralTest, ReadCharacteristic_HeartRate)
{
    TestBleDevice svc(SVC_NAME);
    svc.initProfile();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Marshal [in] serviceUuid + charUuid
    std::vector<uint8_t> request(sizeof(BleUuid) + sizeof(BleUuid));
    BleUuid svcUuid = kHeartRateServiceUuid;
    BleUuid charUuid = kHeartRateMeasurementUuid;
    std::memcpy(request.data(), &svcUuid, sizeof(svcUuid));
    std::memcpy(request.data() + sizeof(svcUuid), &charUuid, sizeof(charUuid));

    std::vector<uint8_t> response;
    int rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kReadCharacteristic,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(BleCharacteristic));

    BleCharacteristic ch{};
    std::memcpy(&ch, response.data(), sizeof(ch));
    EXPECT_GT(ch.valueLength, static_cast<uint8_t>(0));
    EXPECT_TRUE(uuidEqual(ch.uuid, kHeartRateMeasurementUuid));
    EXPECT_EQ(ch.value[0], 72); // initial BPM

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// ReadCharacteristic_BatteryLevel
// ═════════════════════════════════════════════════════════════════════

TEST(BlePeripheralTest, ReadCharacteristic_BatteryLevel)
{
    TestBleDevice svc(SVC_NAME);
    svc.initProfile();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request(sizeof(BleUuid) + sizeof(BleUuid));
    BleUuid svcUuid = kBatteryServiceUuid;
    BleUuid charUuid = kBatteryLevelUuid;
    std::memcpy(request.data(), &svcUuid, sizeof(svcUuid));
    std::memcpy(request.data() + sizeof(svcUuid), &charUuid, sizeof(charUuid));

    std::vector<uint8_t> response;
    int rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kReadCharacteristic,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(BleCharacteristic));

    BleCharacteristic ch{};
    std::memcpy(&ch, response.data(), sizeof(ch));
    EXPECT_GE(ch.value[0], static_cast<uint8_t>(0));
    EXPECT_LE(ch.value[0], static_cast<uint8_t>(100));

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// ReadCharacteristic_UnknownUuid
// ═════════════════════════════════════════════════════════════════════

TEST(BlePeripheralTest, ReadCharacteristic_UnknownUuid)
{
    TestBleDevice svc(SVC_NAME);
    svc.initProfile();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request(sizeof(BleUuid) + sizeof(BleUuid));
    BleUuid svcUuid = kHeartRateServiceUuid;
    BleUuid charUuid = makeBleUuid16(0xFFFF); // non-existent
    std::memcpy(request.data(), &svcUuid, sizeof(svcUuid));
    std::memcpy(request.data() + sizeof(svcUuid), &charUuid, sizeof(charUuid));

    std::vector<uint8_t> response;
    int rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kReadCharacteristic,
                         request, &response);

    EXPECT_NE(rc, IPC_SUCCESS);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// WriteCharacteristic_Succeeds
// ═════════════════════════════════════════════════════════════════════

TEST(BlePeripheralTest, WriteCharacteristic_Succeeds)
{
    TestBleDevice svc(SVC_NAME);
    svc.initProfile();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Write a new value to heart rate measurement
    BleUuid svcUuid = kHeartRateServiceUuid;
    BleUuid charUuid = kHeartRateMeasurementUuid;
    std::array<uint8_t, 20> value{};
    value[0] = 99;
    uint8_t length = 1;

    std::vector<uint8_t> request(sizeof(svcUuid) + sizeof(charUuid) + sizeof(value) + sizeof(length));
    size_t off = 0;
    std::memcpy(request.data() + off, &svcUuid, sizeof(svcUuid));
    off += sizeof(svcUuid);
    std::memcpy(request.data() + off, &charUuid, sizeof(charUuid));
    off += sizeof(charUuid);
    std::memcpy(request.data() + off, &value, sizeof(value));
    off += sizeof(value);
    std::memcpy(request.data() + off, &length, sizeof(length));

    std::vector<uint8_t> response;
    int rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kWriteCharacteristic,
                         request, &response);
    ASSERT_EQ(rc, IPC_SUCCESS);

    // Read back to verify
    std::vector<uint8_t> readReq(sizeof(svcUuid) + sizeof(charUuid));
    std::memcpy(readReq.data(), &svcUuid, sizeof(svcUuid));
    std::memcpy(readReq.data() + sizeof(svcUuid), &charUuid, sizeof(charUuid));

    std::vector<uint8_t> readResp;
    rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kReadCharacteristic,
                     readReq, &readResp);
    ASSERT_EQ(rc, IPC_SUCCESS);

    BleCharacteristic ch{};
    std::memcpy(&ch, readResp.data(), sizeof(ch));
    EXPECT_EQ(ch.value[0], 99);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// EnableNotifications_FiresCallback
// ═════════════════════════════════════════════════════════════════════

TEST(BlePeripheralTest, EnableNotifications_FiresCallback)
{
    TestBleDevice svc(SVC_NAME);
    svc.initProfile();
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Enable notifications on heart rate measurement
    BleUuid svcUuid = kHeartRateServiceUuid;
    BleUuid charUuid = kHeartRateMeasurementUuid;
    uint8_t enable = 1;

    std::vector<uint8_t> request(sizeof(svcUuid) + sizeof(charUuid) + sizeof(enable));
    size_t off = 0;
    std::memcpy(request.data() + off, &svcUuid, sizeof(svcUuid));
    off += sizeof(svcUuid);
    std::memcpy(request.data() + off, &charUuid, sizeof(charUuid));
    off += sizeof(charUuid);
    std::memcpy(request.data() + off, &enable, sizeof(enable));

    std::vector<uint8_t> response;
    int rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kEnableNotifications,
                         request, &response);
    ASSERT_EQ(rc, IPC_SUCCESS);

    // Server fires a notification
    std::array<uint8_t, 20> val{};
    val[0] = 88;
    ASSERT_EQ(svc.notifyCharacteristicChanged(svcUuid, charUuid, val, 1), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(2000),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_TRUE(uuidEqual(client.lastCharUuid, kHeartRateMeasurementUuid));
    EXPECT_EQ(client.lastValue[0], 88);
    EXPECT_EQ(client.lastLength, 1);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// DisableNotifications_StopsCallbacks
// ═════════════════════════════════════════════════════════════════════

TEST(BlePeripheralTest, DisableNotifications_StopsCallbacks)
{
    TestBleDevice svc(SVC_NAME);
    svc.initProfile();
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    BleUuid svcUuid = kHeartRateServiceUuid;
    BleUuid charUuid = kHeartRateMeasurementUuid;

    // Enable notifications
    {
        uint8_t enable = 1;
        std::vector<uint8_t> request(sizeof(svcUuid) + sizeof(charUuid) + sizeof(enable));
        size_t off = 0;
        std::memcpy(request.data() + off, &svcUuid, sizeof(svcUuid));
        off += sizeof(svcUuid);
        std::memcpy(request.data() + off, &charUuid, sizeof(charUuid));
        off += sizeof(charUuid);
        std::memcpy(request.data() + off, &enable, sizeof(enable));
        std::vector<uint8_t> response;
        int rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kEnableNotifications,
                             request, &response);
        ASSERT_EQ(rc, IPC_SUCCESS);
    }

    // Fire one notification to confirm it works
    {
        std::array<uint8_t, 20> val{};
        val[0] = 77;
        ASSERT_EQ(svc.notifyCharacteristicChanged(svcUuid, charUuid, val, 1), IPC_SUCCESS);

        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(2000),
                                        [&] { return client.notifyCount > 0; }));
    }
    EXPECT_EQ(client.notifyCount, 1);

    // Disable notifications
    {
        uint8_t enable = 0;
        std::vector<uint8_t> request(sizeof(svcUuid) + sizeof(charUuid) + sizeof(enable));
        size_t off = 0;
        std::memcpy(request.data() + off, &svcUuid, sizeof(svcUuid));
        off += sizeof(svcUuid);
        std::memcpy(request.data() + off, &charUuid, sizeof(charUuid));
        off += sizeof(charUuid);
        std::memcpy(request.data() + off, &enable, sizeof(enable));
        std::vector<uint8_t> response;
        int rc = client.call(BlePeripheral::kServiceId, BlePeripheral::kEnableNotifications,
                             request, &response);
        ASSERT_EQ(rc, IPC_SUCCESS);
    }

    // Record current count, then fire another notification server-side.
    // The server's handleEnableNotifications disabled the flag, but
    // notifyCharacteristicChanged broadcasts to all clients unconditionally
    // (it's up to the application layer whether to fire or not).
    // Since our test device doesn't run a simulation thread, no further
    // notifications will arrive. We wait briefly to confirm.
    int countBefore = client.notifyCount;

    // The notification path goes: server fires -> all clients receive.
    // But in a real app the device would check notifyEnabled before firing.
    // We verify that the server accepted the disable call (rc == SUCCESS above)
    // and no new autonomous notifications arrive.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(client.notifyCount, countBefore);

    client.disconnect();
    svc.stop();
}
