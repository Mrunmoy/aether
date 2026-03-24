/*
 * device_handlers.c — Handler implementations for DeviceService.
 *
 * Part of the aether-lite MCU firmware template.
 */

#include "device_handlers.h"
#include "aether_lite_types.h"

#include <string.h>

/* ── Firmware identity ──────────────────────────────────────────── */

#define DEVICE_NAME     "AetherSensor-v1"
#define FW_VERSION      0x00010000u     /* 1.0.0 */

/* ----------------------------------------------------------------
 * GetDeviceInfo  (method 1)
 * Request : empty (ignored)
 * Response: device_info_resp_t
 * ---------------------------------------------------------------- */
int handle_get_device_info(const uint8_t *req, uint32_t req_len,
                           uint8_t *resp, uint32_t resp_cap,
                           uint32_t *resp_len)
{
    (void)req;
    (void)req_len;

    if (resp_cap < sizeof(device_info_resp_t))
        return AL_ERR_OVERFLOW;

    device_info_resp_t *info = (device_info_resp_t *)resp;
    memset(info, 0, sizeof(*info));
    strncpy(info->device_name, DEVICE_NAME, sizeof(info->device_name) - 1);
    info->fw_version = FW_VERSION;

    *resp_len = sizeof(device_info_resp_t);
    return AL_SUCCESS;
}

/* ----------------------------------------------------------------
 * ReadSensor  (method 2)
 * Request : empty (ignored)
 * Response: sensor_reading_resp_t
 * ---------------------------------------------------------------- */

/* >>> REPLACE with your actual ADC read function <<< */
static float read_adc_temperature(void)
{
    /* Stub: returns a fixed value.  Replace with e.g.:
     *   uint32_t raw = HAL_ADC_GetValue(&hadc1);
     *   return (float)raw * 3.3f / 4096.0f * TEMP_SCALE;
     */
    return 23.5f;
}

/* >>> REPLACE with your tick source <<< */
static uint32_t get_tick_stub(void)
{
    /* Stub: should call HAL_GetTick() or similar. */
    return 0;
}

int handle_read_sensor(const uint8_t *req, uint32_t req_len,
                       uint8_t *resp, uint32_t resp_cap,
                       uint32_t *resp_len)
{
    (void)req;
    (void)req_len;

    if (resp_cap < sizeof(sensor_reading_resp_t))
        return AL_ERR_OVERFLOW;

    sensor_reading_resp_t *reading = (sensor_reading_resp_t *)resp;
    reading->value        = read_adc_temperature();
    reading->timestamp_ms = get_tick_stub();

    *resp_len = sizeof(sensor_reading_resp_t);
    return AL_SUCCESS;
}

/* ----------------------------------------------------------------
 * SetLedState  (method 3)
 * Request : led_state_req_t
 * Response: led_state_resp_t
 * ---------------------------------------------------------------- */

/* >>> REPLACE with your actual GPIO call <<< */
static void gpio_set_led(uint8_t on)
{
    /* Stub.  Replace with e.g.:
     *   HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,
     *                     on ? GPIO_PIN_SET : GPIO_PIN_RESET);
     */
    (void)on;
}

int handle_set_led_state(const uint8_t *req, uint32_t req_len,
                         uint8_t *resp, uint32_t resp_cap,
                         uint32_t *resp_len)
{
    if (req_len < sizeof(led_state_req_t))
        return AL_ERR_INVALID_ARGUMENT;

    if (resp_cap < sizeof(led_state_resp_t))
        return AL_ERR_OVERFLOW;

    const led_state_req_t *cmd = (const led_state_req_t *)req;
    gpio_set_led(cmd->led_on);

    led_state_resp_t *status = (led_state_resp_t *)resp;
    status->status = 0;   /* OK */

    *resp_len = sizeof(led_state_resp_t);
    return AL_SUCCESS;
}
