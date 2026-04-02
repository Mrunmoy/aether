// SysMon IPC server — reads /proc for CPU, memory, and load average data.
//
// Run:   ./sysmon_device
// Stop:  Ctrl-C

#include "SysMon.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

using namespace aether::ipc;

static std::atomic<bool> g_running{true};

// ── /proc parsers ───────────────────────────────────────────────────

struct CpuJiffies
{
    uint64_t user = 0;
    uint64_t nice = 0;
    uint64_t system = 0;
    uint64_t idle = 0;
    uint64_t iowait = 0;
    uint64_t irq = 0;
    uint64_t softirq = 0;
    uint64_t steal = 0;

    uint64_t totalBusy() const
    {
        return user + nice + system + irq + softirq + steal;
    }

    uint64_t totalIdle() const
    {
        return idle + iowait;
    }

    uint64_t total() const
    {
        return totalBusy() + totalIdle();
    }
};

static bool readCpuJiffies(CpuJiffies *out)
{
    std::ifstream fs("/proc/stat");
    if (!fs.is_open())
        return false;

    std::string line;
    if (!std::getline(fs, line))
        return false;

    // First line: "cpu  user nice system idle iowait irq softirq steal ..."
    if (line.compare(0, 3, "cpu") != 0)
        return false;

    std::istringstream iss(line.substr(3));
    iss >> out->user >> out->nice >> out->system >> out->idle
        >> out->iowait >> out->irq >> out->softirq >> out->steal;

    return !iss.fail();
}

static bool readMemoryInfo(MemoryInfo *info)
{
    std::ifstream fs("/proc/meminfo");
    if (!fs.is_open())
        return false;

    std::string line;
    int found = 0;
    while (std::getline(fs, line) && found < 5)
    {
        uint64_t val = 0;
        if (std::sscanf(line.c_str(), "MemTotal: %lu kB", &val) == 1)
        {
            info->totalKb = val;
            ++found;
        }
        else if (std::sscanf(line.c_str(), "MemFree: %lu kB", &val) == 1)
        {
            info->freeKb = val;
            ++found;
        }
        else if (std::sscanf(line.c_str(), "MemAvailable: %lu kB", &val) == 1)
        {
            info->availableKb = val;
            ++found;
        }
        else if (std::sscanf(line.c_str(), "Buffers: %lu kB", &val) == 1)
        {
            info->buffersKb = val;
            ++found;
        }
        else if (std::sscanf(line.c_str(), "Cached: %lu kB", &val) == 1)
        {
            info->cachedKb = val;
            ++found;
        }
    }

    if (info->totalKb > 0)
    {
        info->usedKb = info->totalKb - info->availableKb;
        info->usedPercent = 100.0f * static_cast<float>(info->usedKb)
                            / static_cast<float>(info->totalKb);
    }
    return found == 5;
}

static bool readLoadAverage(LoadAverage *load)
{
    std::ifstream fs("/proc/loadavg");
    if (!fs.is_open())
        return false;

    // Format: "0.12 0.34 0.56 2/345 6789"
    unsigned running = 0;
    unsigned total = 0;
    char slash = 0;

    std::string line;
    if (!std::getline(fs, line))
        return false;

    std::istringstream iss(line);
    iss >> load->avg1 >> load->avg5 >> load->avg15 >> running >> slash >> total;

    // The running/total field is "running/total" — parse manually
    // Re-parse: "2/345"
    const char *p = line.c_str();
    // Skip three floats
    for (int i = 0; i < 3; ++i)
    {
        while (*p == ' ')
            ++p;
        while (*p && *p != ' ')
            ++p;
    }
    while (*p == ' ')
        ++p;
    if (std::sscanf(p, "%u/%u", &running, &total) == 2)
    {
        load->runningProcs = running;
        load->totalProcs = total;
    }

    return true;
}

// ── SysMon server implementation ────────────────────────────────────

class SysMonDevice : public SysMon
{
public:
    SysMonDevice(const char *name, float cpuThreshold = 90.0f,
                 float memThreshold = 85.0f)
        : SysMon(name)
        , m_cpuThreshold(cpuThreshold)
        , m_memThreshold(memThreshold)
    {
    }

    ~SysMonDevice() override
    {
        stopMonitor();
    }

    void startMonitor()
    {
        m_monitorRunning.store(true);
        readCpuJiffies(&m_prevJiffies);
        m_monitorThread = std::thread(&SysMonDevice::monitorLoop, this);
    }

    void stopMonitor()
    {
        m_monitorRunning.store(false);
        if (m_monitorThread.joinable())
            m_monitorThread.join();
    }

    void setCpuThreshold(float t) { m_cpuThreshold.store(t); }
    void setMemThreshold(float t) { m_memThreshold.store(t); }

protected:
    int handleGetCpuUsage(CpuUsage *usage) override
    {
        CpuJiffies prev, curr;
        readCpuJiffies(&prev);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        readCpuJiffies(&curr);

        computeCpuUsage(prev, curr, usage);
        return IPC_SUCCESS;
    }

    int handleGetMemoryInfo(MemoryInfo *info) override
    {
        if (!readMemoryInfo(info))
            return -1;
        return IPC_SUCCESS;
    }

    int handleGetLoadAverage(LoadAverage *load) override
    {
        if (!readLoadAverage(load))
            return -1;
        return IPC_SUCCESS;
    }

private:
    void computeCpuUsage(const CpuJiffies &prev, const CpuJiffies &curr,
                         CpuUsage *usage)
    {
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
    }

    void monitorLoop()
    {
        CpuJiffies prev;
        readCpuJiffies(&prev);

        while (m_monitorRunning.load())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (!m_monitorRunning.load())
                break;

            // Check CPU
            CpuJiffies curr;
            if (readCpuJiffies(&curr))
            {
                CpuUsage usage{};
                computeCpuUsage(prev, curr, &usage);
                prev = curr;

                if (usage.total > m_cpuThreshold.load())
                {
                    notifyHighCpuAlert(usage.total);
                }
            }

            // Check memory
            MemoryInfo mem{};
            if (readMemoryInfo(&mem))
            {
                if (mem.usedPercent > m_memThreshold.load())
                {
                    notifyLowMemoryAlert(mem.usedPercent, mem.availableKb);
                }
            }
        }
    }

    std::atomic<float> m_cpuThreshold;
    std::atomic<float> m_memThreshold;
    std::atomic<bool> m_monitorRunning{false};
    std::thread m_monitorThread;
    CpuJiffies m_prevJiffies{};
};

// ── main ────────────────────────────────────────────────────────────

int main()
{
    std::signal(SIGINT, [](int) { g_running.store(false); });

    SysMonDevice service("sysmon");
    if (!service.start())
    {
        std::fprintf(stderr, "Failed to start SysMon service\n");
        return 1;
    }

    service.startMonitor();

    std::printf("╔══════════════════════════════════════╗\n");
    std::printf("║  SysMon service running              ║\n");
    std::printf("║  Service name: sysmon                ║\n");
    std::printf("║  CPU threshold:  90%%                 ║\n");
    std::printf("║  Memory threshold: 85%%               ║\n");
    std::printf("║  Ctrl-C to stop                      ║\n");
    std::printf("╚══════════════════════════════════════╝\n");

    while (g_running.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::printf("\nShutting down...\n");
    service.stopMonitor();
    service.stop();
    std::printf("Server stopped.\n");
    return 0;
}
