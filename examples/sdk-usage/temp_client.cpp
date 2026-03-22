/*
 * temp_client.cpp — Temperature sensor client using the Aether C API from C++.
 *
 * Shows how to wrap the C API in an RAII class with typed RPC methods
 * and notification callbacks.
 *
 * Build with SDK:
 *   g++ -std=c++17 -o temp_client temp_client.cpp \
 *       -I<sdk>/include <sdk>/lib/libaether.a -lpthread
 */

#include "aether_ipc.h"
#include "protocol.h"

#include <cstdio>
#include <cstring>
#include <functional>
#include <thread>
#include <chrono>

// ── Thin RAII wrapper around aether_client_t ─────────────────────────

class TempSensorClient
{
public:
    using OverTempCallback = std::function<void(float celsius)>;

    TempSensorClient(const char *name, OverTempCallback onOverTemp = nullptr)
        : m_onOverTemp(std::move(onOverTemp))
    {
        int rc = aether_client_create(name, onNotifyThunk, this, &m_cli);
        if (rc != AETHER_SUCCESS) {
            std::fprintf(stderr, "client_create failed: %d\n", rc);
            std::exit(1);
        }
    }

    ~TempSensorClient()
    {
        if (m_cli)
            aether_client_destroy(m_cli);
    }

    // Not copyable
    TempSensorClient(const TempSensorClient &) = delete;
    TempSensorClient &operator=(const TempSensorClient &) = delete;

    bool connect()
    {
        return aether_client_connect(m_cli) == AETHER_SUCCESS;
    }

    // ── Typed RPC methods ────────────────────────────────────────────

    int getTemperature(float *celsius)
    {
        uint8_t resp[64]{};
        uint32_t resp_len = 0;

        int rc = aether_client_call(m_cli, temp::kServiceId, temp::kMethodGetTemp,
                                    nullptr, 0,
                                    resp, sizeof(resp), &resp_len, 2000);
        if (rc == AETHER_SUCCESS && resp_len >= sizeof(float))
            std::memcpy(celsius, resp, sizeof(float));
        return rc;
    }

    int setThreshold(float high, float low)
    {
        temp::Thresholds t{high, low};
        uint8_t resp[64]{};
        uint32_t resp_len = 0;

        return aether_client_call(m_cli, temp::kServiceId, temp::kMethodSetThreshold,
                                  reinterpret_cast<const uint8_t *>(&t), sizeof(t),
                                  resp, sizeof(resp), &resp_len, 2000);
    }

private:
    aether_client_t m_cli = nullptr;
    OverTempCallback m_onOverTemp;

    static void onNotifyThunk(uint32_t service_id, uint32_t message_id,
                              const uint8_t *payload, uint32_t len,
                              void *user_data)
    {
        auto *self = static_cast<TempSensorClient *>(user_data);
        self->onNotification(service_id, message_id, payload, len);
    }

    void onNotification(uint32_t /*service_id*/, uint32_t message_id,
                        const uint8_t *payload, uint32_t len)
    {
        if (message_id == temp::kNotifyOverTemp && len >= sizeof(float) && m_onOverTemp) {
            float celsius;
            std::memcpy(&celsius, payload, sizeof(float));
            m_onOverTemp(celsius);
        }
    }
};

// ── Main ─────────────────────────────────────────────────────────────

int main()
{
    TempSensorClient client("temp_sensor", [](float celsius) {
        std::printf("[client] ⚠ Over-temperature: %.1f°C!\n", celsius);
    });

    if (!client.connect()) {
        std::fprintf(stderr, "[client] Failed to connect\n");
        return 1;
    }
    std::printf("[client] Connected to temp_sensor\n");

    // Read current temperature
    float temp = 0;
    if (client.getTemperature(&temp) == AETHER_SUCCESS)
        std::printf("[client] Current temperature: %.1f°C\n", temp);

    // Set a threshold below current temp → triggers OverTemperature notification
    if (client.setThreshold(50.0f, -10.0f) == AETHER_SUCCESS)
        std::printf("[client] Threshold set to [50.0, -10.0]°C\n");

    // Wait briefly for the notification to arrive
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::printf("[client] Done.\n");
    return 0;
}
