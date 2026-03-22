/*
 * temp_server.cpp — Temperature sensor service using the Aether C API from C++.
 *
 * Shows how to wrap the C API in an RAII class and dispatch requests
 * by method ID.  The server broadcasts an OverTemperature notification
 * when a client sets a threshold below the current reading.
 *
 * Build with SDK:
 *   g++ -std=c++17 -o temp_server temp_server.cpp \
 *       -I<sdk>/include <sdk>/lib/libaether.a -lpthread
 */

#include "aether_ipc.h"
#include "protocol.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <unistd.h>

// ── Thin RAII wrapper around aether_service_t ────────────────────────

class TempSensorService
{
public:
    TempSensorService(const char *name)
    {
        int rc = aether_service_create(name, onRequestThunk, this, &m_svc);
        if (rc != AETHER_SUCCESS) {
            std::fprintf(stderr, "service_create failed: %d\n", rc);
            std::exit(1);
        }
    }

    ~TempSensorService()
    {
        if (m_svc)
            aether_service_destroy(m_svc);
    }

    // Not copyable
    TempSensorService(const TempSensorService &) = delete;
    TempSensorService &operator=(const TempSensorService &) = delete;

    bool start()
    {
        return aether_service_start(m_svc) == AETHER_SUCCESS;
    }

    bool isRunning() const
    {
        return aether_service_is_running(m_svc);
    }

private:
    aether_service_t m_svc = nullptr;
    float m_temperature = 72.5f;
    float m_thresholdHigh = 100.0f;
    float m_thresholdLow = -40.0f;

    // Static thunk that forwards to the member function via user_data
    static int onRequestThunk(uint32_t service_id, uint32_t message_id,
                              const uint8_t *req, uint32_t req_len,
                              uint8_t **resp, uint32_t *resp_len,
                              void *user_data)
    {
        auto *self = static_cast<TempSensorService *>(user_data);
        return self->onRequest(service_id, message_id, req, req_len, resp, resp_len);
    }

    int onRequest(uint32_t /*service_id*/, uint32_t message_id,
                  const uint8_t *req, uint32_t req_len,
                  uint8_t **resp, uint32_t *resp_len)
    {
        switch (message_id) {

        case temp::kMethodGetTemp:
            return handleGetTemperature(resp, resp_len);

        case temp::kMethodSetThreshold:
            return handleSetThreshold(req, req_len, resp, resp_len);

        default:
            *resp = nullptr;
            *resp_len = 0;
            return AETHER_ERR_INVALID_METHOD;
        }
    }

    int handleGetTemperature(uint8_t **resp, uint32_t *resp_len)
    {
        *resp = static_cast<uint8_t *>(std::malloc(sizeof(float)));
        if (!*resp) return -1;
        std::memcpy(*resp, &m_temperature, sizeof(float));
        *resp_len = sizeof(float);
        std::printf("[server] GetTemperature → %.1f°C\n", m_temperature);
        return AETHER_SUCCESS;
    }

    int handleSetThreshold(const uint8_t *req, uint32_t req_len,
                           uint8_t **resp, uint32_t *resp_len)
    {
        if (req_len < sizeof(temp::Thresholds)) {
            *resp = nullptr;
            *resp_len = 0;
            return AETHER_ERR_INVALID_ARGUMENT;
        }

        temp::Thresholds t;
        std::memcpy(&t, req, sizeof(t));
        m_thresholdHigh = t.high;
        m_thresholdLow = t.low;
        std::printf("[server] SetThreshold(high=%.1f, low=%.1f)\n", t.high, t.low);

        *resp = nullptr;
        *resp_len = 0;

        if (m_temperature > m_thresholdHigh) {
            std::printf("[server] %.1f°C exceeds threshold — broadcasting notification\n",
                        m_temperature);
            aether_service_notify(m_svc, temp::kServiceId, temp::kNotifyOverTemp,
                                  reinterpret_cast<const uint8_t *>(&m_temperature),
                                  sizeof(float));
        }
        return AETHER_SUCCESS;
    }
};

// ── Main ─────────────────────────────────────────────────────────────

static std::atomic<bool> g_running{true};
static void onSignal(int) { g_running = false; }

int main()
{
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    TempSensorService service("temp_sensor");
    if (!service.start()) {
        std::fprintf(stderr, "Failed to start service\n");
        return 1;
    }

    std::printf("[server] Temperature sensor running. Ctrl+C to stop.\n");

    while (g_running && service.isRunning())
        pause();

    std::printf("[server] Stopped.\n");
    return 0;
}
