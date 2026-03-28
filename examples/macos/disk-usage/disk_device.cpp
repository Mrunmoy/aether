// Simulated disk usage device server.
//
// Maintains three simulated volumes and slowly increases usage on "/"
// (1 GB every 3 seconds). Broadcasts DiskSpaceLow when "/" exceeds 90%.
// No platform-specific filesystem APIs — compiles on any OS.
//
// Run:   ./disk_device
// Stop:  Ctrl-C

#include "DiskUsage.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

static std::atomic<bool> g_running{true};

// ── Simulated disk device ───────────────────────────────────────────

class DiskUsageDevice : public aether::ipc::DiskUsage
{
public:
    DiskUsageDevice(const char *name) : DiskUsage(name)
    {
        initVolume("/",     "ext4",  500ULL * kGB, 350ULL * kGB);
        initVolume("/home", "ext4", 1000ULL * kGB, 800ULL * kGB);
        initVolume("/tmp",  "tmpfs",  50ULL * kGB,   5ULL * kGB);
    }

    void startSimulation()
    {
        m_simRunning.store(true);
        m_simThread = std::thread([this] { simulationLoop(); });
    }

    void stopSimulation()
    {
        m_simRunning.store(false);
        if (m_simThread.joinable())
            m_simThread.join();
    }

protected:
    int handleGetDiskUsage(const char *mountPoint, aether::ipc::VolumeInfo *info) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto &vol : m_volumes)
        {
            if (std::strcmp(vol.mountPoint, mountPoint) == 0)
            {
                *info = vol;
                return aether::ipc::IPC_SUCCESS;
            }
        }
        return 1; // mount point not found
    }

    int handleGetVolumeCount(uint32_t *count) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *count = static_cast<uint32_t>(m_volumes.size());
        return aether::ipc::IPC_SUCCESS;
    }

    int handleGetVolumeAt(uint32_t index, aether::ipc::VolumeInfo *info) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (index >= m_volumes.size())
            return 1; // index out of bounds
        *info = m_volumes[index];
        return aether::ipc::IPC_SUCCESS;
    }

private:
    static constexpr uint64_t kGB = 1024ULL * 1024ULL * 1024ULL;

    void initVolume(const char *mount, const char *fs,
                    uint64_t total, uint64_t used)
    {
        aether::ipc::VolumeInfo v{};
        std::strncpy(v.mountPoint, mount, sizeof(v.mountPoint) - 1);
        v.mountPoint[sizeof(v.mountPoint) - 1] = '\0';
        std::strncpy(v.filesystem, fs, sizeof(v.filesystem) - 1);
        v.filesystem[sizeof(v.filesystem) - 1] = '\0';
        v.totalBytes = total;
        v.usedBytes = used;
        v.freeBytes = total - used;
        v.usedPercent = 100.0f * static_cast<float>(used)
                        / static_cast<float>(total);
        m_volumes.push_back(v);
    }

    void simulationLoop()
    {
        std::printf("[disk] simulation started — '/' grows 1 GB every 3s\n");

        while (m_simRunning.load() && g_running.load())
        {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            if (!m_simRunning.load() || !g_running.load())
                break;

            bool alert = false;
            aether::ipc::VolumeInfo snapshot{};

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto &root = m_volumes[0];

                if (root.usedBytes + kGB <= root.totalBytes)
                {
                    root.usedBytes += kGB;
                    root.freeBytes = root.totalBytes - root.usedBytes;
                    root.usedPercent = 100.0f
                        * static_cast<float>(root.usedBytes)
                        / static_cast<float>(root.totalBytes);

                    std::printf("[disk] / usage: %.1f%% (%lu GB free)\n",
                                static_cast<double>(root.usedPercent),
                                static_cast<unsigned long>(root.freeBytes / kGB));

                    if (root.usedPercent > 90.0f)
                    {
                        alert = true;
                        snapshot = root;
                    }
                }
            }

            if (alert)
            {
                notifyDiskSpaceLow(snapshot.mountPoint,
                                   snapshot.usedPercent,
                                   snapshot.freeBytes);
            }
        }
    }

    std::mutex m_mutex;
    std::vector<aether::ipc::VolumeInfo> m_volumes;
    std::atomic<bool> m_simRunning{false};
    std::thread m_simThread;
};

// ── main ────────────────────────────────────────────────────────────

int main()
{
#if !defined(_WIN32)
    std::signal(SIGINT, [](int) { g_running.store(false); });
#endif

    DiskUsageDevice device("disk-usage");
    if (!device.start())
    {
        std::fprintf(stderr, "Failed to start disk-usage service\n");
        return 1;
    }

    device.startSimulation();

    std::printf("╔══════════════════════════════════════╗\n");
    std::printf("║  Disk Usage Monitor (simulated)      ║\n");
    std::printf("║  Service: disk-usage                 ║\n");
    std::printf("║  Volumes: /  /home  /tmp             ║\n");
    std::printf("║  '/' grows 1 GB every 3s             ║\n");
    std::printf("║  Alert when '/' > 90%%                ║\n");
    std::printf("║  Ctrl-C to stop                      ║\n");
    std::printf("╚══════════════════════════════════════╝\n");

    while (g_running.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    device.stopSimulation();
    device.stop();
    std::printf("Disk usage device stopped.\n");
    return 0;
}
