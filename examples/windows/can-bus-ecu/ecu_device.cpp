// Simulated automotive ECU server with OBD-II PID support.
//
// Run:   ./ecu_device
// Stop:  Ctrl-C

#include "CanBusEcu.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

using namespace aether::ipc;

static std::atomic<bool> g_running{true};

#if !defined(_WIN32)
static void signalHandler(int)
{
    g_running.store(false);
}
#endif

// ── ECU state machine ──────────────────────────────────────────────

enum class EcuState
{
    Idle,
    Accelerating,
    Cruising,
    Braking,
};

static const char *stateName(EcuState s)
{
    switch (s)
    {
    case EcuState::Idle:          return "Idle";
    case EcuState::Accelerating:  return "Accelerating";
    case EcuState::Cruising:      return "Cruising";
    case EcuState::Braking:       return "Braking";
    }
    return "Unknown";
}

// ── Simulated ECU device ───────────────────────────────────────────

class EcuDevice : public CanBusEcu
{
public:
    using CanBusEcu::CanBusEcu;

    void startSimulation()
    {
        m_simRunning.store(true);
        m_simThread = std::thread(&EcuDevice::simulationLoop, this);
    }

    void stopSimulation()
    {
        m_simRunning.store(false);
        if (m_simThread.joinable())
            m_simThread.join();
    }

protected:
    int handleReadPid(uint8_t pid, PidReading *reading) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        reading->pid = pid;
        switch (pid)
        {
        case 0x0C:
            reading->value = m_rpm;
            std::strncpy(reading->unit, "RPM", sizeof(reading->unit) - 1);
            break;
        case 0x0D:
            reading->value = m_speed;
            std::strncpy(reading->unit, "km/h", sizeof(reading->unit) - 1);
            break;
        case 0x05:
            reading->value = m_coolant;
            std::strncpy(reading->unit, "\xC2\xB0""C", sizeof(reading->unit) - 1);
            break;
        case 0x11:
            reading->value = m_throttle;
            std::strncpy(reading->unit, "%", sizeof(reading->unit) - 1);
            break;
        case 0x2F:
            reading->value = m_fuel;
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
        uint8_t cleared = 0;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            cleared = static_cast<uint8_t>(m_dtcs.size());
            *clearedCount = cleared;
            m_dtcs.clear();
        }
        if (cleared > 0)
            notifyDtcCleared(cleared);
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

private:
    static DtcEntry makeDtc(const char *code, DtcSeverity severity, const char *desc)
    {
        DtcEntry dtc{};
        std::strncpy(dtc.code, code, sizeof(dtc.code) - 1);
        dtc.severity = severity;
        std::strncpy(dtc.description, desc, sizeof(dtc.description) - 1);
        return dtc;
    }

    void simulationLoop()
    {
        EcuState state = EcuState::Idle;
        int cycleCount = 0;

        // Start with one stored DTC.
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_dtcs.push_back(makeDtc("P0301", Warning, "Cylinder 1 Misfire"));
        }

        while (m_simRunning.load())
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                switch (state)
                {
                case EcuState::Idle:
                    m_rpm = 800.0f;
                    m_speed = 0.0f;
                    m_throttle = 0.0f;
                    m_coolant = 90.0f;
                    break;
                case EcuState::Accelerating:
                    m_rpm = 4500.0f;
                    m_speed = 80.0f;
                    m_throttle = 70.0f;
                    m_coolant = 95.0f;
                    break;
                case EcuState::Cruising:
                    m_rpm = 2500.0f;
                    m_speed = 80.0f;
                    m_throttle = 25.0f;
                    m_coolant = 92.0f;
                    break;
                case EcuState::Braking:
                    m_rpm = 800.0f;
                    m_speed = 0.0f;
                    m_throttle = 0.0f;
                    m_coolant = 88.0f;
                    break;
                }
                // Slow fuel drain
                if (m_fuel > 5.0f)
                    m_fuel -= 0.5f;
            }

            std::printf("[ecu] State: %-14s  RPM=%.0f  Speed=%.0f  Throttle=%.0f%%\n",
                        stateName(state), m_rpm, m_speed, m_throttle);

            // Occasionally inject a new DTC every 4 full cycles.
            if (cycleCount > 0 && cycleCount % 16 == 0)
            {
                DtcEntry dtc = makeDtc("P0420", Warning,
                                       "Catalyst Efficiency Below Threshold");
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_dtcs.push_back(dtc);
                }
                std::printf("[ecu] New DTC: %s — %s\n", dtc.code, dtc.description);
                notifyCheckEngine(dtc);
            }

            // Advance state.
            switch (state)
            {
            case EcuState::Idle:          state = EcuState::Accelerating; break;
            case EcuState::Accelerating:  state = EcuState::Cruising;     break;
            case EcuState::Cruising:      state = EcuState::Braking;      break;
            case EcuState::Braking:       state = EcuState::Idle;         break;
            }
            cycleCount++;

            // Sleep 3s in small increments for responsive shutdown.
            for (int i = 0; i < 30 && m_simRunning.load(); ++i)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    std::mutex m_mutex;
    float m_rpm = 800.0f;
    float m_speed = 0.0f;
    float m_coolant = 90.0f;
    float m_throttle = 0.0f;
    float m_fuel = 72.0f;
    std::vector<DtcEntry> m_dtcs;
    std::atomic<bool> m_simRunning{false};
    std::thread m_simThread;
};

// ── main ───────────────────────────────────────────────────────────

int main()
{
#if !defined(_WIN32)
    std::signal(SIGINT, signalHandler);
#endif

    EcuDevice ecu("can-bus-ecu");
    if (!ecu.start())
    {
        std::fprintf(stderr, "Failed to start ECU service\n");
        return 1;
    }

    ecu.startSimulation();
    std::printf("ECU device running (Ctrl-C to stop)...\n");

    while (g_running.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ecu.stopSimulation();
    ecu.stop();
    std::printf("ECU device stopped.\n");
    return 0;
}
