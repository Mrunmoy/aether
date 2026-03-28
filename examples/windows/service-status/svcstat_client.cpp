// ServiceStatus client — CLI viewer for simulated Windows services.

#include "ServiceStatus.h"

#include <atomic>
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

static const char *stateName(ServiceState state)
{
    switch (state)
    {
    case Stopped:
        return "STOPPED";
    case Starting:
        return "STARTING";
    case Stopping:
        return "STOPPING";
    case Running:
        return "RUNNING";
    case Paused:
        return "PAUSED";
    default:
        return "UNKNOWN";
    }
}

class ServiceStatusClient : public ServiceStatus
{
public:
    using ServiceStatus::ServiceStatus;

protected:
    void onServiceStateChanged(ServiceInfo info, ServiceState oldState) override
    {
        std::printf("\n[notify] %s: %s → %s (PID %s)\n",
                    info.serviceName,
                    stateName(oldState),
                    stateName(info.state),
                    (info.pid > 0) ? std::to_string(info.pid).c_str() : "-");
        std::printf("> ");
        std::fflush(stdout);
    }
};

static void listServices(ServiceStatusClient &client)
{
    uint32_t count = 0;
    int rc = client.GetServiceCount(&count);
    if (rc != IPC_SUCCESS)
    {
        std::fprintf(stderr, "GetServiceCount failed (error %d)\n", rc);
        return;
    }

    std::printf("%-18s %-33s %-10s %s\n", "Service", "Display Name", "State", "PID");
    std::printf("%-18s %-33s %-10s %s\n", "-------", "------------", "-----", "---");

    for (uint32_t i = 0; i < count; ++i)
    {
        ServiceInfo info{};
        rc = client.GetServiceAt(i, &info);
        if (rc != IPC_SUCCESS)
        {
            continue;
        }

        char pidStr[16];
        if (info.pid > 0)
        {
            std::snprintf(pidStr, sizeof(pidStr), "%u", info.pid);
        }
        else
        {
            std::snprintf(pidStr, sizeof(pidStr), "-");
        }

        std::printf("%-18s %-33s %-10s %s\n",
                    info.serviceName, info.displayName,
                    stateName(info.state), pidStr);
    }
}

static void queryService(ServiceStatusClient &client, const char *name)
{
    ServiceInfo info{};
    int rc = client.GetServiceStatus(name, &info);
    if (rc != IPC_SUCCESS)
    {
        std::fprintf(stderr, "Service \"%s\" not found (error %d)\n", name, rc);
        return;
    }

    std::printf("  Service:      %s\n", info.serviceName);
    std::printf("  Display Name: %s\n", info.displayName);
    std::printf("  State:        %s\n", stateName(info.state));
    if (info.pid > 0)
    {
        std::printf("  PID:          %u\n", info.pid);
    }
    else
    {
        std::printf("  PID:          -\n");
    }
}

int main()
{
    std::signal(SIGINT, sigintHandler);

    ServiceStatusClient client("ServiceStatus");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to ServiceStatus service\n");
        return 1;
    }

    std::printf("Connected to ServiceStatus service.\n");
    std::printf("Commands: list, status <name>, quit\n\n");

    listServices(client);
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
            listServices(client);
        }
        else if (std::strncmp(line, "status ", 7) == 0)
        {
            queryService(client, line + 7);
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
