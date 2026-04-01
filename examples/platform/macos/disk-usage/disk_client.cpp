// Interactive CLI disk usage monitor client.
//
// Connects to the simulated disk-usage service, lists volumes with
// a bar chart, and accepts commands: list, check <mount>, quit.
// Prints alerts when DiskSpaceLow notifications arrive.
//
// Run:   ./disk_device   (in another terminal)
//        ./disk_client
// Stop:  Ctrl-C or type "quit"

#include "DiskUsage.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <thread>

static std::atomic<bool> g_running{true};

// ── Helpers ─────────────────────────────────────────────────────────

static void formatBytes(uint64_t bytes, char *buf, size_t bufSize)
{
    constexpr uint64_t kGB = 1024ULL * 1024ULL * 1024ULL;
    constexpr uint64_t kTB = 1024ULL * kGB;

    if (bytes >= kTB)
    {
        std::snprintf(buf, bufSize, "%.1f TB",
                      static_cast<double>(bytes) / static_cast<double>(kTB));
    }
    else
    {
        std::snprintf(buf, bufSize, "%lu GB",
                      static_cast<unsigned long>(bytes / kGB));
    }
}

static void printBar(float percent)
{
    int filled = static_cast<int>(percent / 10.0f);
    if (filled < 0)  filled = 0;
    if (filled > 10) filled = 10;

    std::printf("[");
    for (int i = 0; i < 10; ++i)
        std::printf(i < filled ? "\xe2\x96\x88" : "\xe2\x96\x91");
    std::printf("] %3.0f%%", static_cast<double>(percent));
}

// ── Client subclass with notification handler ───────────────────────

class DiskUsageClient : public aether::ipc::DiskUsage
{
public:
    using DiskUsage::DiskUsage;

protected:
    void onDiskSpaceLow(const char *mountPoint, float usedPercent,
                        uint64_t freeBytes) override
    {
        char freeBuf[32];
        formatBytes(freeBytes, freeBuf, sizeof(freeBuf));
        std::printf("\n\xe2\x9a\xa0  DISK SPACE LOW: %s at %.1f%% (%s free)\n> ",
                    mountPoint, static_cast<double>(usedPercent), freeBuf);
        std::fflush(stdout);
    }
};

// ── Commands ────────────────────────────────────────────────────────

static void listVolumes(DiskUsageClient &client)
{
    uint32_t count = 0;
    if (client.GetVolumeCount(&count) != aether::ipc::IPC_SUCCESS || count == 0)
    {
        std::printf("  (no volumes)\n");
        return;
    }

    std::printf("%-12s %-12s %-10s %-10s %-10s %s\n",
                "Mount", "Filesystem", "Total", "Used", "Free", "Usage");
    std::printf("%-12s %-12s %-10s %-10s %-10s %s\n",
                "-----", "----------", "-----", "----", "----", "-----------------");

    for (uint32_t i = 0; i < count; ++i)
    {
        aether::ipc::VolumeInfo info{};
        if (client.GetVolumeAt(i, &info) != aether::ipc::IPC_SUCCESS)
            continue;

        char totalBuf[32], usedBuf[32], freeBuf[32];
        formatBytes(info.totalBytes, totalBuf, sizeof(totalBuf));
        formatBytes(info.usedBytes,  usedBuf,  sizeof(usedBuf));
        formatBytes(info.freeBytes,  freeBuf,  sizeof(freeBuf));

        std::printf("%-12s %-12s %-10s %-10s %-10s ",
                    info.mountPoint, info.filesystem,
                    totalBuf, usedBuf, freeBuf);
        printBar(info.usedPercent);
        std::printf("\n");
    }
}

static void checkMount(DiskUsageClient &client, const char *mount)
{
    aether::ipc::VolumeInfo info{};
    int rc = client.GetDiskUsage(mount, &info);
    if (rc != aether::ipc::IPC_SUCCESS)
    {
        std::printf("  Mount point '%s' not found (error %d)\n", mount, rc);
        return;
    }

    char totalBuf[32], usedBuf[32], freeBuf[32];
    formatBytes(info.totalBytes, totalBuf, sizeof(totalBuf));
    formatBytes(info.usedBytes,  usedBuf,  sizeof(usedBuf));
    formatBytes(info.freeBytes,  freeBuf,  sizeof(freeBuf));

    std::printf("  Mount:      %s\n", info.mountPoint);
    std::printf("  Filesystem: %s\n", info.filesystem);
    std::printf("  Total:      %s\n", totalBuf);
    std::printf("  Used:       %s\n", usedBuf);
    std::printf("  Free:       %s\n", freeBuf);
    std::printf("  Usage:      ");
    printBar(info.usedPercent);
    std::printf("\n");
}

// ── main ────────────────────────────────────────────────────────────

int main()
{
#if !defined(_WIN32)
    std::signal(SIGINT, [](int) { g_running.store(false); });
#endif

    DiskUsageClient client("disk-usage");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to disk-usage service\n");
        return 1;
    }
    std::printf("Connected to disk-usage service.\n\n");

    listVolumes(client);
    std::printf("\nCommands: list, check <mountpoint>, quit\n");

    char line[256];
    while (g_running.load())
    {
        std::printf("> ");
        std::fflush(stdout);

        if (!std::fgets(line, sizeof(line), stdin))
            break;

        size_t len = std::strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        if (std::strcmp(line, "quit") == 0 || std::strcmp(line, "q") == 0)
            break;
        else if (std::strcmp(line, "list") == 0)
            listVolumes(client);
        else if (std::strncmp(line, "check ", 6) == 0)
            checkMount(client, line + 6);
        else if (std::strlen(line) > 0)
            std::printf("Unknown command. Try: list, check <mountpoint>, quit\n");
    }

    client.disconnect();
    std::printf("Disconnected.\n");
    return 0;
}
