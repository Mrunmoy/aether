// Simulated battery device server.
//
// Simulates a battery draining and charging cycle, broadcasting
// notifications when the charge level or power source changes.
// No platform-specific APIs — compiles and runs on any OS.
//
// Run:   ./battery_device
// Stop:  Ctrl-C

#include "BatteryMonitor.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <mutex>
#include <thread>

static std::atomic<bool> g_running{true};

class BatteryDevice : public aether::ipc::BatteryMonitor
{
public:
    using BatteryMonitor::BatteryMonitor;

    void startSimulation()
    {
        m_simThread = std::thread([this] { simulationLoop(); });
    }

    void stopSimulation()
    {
        m_simRunning.store(false);
        if (m_simThread.joinable())
            m_simThread.join();
    }

protected:
    int handleGetBatteryStatus(aether::ipc::BatteryStatus *status) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *status = m_status;
        return aether::ipc::IPC_SUCCESS;
    }

    int handleGetPowerSource(aether::ipc::PowerSource *source) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *source = m_powerSource;
        return aether::ipc::IPC_SUCCESS;
    }

    int handleGetBatteryHealth(float *healthPercent, uint32_t *cycleCount) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *healthPercent = m_status.healthPercent;
        *cycleCount = m_status.cycleCount;
        return aether::ipc::IPC_SUCCESS;
    }

private:
    static constexpr float kMinVoltage = 3.7f;
    static constexpr float kMaxVoltage = 4.2f;
    static constexpr uint32_t kDrainIntervalMs = 2000;
    static constexpr uint32_t kChargeIntervalMs = 1500;
    static constexpr float kLowBatteryThreshold = 20.0f;

    float voltageForCharge(float percent) const
    {
        return kMinVoltage + (kMaxVoltage - kMinVoltage) * (percent / 100.0f);
    }

    void simulationLoop()
    {
        while (m_simRunning.load() && g_running.load())
        {
            aether::ipc::ChargeState prevState;
            aether::ipc::PowerSource prevSource;
            float prevCharge;

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                prevState = m_status.state;
                prevSource = m_powerSource;
                prevCharge = m_status.chargePercent;

                switch (m_status.state)
                {
                case aether::ipc::Discharging:
                {
                    m_status.chargePercent -= 1.0f;
                    if (m_status.chargePercent <= kLowBatteryThreshold)
                    {
                        m_status.state = aether::ipc::Charging;
                        m_powerSource = aether::ipc::AC;
                        m_status.timeToEmptyMin = 0;
                    }
                    m_status.timeToEmptyMin = static_cast<uint32_t>(
                        m_status.chargePercent * (kDrainIntervalMs / 60000.0f));
                    m_status.timeToFullMin = 0;
                    break;
                }
                case aether::ipc::Charging:
                {
                    m_status.chargePercent += 1.0f;
                    if (m_status.chargePercent >= 100.0f)
                    {
                        m_status.chargePercent = 100.0f;
                        m_status.state = aether::ipc::Full;
                    }
                    float remaining = 100.0f - m_status.chargePercent;
                    m_status.timeToFullMin = static_cast<uint32_t>(remaining * 1.5f / 60.0f * 60.0f);
                    m_status.timeToEmptyMin = 0;
                    break;
                }
                case aether::ipc::Full:
                {
                    // After a brief pause at full, unplug and start discharging
                    m_status.state = aether::ipc::Discharging;
                    m_powerSource = aether::ipc::Battery;
                    m_status.timeToFullMin = 0;
                    break;
                }
                default:
                    break;
                }

                m_status.voltageV = voltageForCharge(m_status.chargePercent);
            }

            // Fire notifications outside the lock
            float currentCharge;
            aether::ipc::ChargeState currentState;
            aether::ipc::PowerSource currentSource;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                currentCharge = m_status.chargePercent;
                currentState = m_status.state;
                currentSource = m_powerSource;
            }

            if (currentCharge != prevCharge || currentState != prevState)
            {
                notifyBatteryLevelChanged(currentCharge, currentState);
            }
            if (currentSource != prevSource)
            {
                notifyPowerSourceChanged(currentSource);
            }

            uint32_t sleepMs = (prevState == aether::ipc::Charging)
                                   ? kChargeIntervalMs
                                   : kDrainIntervalMs;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
        }
    }

    std::mutex m_mutex;
    std::atomic<bool> m_simRunning{true};
    std::thread m_simThread;
    aether::ipc::PowerSource m_powerSource = aether::ipc::Battery;
    aether::ipc::BatteryStatus m_status{
        aether::ipc::Discharging, // state
        100.0f,                   // chargePercent
        0,                        // timeToEmptyMin
        0,                        // timeToFullMin
        4.2f,                     // voltageV
        500,                      // cycleCount
        92.0f,                    // healthPercent
    };
};

int main()
{
#if !defined(_WIN32)
    std::signal(SIGINT, [](int) { g_running.store(false); });
#endif

    BatteryDevice device("battery-monitor");
    if (!device.start())
    {
        std::fprintf(stderr, "Failed to start battery device service\n");
        return 1;
    }

    device.startSimulation();

    std::printf("╔══════════════════════════════════════╗\n");
    std::printf("║   Battery Monitor Device (simulated) ║\n");
    std::printf("║   Service: battery-monitor           ║\n");
    std::printf("║   Ctrl-C to stop                     ║\n");
    std::printf("╚══════════════════════════════════════╝\n");

    while (g_running.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    device.stopSimulation();
    device.stop();
    std::printf("Battery device stopped.\n");
    return 0;
}
