// Motor control CLI dashboard.
//
// Connects to the motor_device server and provides interactive control.
//
// Run:   ./motor_device     (in another terminal)
//        ./motor_dashboard

#include "MotorController.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace aether::ipc;

// ── Notification-printing client ────────────────────────────────────

class DashboardClient : public MotorController
{
public:
    using MotorController::MotorController;

protected:
    void onMotionComplete(int32_t position) override
    {
        std::printf("\n  >> Motion complete at position %d\n> ", position);
        std::fflush(stdout);
    }

    void onLimitHit(LimitSwitch which, int32_t position) override
    {
        const char *name = (which == Lower) ? "LOWER" : (which == Upper) ? "UPPER" : "NONE";
        std::printf("\n  >> Limit hit: %s at position %d\n> ", name, position);
        std::fflush(stdout);
    }

    void onStallDetected(int32_t position, float velocity) override
    {
        std::printf("\n  >> STALL DETECTED at position %d, velocity=%.1f\n> ", position, velocity);
        std::fflush(stdout);
    }
};

// ── Helpers ─────────────────────────────────────────────────────────

static const char *stateName(MotorState s)
{
    switch (s)
    {
    case Idle:    return "IDLE";
    case Homing:  return "HOMING";
    case Moving:  return "MOVING";
    case Stopped: return "STOPPED";
    case Fault:   return "FAULT";
    default:      return "UNKNOWN";
    }
}

static void printStatus(DashboardClient &client)
{
    MotorStatus status{};
    int rc = client.GetStatus(&status);
    if (rc != IPC_SUCCESS)
    {
        std::printf("  GetStatus failed: %d\n", rc);
        return;
    }

    std::printf("\n  Motor Controller\n");
    std::printf("  State:    %s\n", stateName(status.state));
    std::printf("  Position: %d steps\n", status.position);
    std::printf("  Velocity: %.1f steps/s\n", static_cast<double>(status.velocity));
    std::printf("  Homed:    %s\n", status.homed ? "YES" : "NO");
    std::printf("  Limits:   [%c] Lower  [%c] Upper\n",
                status.activeLimits == Lower ? 'X' : '_',
                status.activeLimits == Upper ? 'X' : '_');
    std::printf("\n");
}

static void printHelp()
{
    std::printf("\n  Commands:\n");
    std::printf("    home            Start homing sequence\n");
    std::printf("    move <pos> [v]  Move to position (optional velocity, default 500)\n");
    std::printf("    jog <vel>       Jog at velocity (steps/s)\n");
    std::printf("    stop            Stop motor\n");
    std::printf("    status          Show motor status\n");
    std::printf("    pos             Show current position\n");
    std::printf("    help            Show this help\n");
    std::printf("    quit            Exit\n\n");
}

// ── main ────────────────────────────────────────────────────────────

int main()
{
    DashboardClient client("motor");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to motor controller\n");
        return 1;
    }
    std::printf("Connected to motor controller.\n");
    printHelp();

    std::string line;
    std::printf("> ");
    std::fflush(stdout);

    while (std::getline(std::cin, line))
    {
        // Trim leading spaces
        size_t start = line.find_first_not_of(' ');
        if (start == std::string::npos)
        {
            std::printf("> ");
            std::fflush(stdout);
            continue;
        }
        line = line.substr(start);

        char cmd[32] = {};
        std::sscanf(line.c_str(), "%31s", cmd);

        if (std::strcmp(cmd, "quit") == 0 || std::strcmp(cmd, "exit") == 0)
        {
            break;
        }
        else if (std::strcmp(cmd, "home") == 0)
        {
            int rc = client.Home();
            if (rc != IPC_SUCCESS)
                std::printf("  Home failed: %d\n", rc);
            else
                std::printf("  Homing started...\n");
        }
        else if (std::strcmp(cmd, "move") == 0)
        {
            int32_t pos = 0;
            float vel = 500.0f;
            int n = std::sscanf(line.c_str(), "%*s %d %f", &pos, &vel);
            if (n < 1)
            {
                std::printf("  Usage: move <position> [velocity]\n");
            }
            else
            {
                int rc = client.MoveTo(pos, vel);
                if (rc != IPC_SUCCESS)
                    std::printf("  MoveTo failed: %d\n", rc);
                else
                    std::printf("  Moving to %d at %.1f steps/s\n", pos, static_cast<double>(vel));
            }
        }
        else if (std::strcmp(cmd, "jog") == 0)
        {
            float vel = 0.0f;
            if (std::sscanf(line.c_str(), "%*s %f", &vel) != 1)
            {
                std::printf("  Usage: jog <velocity>\n");
            }
            else
            {
                int rc = client.Jog(vel);
                if (rc != IPC_SUCCESS)
                    std::printf("  Jog failed: %d\n", rc);
                else
                    std::printf("  Jogging at %.1f steps/s\n", static_cast<double>(vel));
            }
        }
        else if (std::strcmp(cmd, "stop") == 0)
        {
            int rc = client.Stop();
            if (rc != IPC_SUCCESS)
                std::printf("  Stop failed: %d\n", rc);
            else
                std::printf("  Motor stopped.\n");
        }
        else if (std::strcmp(cmd, "status") == 0)
        {
            printStatus(client);
        }
        else if (std::strcmp(cmd, "pos") == 0)
        {
            int32_t pos = 0;
            int rc = client.GetPosition(&pos);
            if (rc != IPC_SUCCESS)
                std::printf("  GetPosition failed: %d\n", rc);
            else
                std::printf("  Position: %d steps\n", pos);
        }
        else if (std::strcmp(cmd, "help") == 0)
        {
            printHelp();
        }
        else
        {
            std::printf("  Unknown command: %s (type 'help')\n", cmd);
        }

        std::printf("> ");
        std::fflush(stdout);
    }

    client.disconnect();
    std::printf("Disconnected.\n");
    return 0;
}
