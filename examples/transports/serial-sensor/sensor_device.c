#include "sensor_device.h"

#include "aether_lite_dispatch.h"
#include "aether_lite_endian.h"
#include "aether_lite_framing.h"
#include "aether_lite_hal.h"
#include "aether_lite_types.h"

#include <poll.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* ── Constants ──────────────────────────────────────────────────────── */

#define SENSOR_SERVICE_ID 0xA3B7C901u
#define METHOD_GET_TEMPERATURE 1
#define METHOD_GET_HUMIDITY    2
#define METHOD_GET_DEVICE_ID   3

/* ── Thread-local state passed via HAL user_ctx ────────────────────── */

typedef struct
{
    int fd;
    const volatile int *stop_flag;
} device_ctx_t;

/* ── HAL callbacks ──────────────────────────────────────────────────── */

static size_t hal_rx_read(uint8_t *buf, size_t max_bytes, void *user_ctx)
{
    device_ctx_t *ctx = (device_ctx_t *)user_ctx;
    struct pollfd pfd;
    ssize_t n;

    pfd.fd = ctx->fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    if (poll(&pfd, 1, 10) <= 0)
        return 0;
    if (!(pfd.revents & POLLIN))
        return 0;

    n = read(ctx->fd, buf, max_bytes);
    return (n > 0) ? (size_t)n : 0;
}

static int hal_tx_write(const uint8_t *buf, size_t len, void *user_ctx)
{
    device_ctx_t *ctx = (device_ctx_t *)user_ctx;
    size_t written = 0;

    while (written < len)
    {
        ssize_t n = write(ctx->fd, buf + written, len - written);
        if (n <= 0)
            return AL_ERR_DISCONNECTED;
        written += (size_t)n;
    }
    return AL_SUCCESS;
}

static uint32_t hal_get_tick_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

/* ── Simple PRNG for simulated variation ────────────────────────────── */

static uint32_t s_rng_state = 12345;

static float random_variation(float range)
{
    /* xorshift32 */
    s_rng_state ^= s_rng_state << 13;
    s_rng_state ^= s_rng_state >> 17;
    s_rng_state ^= s_rng_state << 5;
    /* Map to [-range, +range] */
    return ((float)(s_rng_state % 10001) / 10000.0f - 0.5f) * 2.0f * range;
}

/* ── Method handlers ────────────────────────────────────────────────── */

static int handle_get_temperature(
    const uint8_t *req, uint32_t req_len,
    uint8_t *resp, uint32_t resp_cap,
    uint32_t *resp_len)
{
    float temp;
    (void)req;
    (void)req_len;

    if (resp_cap < sizeof(float))
        return AL_ERR_OVERFLOW;

    temp = 22.5f + random_variation(1.5f);
    memcpy(resp, &temp, sizeof(float));
    *resp_len = sizeof(float);
    return AL_SUCCESS;
}

static int handle_get_humidity(
    const uint8_t *req, uint32_t req_len,
    uint8_t *resp, uint32_t resp_cap,
    uint32_t *resp_len)
{
    float hum;
    (void)req;
    (void)req_len;

    if (resp_cap < sizeof(float))
        return AL_ERR_OVERFLOW;

    hum = 45.0f + random_variation(5.0f);
    memcpy(resp, &hum, sizeof(float));
    *resp_len = sizeof(float);
    return AL_SUCCESS;
}

static int handle_get_device_id(
    const uint8_t *req, uint32_t req_len,
    uint8_t *resp, uint32_t resp_cap,
    uint32_t *resp_len)
{
    const char *id = "AETHER-SENSOR-001";
    uint32_t len = (uint32_t)strlen(id) + 1; /* include NUL */
    (void)req;
    (void)req_len;

    if (resp_cap < len)
        return AL_ERR_OVERFLOW;

    memcpy(resp, id, len);
    *resp_len = len;
    return AL_SUCCESS;
}

/* ── Method table ───────────────────────────────────────────────────── */

static const al_method_entry_t s_sensor_methods[] = {
    {METHOD_GET_TEMPERATURE, handle_get_temperature},
    {METHOD_GET_HUMIDITY,    handle_get_humidity},
    {METHOD_GET_DEVICE_ID,   handle_get_device_id},
};

/* ── Public entry point ─────────────────────────────────────────────── */

void sensor_device_run(int fd, const volatile int *stop_flag)
{
    device_ctx_t ctx;
    al_hal_t hal;

    ctx.fd = fd;
    ctx.stop_flag = stop_flag;

    memset(&hal, 0, sizeof(hal));
    hal.rx_read     = hal_rx_read;
    hal.tx_write    = hal_tx_write;
    hal.get_tick_ms = hal_get_tick_ms;
    hal.user_ctx    = &ctx;

    al_init(&hal);
    al_register_service(SENSOR_SERVICE_ID, s_sensor_methods, 3);

    while (!*stop_flag)
    {
        al_poll();
    }
}
