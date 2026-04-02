// ProcessMonitor client — CLI process viewer.

#include "ProcessMonitor.h"

#include <atomic>
#include <cinttypes>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <string>

using namespace aether::ipc;

static std::atomic<bool> g_running{true};

static void sigintHandler(int)
{
    g_running = false;
}

class ProcessViewer : public ProcessMonitor
{
public:
    using ProcessMonitor::ProcessMonitor;

protected:
    void onProcessStarted(ProcessInfo info) override
    {
        std::printf("\n[notify] Process started: PID %u  %s  (threads=%u, mem=%.1f MB)\n",
                    info.pid, info.name, info.threadCount,
                    static_cast<double>(info.memoryBytes) / (1024.0 * 1024.0));
        std::printf("> ");
        std::fflush(stdout);
    }

    void onProcessExited(uint32_t pid, uint32_t exitCode) override
    {
        std::printf("\n[notify] Process exited: PID %u  exitCode=%u\n", pid, exitCode);
        std::printf("> ");
        std::fflush(stdout);
    }
};

static void listProcesses(ProcessViewer &client)
{
    uint32_t count = 0;
    int rc = client.GetProcessCount(&count);
    if (rc != IPC_SUCCESS)
    {
        std::fprintf(stderr, "GetProcessCount failed (error %d)\n", rc);
        return;
    }

    std::printf("%-8s %-8s %-20s %-10s %s\n", "PID", "Parent", "Name", "Threads", "Memory");
    std::printf("%-8s %-8s %-20s %-10s %s\n", "---", "------", "----", "-------", "------");

    for (uint32_t i = 0; i < count; ++i)
    {
        ProcessInfo info{};
        rc = client.GetProcessAt(i, &info);
        if (rc != IPC_SUCCESS)
        {
            continue;
        }

        std::printf("%-8u %-8u %-20s %-10u %.1f MB\n",
                    info.pid, info.parentPid, info.name,
                    info.threadCount,
                    static_cast<double>(info.memoryBytes) / (1024.0 * 1024.0));
    }
}

static void showProcessInfo(ProcessViewer &client, uint32_t pid)
{
    ProcessInfo info{};
    int rc = client.GetProcessInfo(pid, &info);
    if (rc != IPC_SUCCESS)
    {
        std::fprintf(stderr, "Process %u not found (error %d)\n", pid, rc);
        return;
    }

    std::printf("  PID:      %u\n", info.pid);
    std::printf("  Parent:   %u\n", info.parentPid);
    std::printf("  Name:     %s\n", info.name);
    std::printf("  Threads:  %u\n", info.threadCount);
    std::printf("  Memory:   %" PRIu64 " bytes (%.1f MB)\n",
                info.memoryBytes,
                static_cast<double>(info.memoryBytes) / (1024.0 * 1024.0));
}

int main()
{
    std::signal(SIGINT, sigintHandler);

    ProcessViewer client("ProcessMonitor");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to ProcessMonitor service\n");
        return 1;
    }

    std::printf("Connected to ProcessMonitor service.\n");
    std::printf("Commands: list, info <pid>, quit\n\n");

    listProcesses(client);
    std::printf("\n");

    char line[256];
    while (g_running)
    {
        std::printf("> ");
        std::fflush(stdout);

        if (!std::fgets(line, sizeof(line), stdin))
        {
            break;
        }

        // Strip trailing newline.
        size_t len = std::strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        if (std::strcmp(line, "quit") == 0 || std::strcmp(line, "q") == 0)
        {
            break;
        }
        else if (std::strcmp(line, "list") == 0)
        {
            listProcesses(client);
        }
        else if (std::strncmp(line, "info ", 5) == 0)
        {
            uint32_t pid = 0;
            if (std::sscanf(line + 5, "%u", &pid) == 1)
            {
                showProcessInfo(client, pid);
            }
            else
            {
                std::printf("Usage: info <pid>\n");
            }
        }
        else if (std::strlen(line) > 0)
        {
            std::printf("Unknown command: %s\n", line);
        }
    }

    std::printf("Disconnecting...\n");
    client.disconnect();
    return 0;
}
