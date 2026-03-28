// CLI dashboard for the simulated GPIO controller.
//
// Run:   ./gpio_device     (in another terminal)
//        ./gpio_dashboard

#include "GpioController.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace aether::ipc;

static const char *directionStr(PinDirection d)
{
    return (d == Output) ? "OUTPUT" : "INPUT";
}

static const char *levelStr(PinLevel l)
{
    return (l == High) ? "HIGH" : "LOW";
}

static const char *edgeStr(EdgeTrigger e)
{
    switch (e)
    {
    case Rising:
        return "Rising";
    case Falling:
        return "Falling";
    case Both:
        return "Both";
    default:
        return "None";
    }
}

class GpioDashboard : public GpioController
{
public:
    using GpioController::GpioController;

protected:
    void onPinInterrupt(uint8_t pin, EdgeTrigger edge) override
    {
        std::printf("\n[interrupt] Pin %d: %s\n> ", pin, edgeStr(edge));
        std::fflush(stdout);
    }
};

static void printTable(const PinState *pins)
{
    std::printf(" Pin | Dir    | Level | Edge\n");
    std::printf("-----+--------+-------+---------\n");
    for (int i = 0; i < 16; i++)
    {
        std::printf(" %2d  | %-6s | %-5s | %s\n",
                    i, directionStr(pins[i].direction),
                    levelStr(pins[i].level), edgeStr(pins[i].edge));
    }
}

static EdgeTrigger parseEdge(const char *s)
{
    if (std::strcmp(s, "rising") == 0)
        return Rising;
    if (std::strcmp(s, "falling") == 0)
        return Falling;
    if (std::strcmp(s, "both") == 0)
        return Both;
    return None;
}

int main()
{
    GpioDashboard client("gpio");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to GPIO service\n");
        return 1;
    }
    std::printf("Connected to GPIO controller.\n\n");

    // Show initial state.
    PinState pins[16]{};
    int rc = client.ReadAllPins(pins);
    if (rc == IPC_SUCCESS)
        printTable(pins);
    else
        std::fprintf(stderr, "ReadAllPins failed: %d\n", rc);

    std::printf("\nCommands: config <pin> <in|out> [edge], read <pin>, "
                "write <pin> <0|1>, status, quit\n");

    std::string line;
    while (true)
    {
        std::printf("> ");
        std::fflush(stdout);
        if (!std::getline(std::cin, line))
            break;

        char cmd[32] = {};
        char arg1[32] = {};
        char arg2[32] = {};
        char arg3[32] = {};
        int n = std::sscanf(line.c_str(), "%31s %31s %31s %31s", cmd, arg1, arg2, arg3);
        if (n < 1)
            continue;

        if (std::strcmp(cmd, "quit") == 0 || std::strcmp(cmd, "q") == 0)
        {
            break;
        }
        else if (std::strcmp(cmd, "status") == 0)
        {
            PinState all[16]{};
            rc = client.ReadAllPins(all);
            if (rc == IPC_SUCCESS)
                printTable(all);
            else
                std::fprintf(stderr, "ReadAllPins failed: %d\n", rc);
        }
        else if (std::strcmp(cmd, "read") == 0 && n >= 2)
        {
            int pin = std::atoi(arg1);
            PinLevel level{};
            rc = client.ReadPin(static_cast<uint8_t>(pin), &level);
            if (rc == IPC_SUCCESS)
                std::printf("Pin %d: %s\n", pin, levelStr(level));
            else
                std::fprintf(stderr, "ReadPin failed: %d\n", rc);
        }
        else if (std::strcmp(cmd, "write") == 0 && n >= 3)
        {
            int pin = std::atoi(arg1);
            PinLevel level = (std::atoi(arg2) != 0) ? High : Low;
            rc = client.WritePin(static_cast<uint8_t>(pin), level);
            if (rc == IPC_SUCCESS)
                std::printf("Pin %d written: %s\n", pin, levelStr(level));
            else
                std::fprintf(stderr, "WritePin failed: %d\n", rc);
        }
        else if (std::strcmp(cmd, "config") == 0 && n >= 3)
        {
            int pin = std::atoi(arg1);
            PinDirection dir = (std::strcmp(arg2, "out") == 0) ? Output : Input;
            EdgeTrigger edge = (n >= 4) ? parseEdge(arg3) : None;
            rc = client.ConfigurePin(static_cast<uint8_t>(pin), dir, edge);
            if (rc == IPC_SUCCESS)
                std::printf("Pin %d configured: %s, edge=%s\n",
                            pin, directionStr(dir), edgeStr(edge));
            else
                std::fprintf(stderr, "ConfigurePin failed: %d\n", rc);
        }
        else
        {
            std::printf("Unknown command. Try: config, read, write, status, quit\n");
        }
    }

    client.disconnect();
    std::printf("Disconnected.\n");
    return 0;
}
