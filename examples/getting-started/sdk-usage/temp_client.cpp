/*
 * temp_client.cpp — Temperature sensor client using ipcgen-generated code.
 *
 * The generated class uses aether_ipc.h internally, so this project
 * only needs the SDK release artifact to build — no source checkout required.
 *
 * Steps:
 *   1. Write TemperatureSensor.idl
 *   2. python3 ipcgen TemperatureSensor.idl --outdir gen --backend c_api
 *   3. Subclass the generated client, override notification callbacks (this file)
 *   4. cmake -B build -DAETHER_SDK=/path/to/sdk && cmake --build build
 */

#include "client/TemperatureSensor.h"

#include <cstdio>
#include <thread>
#include <chrono>

// Subclass the generated client — override notification callbacks.
class TempClient : public aether::ipc::TemperatureSensor
{
public:
    using TemperatureSensor::TemperatureSensor;

protected:
    void onOverTemperature(float celsius) override
    {
        std::printf("[client] ⚠ Over-temperature: %.1f°C!\n", celsius);
    }
};

int main()
{
    TempClient client("temp_sensor");
    if (!client.connect()) {
        std::fprintf(stderr, "[client] Failed to connect\n");
        return 1;
    }
    std::printf("[client] Connected to temp_sensor\n");

    // Read current temperature
    float temp = 0;
    if (client.GetTemperature(&temp) == AETHER_SUCCESS)
        std::printf("[client] Current temperature: %.1f°C\n", temp);

    // Set threshold below current temp → triggers OverTemperature notification
    if (client.SetThreshold(50.0f, -10.0f) == AETHER_SUCCESS)
        std::printf("[client] Threshold set to [50.0, -10.0]°C\n");

    // Wait briefly for the notification to arrive
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::printf("[client] Done.\n");
    return 0;
}
