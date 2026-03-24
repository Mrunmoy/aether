/*
 * device_handlers.h — Service, method, and notification IDs plus
 *                     packed wire-format structs for DeviceService.
 *
 * Part of the aether-lite MCU firmware template.
 */

#ifndef DEVICE_HANDLERS_H
#define DEVICE_HANDLERS_H

#include <stdint.h>

/* ── Service ID (FNV-1a of "DeviceService") ─────────────────────── */

#define DEVICE_SERVICE_ID       0x56B672B6u

/* ── Method IDs ─────────────────────────────────────────────────── */

#define METHOD_GET_DEVICE_INFO  1
#define METHOD_READ_SENSOR      2
#define METHOD_SET_LED_STATE    3

/* ── Notification IDs ───────────────────────────────────────────── */

#define NOTIFY_SENSOR_UPDATE    1

/* ── Wire-format structs (little-endian, packed) ────────────────── */

#pragma pack(push, 1)

/* GetDeviceInfo response */
typedef struct
{
    char     device_name[32];   /* NUL-padded device name             */
    uint32_t fw_version;        /* Firmware version as 0xMMmmPP       */
} device_info_resp_t;

/* ReadSensor response */
typedef struct
{
    float    value;             /* Sensor reading (e.g., temperature) */
    uint32_t timestamp_ms;     /* Tick at time of read               */
} sensor_reading_resp_t;

/* SetLedState request */
typedef struct
{
    uint8_t  led_on;            /* 0 = off, 1 = on                   */
} led_state_req_t;

/* SetLedState response */
typedef struct
{
    uint8_t  status;            /* 0 = OK, non-zero = error           */
} led_state_resp_t;

/* SensorUpdate notification payload */
typedef struct
{
    float    value;
    uint32_t timestamp_ms;
} sensor_update_payload_t;

#pragma pack(pop)

/* ── Handler prototypes ─────────────────────────────────────────── */

int handle_get_device_info(const uint8_t *req, uint32_t req_len,
                           uint8_t *resp, uint32_t resp_cap,
                           uint32_t *resp_len);

int handle_read_sensor(const uint8_t *req, uint32_t req_len,
                       uint8_t *resp, uint32_t resp_cap,
                       uint32_t *resp_len);

int handle_set_led_state(const uint8_t *req, uint32_t req_len,
                         uint8_t *resp, uint32_t resp_cap,
                         uint32_t *resp_len);

#endif /* DEVICE_HANDLERS_H */
