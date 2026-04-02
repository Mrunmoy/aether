/*
 * temp_server.cpp — Temperature sensor service using ipcgen-generated code.
 *
 * The generated class uses aether_ipc.h internally, so this project
 * only needs the SDK release artifact to build — no source checkout required.
 *
 * Steps:
 *   1. Write TemperatureSensor.idl
 *   2. python3 ipcgen TemperatureSensor.idl --outdir gen --backend c_api
 *   3. Subclass the generated server, implement the handlers (this file)
 *   4. cmake -B build -DAETHER_SDK=/path/to/sdk && cmake --build build
 */

#include "server/TemperatureSensor.h"

#include <atomic>
#include <cstdio>
#include <csignal>
#include <unistd.h>

// Subclass the generated server — implement the pure virtual handlers.
class TempSensorImpl : public aether::ipc::TemperatureSensor
{
public:
    using TemperatureSensor::TemperatureSensor;

protected:
    int handleGetTemperature(float *celsius) override
    {
        *celsius = m_currentTemp;
        std::printf("[server] GetTemperature → %.1f°C\n", m_currentTemp);
        return AETHER_SUCCESS;
    }

    int handleSetThreshold(float high, float low) override
    {
        m_thresholdHigh = high;
        m_thresholdLow = low;
        std::printf("[server] SetThreshold(high=%.1f, low=%.1f)\n", high, low);

        if (m_currentTemp > m_thresholdHigh) {
            std::printf("[server] %.1f°C exceeds threshold — broadcasting\n",
                        m_currentTemp);
            sendOverTemperature(m_currentTemp);
        }
        return AETHER_SUCCESS;
    }

private:
    float m_currentTemp = 72.5f;
    float m_thresholdHigh = 100.0f;
    float m_thresholdLow = -40.0f;
};

static std::atomic<bool> g_running{true};
static void onSignal(int) { g_running = false; }

int main()
{
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    TempSensorImpl service("temp_sensor");
    if (!service.start()) {
        std::fprintf(stderr, "Failed to start service\n");
        return 1;
    }

    std::printf("[server] Temperature sensor running. Ctrl+C to stop.\n");

    while (g_running && service.isRunning())
        pause();

    service.stop();
    std::printf("[server] Stopped.\n");
    return 0;
}
