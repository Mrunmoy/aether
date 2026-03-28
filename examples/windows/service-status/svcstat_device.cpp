// ServiceStatus device (server) — simulates a Windows service list.

#include "ServiceStatus.h"

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

struct SimulatedService
{
    char serviceName[65];
    char displayName[129];
    ServiceState state;
    uint32_t pid;
};

class ServiceStatusDevice : public ServiceStatus
{
public:
    using ServiceStatus::ServiceStatus;
    using ServiceStatus::notifyServiceStateChanged;

    void initServices()
    {
        auto add = [this](const char *name, const char *display,
                          ServiceState state, uint32_t pid)
        {
            SimulatedService s{};
            std::strncpy(s.serviceName, name, 64);
            std::strncpy(s.displayName, display, 128);
            s.state = state;
            s.pid = pid;
            m_services.push_back(s);
        };

        add("Spooler", "Print Spooler", Running, 1200);
        add("BITS", "Background Intelligent Transfer", Running, 800);
        add("wuauserv", "Windows Update", Stopped, 0);
        add("WinDefend", "Windows Defender", Running, 2100);
        add("Dhcp", "DHCP Client", Running, 900);
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
    int handleGetServiceStatus(const char *serviceName, ServiceInfo *info) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        for (const auto &s : m_services)
        {
            if (std::strcmp(s.serviceName, serviceName) == 0)
            {
                std::memcpy(info->serviceName, s.serviceName, sizeof(info->serviceName));
                std::memcpy(info->displayName, s.displayName, sizeof(info->displayName));
                info->state = s.state;
                info->pid = s.pid;
                return IPC_SUCCESS;
            }
        }
        return IPC_ERR_INVALID_ARGUMENT;
    }

    int handleGetServiceCount(uint32_t *count) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *count = static_cast<uint32_t>(m_services.size());
        return IPC_SUCCESS;
    }

    int handleGetServiceAt(uint32_t index, ServiceInfo *info) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (index >= m_services.size())
        {
            return IPC_ERR_INVALID_ARGUMENT;
        }

        const auto &s = m_services[index];
        std::memcpy(info->serviceName, s.serviceName, sizeof(info->serviceName));
        std::memcpy(info->displayName, s.displayName, sizeof(info->displayName));
        info->state = s.state;
        info->pid = s.pid;
        return IPC_SUCCESS;
    }

private:
    void simulationLoop()
    {
        std::mt19937 rng(std::random_device{}());

        while (m_simRunning)
        {
            for (int i = 0; i < 40 && m_simRunning; ++i)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (!m_simRunning)
            {
                break;
            }

            std::lock_guard<std::mutex> lock(m_mutex);
            std::uniform_int_distribution<size_t> dist(0, m_services.size() - 1);
            size_t idx = dist(rng);
            auto &s = m_services[idx];

            ServiceState oldState = s.state;
            if (s.state == Running)
            {
                s.state = Stopped;
                s.pid = 0;
            }
            else
            {
                s.state = Running;
                std::uniform_int_distribution<uint32_t> pidDist(500, 9999);
                s.pid = pidDist(rng);
            }

            ServiceInfo info{};
            std::memcpy(info.serviceName, s.serviceName, sizeof(info.serviceName));
            std::memcpy(info.displayName, s.displayName, sizeof(info.displayName));
            info.state = s.state;
            info.pid = s.pid;

            std::printf("[sim] %s: %s → %s\n", s.serviceName,
                        (oldState == Running) ? "Running" : "Stopped",
                        (s.state == Running) ? "Running" : "Stopped");

            notifyServiceStateChanged(info, oldState);
        }
    }

    std::mutex m_mutex;
    std::vector<SimulatedService> m_services;
    std::atomic<bool> m_simRunning{true};
    std::thread m_simThread;
};

int main()
{
    std::signal(SIGINT, sigintHandler);

    ServiceStatusDevice svc("ServiceStatus");
    svc.initServices();

    if (!svc.start())
    {
        std::fprintf(stderr, "Failed to start ServiceStatus service\n");
        return 1;
    }

    svc.startSimulation();
    std::printf("ServiceStatus device running. Press Ctrl-C to stop.\n");

    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::printf("\nShutting down...\n");
    svc.stopSimulation();
    svc.stop();
    return 0;
}
