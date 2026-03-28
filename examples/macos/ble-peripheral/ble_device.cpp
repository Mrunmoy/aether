// Simulated BLE Heart Rate Monitor peripheral.
//
// Exposes a standard GATT profile over Aether IPC:
//   - Heart Rate Service (0x180D)
//       * Heart Rate Measurement (0x2A37) — read/notify
//       * Body Sensor Location  (0x2A38) — read-only
//   - Battery Service (0x180F)
//       * Battery Level (0x2A19) — read/notify
//
// Run:   ./ble_device
// Stop:  Ctrl-C

#include "BlePeripheral.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <csignal>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

using namespace aether::ipc;

// ── BLE UUID helpers ────────────────────────────────────────────────

// Bluetooth Base UUID: 00000000-0000-1000-8000-00805F9B34FB
static BleUuid makeBleUuid16(uint16_t uuid16)
{
    BleUuid uuid{};
    // Big-endian 128-bit layout per Bluetooth spec
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

// ── BLE property flags ──────────────────────────────────────────────

static constexpr uint8_t kPropRead   = 0x02;
static constexpr uint8_t kPropNotify = 0x10;

// ── Well-known UUIDs ────────────────────────────────────────────────

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

// ── BLE Heart Rate Monitor device ───────────────────────────────────

class HeartRateDevice : public BlePeripheral
{
public:
    using BlePeripheral::BlePeripheral;

    // Expose notify for the simulation thread.
    using BlePeripheral::notifyCharacteristicChanged;

    void initProfile()
    {
        // Heart Rate Service
        ServiceState hrSvc;
        hrSvc.uuid = kHeartRateServiceUuid;

        CharState hrMeas;
        hrMeas.uuid = kHeartRateMeasurementUuid;
        hrMeas.properties = kPropRead | kPropNotify;
        hrMeas.value[0] = 72; // initial BPM
        hrMeas.valueLength = 1;
        hrSvc.characteristics.push_back(hrMeas);

        CharState bodySensor;
        bodySensor.uuid = kBodySensorLocationUuid;
        bodySensor.properties = kPropRead;
        bodySensor.value[0] = 1; // Chest
        bodySensor.valueLength = 1;
        hrSvc.characteristics.push_back(bodySensor);

        // Battery Service
        ServiceState batSvc;
        batSvc.uuid = kBatteryServiceUuid;

        CharState batLevel;
        batLevel.uuid = kBatteryLevelUuid;
        batLevel.properties = kPropRead | kPropNotify;
        batLevel.value[0] = 100; // 100%
        batLevel.valueLength = 1;
        batSvc.characteristics.push_back(batLevel);

        std::lock_guard<std::mutex> lock(m_profileMutex);
        m_services.push_back(std::move(hrSvc));
        m_services.push_back(std::move(batSvc));
    }

    void startSimulation()
    {
        m_simRunning.store(true);
        m_simThread = std::thread([this]
        {
            std::mt19937 rng(42);
            std::uniform_int_distribution<int> hrDist(0, 40);
            int tick = 0;

            while (m_simRunning.load())
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                if (!m_simRunning.load())
                    break;

                // Update heart rate every second
                uint8_t bpm = static_cast<uint8_t>(60 + hrDist(rng));
                updateAndNotify(kHeartRateServiceUuid, kHeartRateMeasurementUuid, &bpm, 1);

                // Drain battery every 30 seconds
                tick++;
                if (tick % 30 == 0)
                {
                    uint8_t level = 0;
                    {
                        std::lock_guard<std::mutex> lock(m_profileMutex);
                        auto *ch = findChar(kBatteryServiceUuid, kBatteryLevelUuid);
                        if (ch && ch->value[0] > 0)
                            ch->value[0]--;
                        if (ch)
                            level = ch->value[0];
                    }
                    updateAndNotify(kBatteryServiceUuid, kBatteryLevelUuid, &level, 1);
                }
            }
        });
    }

    void stopSimulation()
    {
        m_simRunning.store(false);
        if (m_simThread.joinable())
            m_simThread.join();
    }

protected:
    int handleGetServiceCount(uint8_t *count) override
    {
        std::lock_guard<std::mutex> lock(m_profileMutex);
        *count = static_cast<uint8_t>(m_services.size());
        return IPC_SUCCESS;
    }

    int handleGetService(uint8_t index, BleService *svc) override
    {
        std::lock_guard<std::mutex> lock(m_profileMutex);
        if (index >= m_services.size())
            return IPC_ERR_INVALID_ARGUMENT;

        svc->uuid = m_services[index].uuid;
        svc->characteristicCount = static_cast<uint8_t>(m_services[index].characteristics.size());
        return IPC_SUCCESS;
    }

    int handleReadCharacteristic(BleUuid serviceUuid, BleUuid charUuid,
                                 BleCharacteristic *characteristic) override
    {
        std::lock_guard<std::mutex> lock(m_profileMutex);
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
        std::lock_guard<std::mutex> lock(m_profileMutex);
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
        std::lock_guard<std::mutex> lock(m_profileMutex);
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

    void updateAndNotify(const BleUuid &svcUuid, const BleUuid &charUuid,
                         const uint8_t *data, uint8_t len)
    {
        bool shouldNotify = false;
        std::array<uint8_t, 20> val{};
        {
            std::lock_guard<std::mutex> lock(m_profileMutex);
            auto *ch = findChar(svcUuid, charUuid);
            if (!ch)
                return;
            std::memcpy(ch->value.data(), data, len);
            ch->valueLength = len;
            if (ch->notifyEnabled)
            {
                shouldNotify = true;
                val = ch->value;
            }
        }
        if (shouldNotify)
            notifyCharacteristicChanged(svcUuid, charUuid, val, len);
    }

    std::mutex m_profileMutex;
    std::vector<ServiceState> m_services;
    std::atomic<bool> m_simRunning{false};
    std::thread m_simThread;
};

// ── main ────────────────────────────────────────────────────────────

static std::atomic<bool> g_running{true};

int main()
{
#if !defined(_WIN32)
    std::signal(SIGINT, [](int) { g_running.store(false); });
#endif

    HeartRateDevice device("BlePeripheral");
    device.initProfile();

    if (!device.start())
    {
        std::fprintf(stderr, "Failed to start BLE peripheral service\n");
        return 1;
    }

    std::printf("BLE Heart Rate Monitor running (Ctrl-C to stop)...\n");
    std::printf("Services:\n");
    std::printf("  [1] Heart Rate Service (0x180D) — 2 characteristics\n");
    std::printf("  [2] Battery Service    (0x180F) — 1 characteristic\n");

    device.startSimulation();

    while (g_running.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    device.stopSimulation();
    device.stop();
    std::printf("Device stopped.\n");
    return 0;
}
