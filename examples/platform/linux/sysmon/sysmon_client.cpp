// SysMon IPC client — connects to sysmon_device, queries system stats,
// and listens for threshold notifications.
//
// Run:   ./sysmon_device   (in another terminal)
//        ./sysmon_client

#include "SysMon.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <thread>

using namespace aether::ipc;

static std::atomic<bool> g_running{true};

// ── SysMon client with notification handlers ────────────────────────

class SysMonClient : public SysMon
{
public:
    using SysMon::SysMon;

protected:
    void onHighCpuAlert(float cpuPercent) override
    {
        std::printf("\n⚠  HIGH CPU ALERT: %.1f%%\n", static_cast<double>(cpuPercent));
    }

    void onLowMemoryAlert(float usedPercent, uint64_t availableKb) override
    {
        std::printf("\n⚠  LOW MEMORY ALERT: %.1f%% used, %lu KB available\n",
                    static_cast<double>(usedPercent),
                    static_cast<unsigned long>(availableKb));
    }
};

// ── Helpers ─────────────────────────────────────────────────────────

static void printCpuUsage(const CpuUsage &cpu)
{
    std::printf("  %-14s %6.1f %%\n", "User:", static_cast<double>(cpu.user));
    std::printf("  %-14s %6.1f %%\n", "System:", static_cast<double>(cpu.system));
    std::printf("  %-14s %6.1f %%\n", "IO Wait:", static_cast<double>(cpu.iowait));
    std::printf("  %-14s %6.1f %%\n", "Idle:", static_cast<double>(cpu.idle));
    std::printf("  %-14s %6.1f %%\n", "Total:", static_cast<double>(cpu.total));
}

static void printMemoryInfo(const MemoryInfo &mem)
{
    auto toMb = [](uint64_t kb) { return static_cast<double>(kb) / 1024.0; };
    std::printf("  %-14s %8.1f MB\n", "Total:", toMb(mem.totalKb));
    std::printf("  %-14s %8.1f MB\n", "Used:", toMb(mem.usedKb));
    std::printf("  %-14s %8.1f MB\n", "Free:", toMb(mem.freeKb));
    std::printf("  %-14s %8.1f MB\n", "Available:", toMb(mem.availableKb));
    std::printf("  %-14s %8.1f MB\n", "Buffers:", toMb(mem.buffersKb));
    std::printf("  %-14s %8.1f MB\n", "Cached:", toMb(mem.cachedKb));
    std::printf("  %-14s %6.1f %%\n", "Used:", static_cast<double>(mem.usedPercent));
}

static void printLoadAverage(const LoadAverage &load)
{
    std::printf("  %-14s %6.2f\n", "1 min:", static_cast<double>(load.avg1));
    std::printf("  %-14s %6.2f\n", "5 min:", static_cast<double>(load.avg5));
    std::printf("  %-14s %6.2f\n", "15 min:", static_cast<double>(load.avg15));
    std::printf("  %-14s %u / %u\n", "Procs:", load.runningProcs, load.totalProcs);
}

static void querySysmon(SysMonClient &client)
{
    CpuUsage cpu{};
    MemoryInfo mem{};
    LoadAverage load{};

    std::printf("┌──────────────────────────────────────┐\n");

    int rc = client.GetCpuUsage(&cpu);
    if (rc == IPC_SUCCESS)
    {
        std::printf("│ CPU Usage                            │\n");
        std::printf("├──────────────────────────────────────┤\n");
        printCpuUsage(cpu);
    }
    else
    {
        std::printf("│ CPU Usage: error %d                   │\n", rc);
    }

    std::printf("├──────────────────────────────────────┤\n");

    rc = client.GetMemoryInfo(&mem);
    if (rc == IPC_SUCCESS)
    {
        std::printf("│ Memory                               │\n");
        std::printf("├──────────────────────────────────────┤\n");
        printMemoryInfo(mem);
    }
    else
    {
        std::printf("│ Memory: error %d                      │\n", rc);
    }

    std::printf("├──────────────────────────────────────┤\n");

    rc = client.GetLoadAverage(&load);
    if (rc == IPC_SUCCESS)
    {
        std::printf("│ Load Average                         │\n");
        std::printf("├──────────────────────────────────────┤\n");
        printLoadAverage(load);
    }
    else
    {
        std::printf("│ Load Average: error %d                │\n", rc);
    }

    std::printf("└──────────────────────────────────────┘\n\n");
}

// ── main ────────────────────────────────────────────────────────────

int main()
{
    std::signal(SIGINT, [](int) { g_running.store(false); });

    SysMonClient client("sysmon");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to sysmon service\n");
        return 1;
    }
    std::printf("Connected to sysmon service.\n\n");

    while (g_running.load())
    {
        querySysmon(client);

        // Wait 2 seconds between updates, checking for Ctrl-C every 100ms
        for (int i = 0; i < 20 && g_running.load(); ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    client.disconnect();
    std::printf("\nDisconnected.\n");
    return 0;
}
