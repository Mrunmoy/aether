// OBD-II dashboard CLI — connects to the simulated ECU device.
//
// Run:   ./ecu_device   (in another terminal)
//        ./ecu_dashboard

#include "CanBusEcu.h"

#include <cstdio>
#include <cstring>
#include <string>

using namespace aether::ipc;

// ── Dashboard client with notification callbacks ───────────────────

class DashboardClient : public CanBusEcu
{
public:
    using CanBusEcu::CanBusEcu;

protected:
    void onCheckEngine(DtcEntry dtc) override
    {
        std::printf("\n\xe2\x9a\xa0 CHECK ENGINE: [%s] %s\n> ",
                    dtc.code, dtc.description);
        std::fflush(stdout);
    }

    void onDtcCleared(uint8_t clearedCount) override
    {
        std::printf("\n\xe2\x9c\x93 DTCs cleared: %u\n> ", clearedCount);
        std::fflush(stdout);
    }
};

// ── Gauge rendering ────────────────────────────────────────────────

static void printBar(float value, float maxVal, int width)
{
    int filled = 0;
    if (maxVal > 0.0f)
        filled = static_cast<int>((value / maxVal) * width + 0.5f);
    if (filled > width)
        filled = width;
    if (filled < 0)
        filled = 0;

    for (int i = 0; i < filled; ++i)
        std::printf("\xe2\x96\x88");   // █
    for (int i = filled; i < width; ++i)
        std::printf("\xe2\x96\x91");   // ░
}

static void displayDashboard(DashboardClient &client)
{
    struct PidDef
    {
        uint8_t pid;
        const char *label;
        float maxVal;
    };

    static const PidDef pids[] = {
        {0x0C, "RPM",      8000.0f},
        {0x0D, "Speed",     200.0f},
        {0x05, "Coolant",   120.0f},
        {0x11, "Throttle",  100.0f},
        {0x2F, "Fuel",      100.0f},
    };

    std::printf("\xe2\x95\x94\xe2\x95\x90\xe2\x95\x90"
                " Engine ECU Dashboard "
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x97\n");

    for (const auto &def : pids)
    {
        PidReading reading{};
        int rc = client.ReadPid(def.pid, &reading);

        std::printf("\xe2\x95\x91 %-9s ", def.label);
        if (rc == IPC_SUCCESS)
        {
            std::printf("%-6.0f%-5s ", reading.value, reading.unit);
            printBar(reading.value, def.maxVal, 12);
            std::printf("  (%.0f)", def.maxVal);
        }
        else
        {
            std::printf("  -- error --               ");
        }
        std::printf(" \xe2\x95\x91\n");
    }

    std::printf("\xe2\x95\x9a\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"
                "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x9d\n");

    // Show stored DTCs.
    uint8_t dtcCount = 0;
    if (client.GetDtcCount(&dtcCount) == IPC_SUCCESS && dtcCount > 0)
    {
        std::printf("DTCs: %u stored", dtcCount);
        for (uint8_t i = 0; i < dtcCount; ++i)
        {
            DtcEntry dtc{};
            if (client.GetDtc(i, &dtc) == IPC_SUCCESS)
                std::printf(" \xe2\x80\x94 [%s] %s", dtc.code, dtc.description);
        }
        std::printf("\n");
    }
    else
    {
        std::printf("DTCs: none stored\n");
    }
}

// ── Command handlers ───────────────────────────────────────────────

static void cmdReadPid(DashboardClient &client, const std::string &arg)
{
    unsigned long pidVal = 0;
    try
    {
        pidVal = std::stoul(arg, nullptr, 16);
    }
    catch (...)
    {
        std::printf("Invalid PID: %s\n", arg.c_str());
        return;
    }

    PidReading reading{};
    int rc = client.ReadPid(static_cast<uint8_t>(pidVal), &reading);
    if (rc == IPC_SUCCESS)
        std::printf("PID 0x%02X: %.2f %s\n", reading.pid, reading.value, reading.unit);
    else
        std::printf("PID 0x%02lX: error %d\n", pidVal, rc);
}

static void cmdDtc(DashboardClient &client)
{
    uint8_t count = 0;
    int rc = client.GetDtcCount(&count);
    if (rc != IPC_SUCCESS)
    {
        std::printf("Failed to read DTC count: %d\n", rc);
        return;
    }
    std::printf("Stored DTCs: %u\n", count);
    for (uint8_t i = 0; i < count; ++i)
    {
        DtcEntry dtc{};
        if (client.GetDtc(i, &dtc) == IPC_SUCCESS)
            std::printf("  [%s] severity=%u  %s\n", dtc.code, dtc.severity, dtc.description);
    }
}

static void cmdClear(DashboardClient &client)
{
    uint8_t cleared = 0;
    int rc = client.ClearDtc(&cleared);
    if (rc == IPC_SUCCESS)
        std::printf("Cleared %u DTC(s).\n", cleared);
    else
        std::printf("ClearDtc failed: %d\n", rc);
}

static void cmdVin(DashboardClient &client)
{
    VehicleInfo info{};
    int rc = client.GetVehicleInfo(&info);
    if (rc == IPC_SUCCESS)
        std::printf("VIN: %s  %u %s %s\n", info.vin, info.modelYear, info.make, info.model);
    else
        std::printf("GetVehicleInfo failed: %d\n", rc);
}

// ── main ───────────────────────────────────────────────────────────

int main()
{
    DashboardClient client("can-bus-ecu");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to ECU service\n");
        return 1;
    }
    std::printf("Connected to ECU.\n\n");

    displayDashboard(client);

    std::printf("\nCommands: read <pid_hex>, dtc, clear, vin, quit\n");

    char line[256];
    while (true)
    {
        std::printf("> ");
        std::fflush(stdout);
        if (!std::fgets(line, sizeof(line), stdin))
            break;

        // Trim trailing newline.
        size_t len = std::strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[--len] = '\0';

        std::string cmd(line);
        if (cmd == "quit" || cmd == "q")
            break;
        else if (cmd.rfind("read ", 0) == 0)
            cmdReadPid(client, cmd.substr(5));
        else if (cmd == "dtc")
            cmdDtc(client);
        else if (cmd == "clear")
            cmdClear(client);
        else if (cmd == "vin")
            cmdVin(client);
        else if (cmd == "dashboard" || cmd == "d")
            displayDashboard(client);
        else if (!cmd.empty())
            std::printf("Unknown command: %s\n", cmd.c_str());
    }

    client.disconnect();
    std::printf("Disconnected.\n");
    return 0;
}
