// CLI battery monitor client.
//
// Connects to the simulated battery device and polls status every 3 seconds,
// printing a formatted dashboard. Also prints notification alerts.
//
// Run:   ./battery_device   (in another terminal)
//        ./battery_client
// Stop:  Ctrl-C

#include "BatteryMonitor.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <thread>

static std::atomic<bool> g_running{true};

static const char *chargeStateName(aether::ipc::ChargeState s)
{
    switch (s)
    {
    case aether::ipc::Charging:    return "Charging";
    case aether::ipc::Discharging: return "Discharging";
    case aether::ipc::Full:        return "Full";
    case aether::ipc::NotCharging: return "NotCharging";
    default:                       return "?";
    }
}

static const char *powerSourceName(aether::ipc::PowerSource s)
{
    switch (s)
    {
    case aether::ipc::Battery: return "Battery";
    case aether::ipc::AC:      return "AC";
    case aether::ipc::UPS:     return "UPS";
    case aether::ipc::Unknown: return "Unknown";
    default:                   return "?";
    }
}

class BatteryClient : public aether::ipc::BatteryMonitor
{
public:
    using BatteryMonitor::BatteryMonitor;

protected:
    void onBatteryLevelChanged(float chargePercent, aether::ipc::ChargeState state) override
    {
        std::printf("  ⚡ Alert: Battery now %.0f%% [%s]\n", chargePercent, chargeStateName(state));
    }

    void onPowerSourceChanged(aether::ipc::PowerSource source) override
    {
        std::printf("  🔌 Alert: Power source changed to %s\n", powerSourceName(source));
    }
};

static void printBar(float percent)
{
    constexpr int kBarWidth = 10;
    int filled = static_cast<int>(percent / 100.0f * kBarWidth + 0.5f);
    if (filled < 0) filled = 0;
    if (filled > kBarWidth) filled = kBarWidth;
    for (int i = 0; i < filled; ++i)
        std::printf("\xe2\x96\x88"); // █
    for (int i = filled; i < kBarWidth; ++i)
        std::printf("\xe2\x96\x91"); // ░
}

int main()
{
#if !defined(_WIN32)
    std::signal(SIGINT, [](int) { g_running.store(false); });
#endif

    BatteryClient client("battery-monitor");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to battery-monitor service\n");
        return 1;
    }
    std::printf("Connected to battery-monitor service.\n\n");

    while (g_running.load())
    {
        aether::ipc::BatteryStatus status{};
        int rc = client.GetBatteryStatus(&status);
        if (rc != aether::ipc::IPC_SUCCESS)
        {
            std::fprintf(stderr, "GetBatteryStatus failed: %d\n", rc);
            break;
        }

        aether::ipc::PowerSource source{};
        rc = client.GetPowerSource(&source);
        if (rc != aether::ipc::IPC_SUCCESS)
        {
            std::fprintf(stderr, "GetPowerSource failed: %d\n", rc);
            break;
        }

        float health = 0;
        uint32_t cycles = 0;
        rc = client.GetBatteryHealth(&health, &cycles);
        if (rc != aether::ipc::IPC_SUCCESS)
        {
            std::fprintf(stderr, "GetBatteryHealth failed: %d\n", rc);
            break;
        }

        std::printf("  Battery: %.0f%% [%s] ", status.chargePercent, chargeStateName(status.state));
        printBar(status.chargePercent);
        std::printf(" %.2fV\n", status.voltageV);

        std::printf("  Power:   %s | Health: %.0f%% (%u cycles)\n",
                    powerSourceName(source), health, cycles);

        if (status.state == aether::ipc::Discharging && status.timeToEmptyMin > 0)
            std::printf("  Time remaining: %u min\n", status.timeToEmptyMin);
        else if (status.state == aether::ipc::Charging && status.timeToFullMin > 0)
            std::printf("  Time to full:   %u min\n", status.timeToFullMin);

        std::printf("\n");

        for (int i = 0; i < 30 && g_running.load(); ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    client.disconnect();
    std::printf("Disconnected.\n");
    return 0;
}
