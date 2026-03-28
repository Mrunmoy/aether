// SensorHub dashboard (client) — CLI interface for querying sensors
// and receiving live data notifications.

#include "SensorHub.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>

using namespace aether::ipc;

static std::atomic<bool> g_running{true};

static void sigintHandler(int)
{
    g_running = false;
}

class SensorDashboard : public SensorHub
{
public:
    using SensorHub::SensorHub;

protected:
    void onDataReady(AccelData accel, GyroData gyro) override
    {
        std::lock_guard<std::mutex> lock(m_printMutex);
        std::printf("  Accel: X=%-7.2f Y=%-7.2f Z=%-7.2f (m/s²)\n",
                    accel.x, accel.y, accel.z);
        std::printf("  Gyro:  X=%-7.2f Y=%-7.2f Z=%-7.2f (deg/s)\n",
                    gyro.x, gyro.y, gyro.z);
    }

    void onMotionDetected(float magnitude) override
    {
        std::lock_guard<std::mutex> lock(m_printMutex);
        std::printf("  *** MOTION DETECTED *** magnitude=%.2f m/s²\n", magnitude);
    }

public:
    std::mutex m_printMutex;
};

static void printUsage()
{
    std::printf("\nCommands: accel | gyro | env | rate <1|10|50|100> | calibrate | quit\n> ");
    std::fflush(stdout);
}

int main()
{
    std::signal(SIGINT, sigintHandler);

    SensorDashboard client("SensorHub");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to SensorHub service\n");
        return 1;
    }
    std::printf("Connected to SensorHub.\n");

    // Print initial readings.
    AccelData accel{};
    if (client.GetAccelerometer(&accel) == IPC_SUCCESS)
    {
        std::printf("Accel: X=%.2f  Y=%.2f  Z=%.2f  (m/s²)\n",
                    accel.x, accel.y, accel.z);
    }
    GyroData gyro{};
    if (client.GetGyroscope(&gyro) == IPC_SUCCESS)
    {
        std::printf("Gyro:  X=%.2f  Y=%.2f  Z=%.2f  (deg/s)\n",
                    gyro.x, gyro.y, gyro.z);
    }
    EnvironmentData env{};
    if (client.GetEnvironment(&env) == IPC_SUCCESS)
    {
        std::printf("Env:   T=%.1f°C  P=%.1fhPa  H=%.1f%%\n",
                    env.temperatureC, env.pressureHpa, env.humidityPercent);
    }

    printUsage();

    char line[256];
    while (g_running && std::fgets(line, sizeof(line), stdin))
    {
        // Strip trailing newline.
        size_t len = std::strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        if (std::strcmp(line, "quit") == 0)
        {
            break;
        }
        else if (std::strcmp(line, "accel") == 0)
        {
            AccelData a{};
            int rc = client.GetAccelerometer(&a);
            if (rc == IPC_SUCCESS)
            {
                std::printf("Accel: X=%.2f  Y=%.2f  Z=%.2f  (m/s²)\n",
                            a.x, a.y, a.z);
            }
            else
            {
                std::printf("Error: %d\n", rc);
            }
        }
        else if (std::strcmp(line, "gyro") == 0)
        {
            GyroData g{};
            int rc = client.GetGyroscope(&g);
            if (rc == IPC_SUCCESS)
            {
                std::printf("Gyro:  X=%.2f  Y=%.2f  Z=%.2f  (deg/s)\n",
                            g.x, g.y, g.z);
            }
            else
            {
                std::printf("Error: %d\n", rc);
            }
        }
        else if (std::strcmp(line, "env") == 0)
        {
            EnvironmentData e{};
            int rc = client.GetEnvironment(&e);
            if (rc == IPC_SUCCESS)
            {
                std::printf("Env:   T=%.1f°C  P=%.1fhPa  H=%.1f%%\n",
                            e.temperatureC, e.pressureHpa, e.humidityPercent);
            }
            else
            {
                std::printf("Error: %d\n", rc);
            }
        }
        else if (std::strncmp(line, "rate ", 5) == 0)
        {
            int hz = std::atoi(line + 5);
            SampleRate rate;
            switch (hz)
            {
            case 1:
                rate = Rate_1Hz;
                break;
            case 10:
                rate = Rate_10Hz;
                break;
            case 50:
                rate = Rate_50Hz;
                break;
            case 100:
                rate = Rate_100Hz;
                break;
            default:
                std::printf("Invalid rate. Use 1, 10, 50, or 100.\n");
                printUsage();
                continue;
            }
            int rc = client.SetSampleRate(rate);
            if (rc == IPC_SUCCESS)
            {
                std::printf("Sample rate set to %d Hz\n", hz);
            }
            else
            {
                std::printf("Error: %d\n", rc);
            }
        }
        else if (std::strcmp(line, "calibrate") == 0)
        {
            uint8_t success = 0;
            int rc = client.Calibrate(&success);
            if (rc == IPC_SUCCESS && success)
            {
                std::printf("Calibration successful\n");
            }
            else
            {
                std::printf("Calibration failed (rc=%d, success=%u)\n", rc, success);
            }
        }
        else if (line[0] != '\0')
        {
            std::printf("Unknown command: %s\n", line);
        }

        printUsage();
    }

    std::printf("\nDisconnecting...\n");
    client.disconnect();
    return 0;
}
