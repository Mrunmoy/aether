// ProcessMonitor device (server) — simulates a process list.

#include "ProcessMonitor.h"

#include <algorithm>
#include <atomic>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

using namespace aether::ipc;

static std::atomic<bool> g_running{true};

static void sigintHandler(int)
{
    g_running = false;
}

class ProcessMonitorDevice : public ProcessMonitor
{
public:
    using ProcessMonitor::ProcessMonitor;

    void initProcessList()
    {
        auto add = [this](uint32_t pid, uint32_t parent, const char *name,
                          uint32_t threads, uint64_t mem)
        {
            ProcessInfo p{};
            p.pid = pid;
            p.parentPid = parent;
            std::strncpy(p.name, name, sizeof(p.name) - 1);
            p.threadCount = threads;
            p.memoryBytes = mem;
            m_processes.push_back(p);
        };

        add(1, 0, "System", 4, 1ULL * 1024 * 1024);
        add(100, 1, "explorer.exe", 12, 80ULL * 1024 * 1024);
        add(200, 1, "svchost.exe", 8, 40ULL * 1024 * 1024);
        add(300, 100, "chrome.exe", 25, 350ULL * 1024 * 1024);
        add(400, 100, "notepad.exe", 3, 5ULL * 1024 * 1024);
    }

    void startSimulation()
    {
        m_simThread = std::thread([this] { simulationLoop(); });
    }

    void stopSimulation()
    {
        m_simRunning = false;
        if (m_simThread.joinable())
        {
            m_simThread.join();
        }
    }

protected:
    int handleGetProcessCount(uint32_t *count) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *count = static_cast<uint32_t>(m_processes.size());
        return IPC_SUCCESS;
    }

    int handleGetProcessInfo(uint32_t pid, ProcessInfo *info) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto &p : m_processes)
        {
            if (p.pid == pid)
            {
                *info = p;
                return IPC_SUCCESS;
            }
        }
        return IPC_ERR_INVALID_ARGUMENT;
    }

    int handleGetProcessAt(uint32_t index, ProcessInfo *info) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (index >= m_processes.size())
        {
            return IPC_ERR_INVALID_ARGUMENT;
        }
        *info = m_processes[index];
        return IPC_SUCCESS;
    }

private:
    void simulationLoop()
    {
        std::mt19937 rng(42);
        uint32_t nextPid = 1000;

        while (m_simRunning)
        {
            // Sleep in small increments so we can exit promptly.
            for (int i = 0; i < 30 && m_simRunning; ++i)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (!m_simRunning)
            {
                break;
            }

            std::uniform_int_distribution<int> coinFlip(0, 1);
            bool doSpawn = coinFlip(rng);

            ProcessInfo spawnedInfo{};
            bool didSpawn = false;
            uint32_t exitedPid = 0;
            bool didExit = false;

            {
                std::lock_guard<std::mutex> lock(m_mutex);

                // Always spawn if the list is very small.
                if (doSpawn || m_processes.size() <= 2)
                {
                    ProcessInfo p{};
                    p.pid = nextPid++;
                    p.parentPid = 1;
                    std::snprintf(p.name, sizeof(p.name), "app_%04u.exe", p.pid);
                    std::uniform_int_distribution<uint32_t> threadDist(1, 16);
                    std::uniform_int_distribution<uint64_t> memDist(1ULL * 1024 * 1024,
                                                                    200ULL * 1024 * 1024);
                    p.threadCount = threadDist(rng);
                    p.memoryBytes = memDist(rng);

                    m_processes.push_back(p);
                    spawnedInfo = p;
                    didSpawn = true;
                    std::printf("[sim] Spawned PID %u: %s\n", p.pid, p.name);
                }
                else
                {
                    // Kill a random non-system process (PID > 1).
                    std::vector<size_t> killable;
                    for (size_t i = 0; i < m_processes.size(); ++i)
                    {
                        if (m_processes[i].pid > 1)
                        {
                            killable.push_back(i);
                        }
                    }
                    if (!killable.empty())
                    {
                        std::uniform_int_distribution<size_t> pick(0, killable.size() - 1);
                        size_t idx = killable[pick(rng)];
                        exitedPid = m_processes[idx].pid;
                        std::printf("[sim] Killed PID %u: %s\n", exitedPid, m_processes[idx].name);
                        m_processes.erase(m_processes.begin() + static_cast<ptrdiff_t>(idx));
                        didExit = true;
                    }
                }
            }

            if (didSpawn)
                notifyProcessStarted(spawnedInfo);
            if (didExit)
                notifyProcessExited(exitedPid, 0);
        }
    }

    std::mutex m_mutex;
    std::vector<ProcessInfo> m_processes;
    std::atomic<bool> m_simRunning{true};
    std::thread m_simThread;
};

int main()
{
    std::signal(SIGINT, sigintHandler);

    ProcessMonitorDevice svc("ProcessMonitor");
    svc.initProcessList();

    if (!svc.start())
    {
        std::fprintf(stderr, "Failed to start ProcessMonitor service\n");
        return 1;
    }

    svc.startSimulation();
    std::printf("ProcessMonitor device running. Press Ctrl-C to stop.\n");

    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::printf("\nShutting down...\n");
    svc.stopSimulation();
    svc.stop();
    return 0;
}
