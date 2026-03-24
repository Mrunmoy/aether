/*
 * Aether-Lite Firmware Template
 * Target: STM32 (or any ARM Cortex-M with UART)
 * Transport: UART at 115200 baud
 *
 * This template shows the complete pattern for implementing an
 * aether-lite service on a bare-metal MCU. Replace the HAL stubs
 * with your actual MCU HAL calls.
 */

#include "aether_lite_config.h"
#include "aether_lite_hal.h"
#include "aether_lite_dispatch.h"
#include "device_handlers.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ================================================================
 * UART Ring Buffer
 * ================================================================
 * Single-producer (UART RX IRQ) / single-consumer (main loop).
 * Size must be a power of two so the mask trick works.
 * ================================================================ */

#define RING_SIZE  512u
#define RING_MASK  (RING_SIZE - 1u)

static volatile uint8_t  g_ring_buf[RING_SIZE];
static volatile uint32_t g_ring_head;  /* written by ISR  */
static volatile uint32_t g_ring_tail;  /* read by main    */

/* Called from UART RX interrupt handler. */
void ring_push(uint8_t byte)
{
    uint32_t next = (g_ring_head + 1u) & RING_MASK;
    if (next != g_ring_tail)          /* drop byte on overflow */
    {
        g_ring_buf[g_ring_head] = byte;
        g_ring_head = next;
    }
}

/* Drain up to max_bytes from the ring buffer into buf.
 * Returns number of bytes copied.  Safe to call from main loop. */
static size_t ring_drain(uint8_t *buf, size_t max_bytes)
{
    size_t n = 0;
    while (n < max_bytes && g_ring_tail != g_ring_head)
    {
        buf[n++] = g_ring_buf[g_ring_tail];
        g_ring_tail = (g_ring_tail + 1u) & RING_MASK;
    }
    return n;
}

/* ================================================================
 * HAL Callbacks
 * ================================================================ */

/* --- rx_read: provide bytes to the aether-lite parser ----------- */
static size_t hal_rx_read(uint8_t *buf, size_t max_bytes, void *ctx)
{
    (void)ctx;
    return ring_drain(buf, max_bytes);
}

/* --- tx_write: transmit a response frame over UART -------------- */
static int hal_tx_write(const uint8_t *buf, size_t len, void *ctx)
{
    (void)ctx;

    /*
     * >>> REPLACE WITH YOUR HAL <<<
     * Example for STM32 HAL:
     *   HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, 100);
     *
     * The function must block until all bytes are sent (or return
     * non-zero on failure).
     */
    (void)buf;
    (void)len;
    return 0;   /* 0 = success */
}

/* --- get_tick_ms: monotonic millisecond counter ----------------- */
static uint32_t hal_get_tick_ms(void)
{
    /*
     * >>> REPLACE WITH YOUR HAL <<<
     * Example for STM32 HAL:
     *   return HAL_GetTick();
     */
    return 0;
}

/* ================================================================
 * Service Registration
 * ================================================================ */

static const al_method_entry_t device_methods[] =
{
    { METHOD_GET_DEVICE_INFO, handle_get_device_info },
    { METHOD_READ_SENSOR,     handle_read_sensor     },
    { METHOD_SET_LED_STATE,   handle_set_led_state   },
};

#define DEVICE_METHOD_COUNT \
    (sizeof(device_methods) / sizeof(device_methods[0]))

/* ================================================================
 * Notification Helper
 * ================================================================ */

#define SENSOR_NOTIFY_INTERVAL_MS  1000u

#if AL_ENABLE_NOTIFICATIONS
static void send_sensor_notification(void)
{
    sensor_update_payload_t payload;
    payload.value        = 23.5f;  /* >>> replace with real read <<< */
    payload.timestamp_ms = hal_get_tick_ms();

    al_send_notify(DEVICE_SERVICE_ID, NOTIFY_SENSOR_UPDATE,
                   (const uint8_t *)&payload, sizeof(payload));
}
#endif

/* ================================================================
 * main()
 * ================================================================ */

int main(void)
{
    /*
     * >>> REPLACE: MCU system init <<<
     * Example for STM32:
     *   HAL_Init();
     *   SystemClock_Config();
     *   MX_GPIO_Init();
     *   MX_USART2_UART_Init();   // 115200-8N1
     *   __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
     */

    /* ---- Aether-lite init ---- */

    al_hal_t hal;
    memset(&hal, 0, sizeof(hal));
    hal.rx_read     = hal_rx_read;
    hal.tx_write    = hal_tx_write;
    hal.get_tick_ms = hal_get_tick_ms;
    hal.user_ctx    = (void *)0;   /* unused in this template */

    al_init(&hal);
    al_register_service(DEVICE_SERVICE_ID,
                        device_methods, DEVICE_METHOD_COUNT);

    /* ---- Main loop ---- */

#if AL_ENABLE_NOTIFICATIONS
    uint32_t last_notify_ms = 0;
#endif

    for (;;)
    {
        /* Process incoming aether-lite frames. */
        al_poll();

#if AL_ENABLE_NOTIFICATIONS
        /* Send periodic sensor notification. */
        uint32_t now = hal_get_tick_ms();
        if ((now - last_notify_ms) >= SENSOR_NOTIFY_INTERVAL_MS)
        {
            last_notify_ms = now;
            send_sensor_notification();
        }
#endif

        /*
         * Optional: enter low-power sleep when the parser is idle
         * and the next notification is not due.
         *
         *   if (al_is_idle())
         *       __WFI();
         */
    }

    /* Not reached on bare-metal, but keeps the compiler happy. */
    return 0;
}
