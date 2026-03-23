#ifndef AETHER_LITE_TYPES_H
#define AETHER_LITE_TYPES_H

#include "aether_lite_config.h"
#include <stdint.h>

/* ── Compile-time assert (portable C99) ──────────────────────────── */

#define AL_CONCAT_(a, b) a##b
#define AL_CONCAT(a, b)  AL_CONCAT_(a, b)
#define AL_STATIC_ASSERT(cond, name) \
    typedef char al_sa_##name[(cond) ? 1 : -1]

/* ── Error Codes ─────────────────────────────────────────────────── */
/* Negative = framework, 0 = success, positive = user-defined. */

#define AL_SUCCESS              0
#define AL_ERR_DISCONNECTED     (-1)
#define AL_ERR_TIMEOUT          (-2)
#define AL_ERR_INVALID_SERVICE  (-3)
#define AL_ERR_INVALID_METHOD   (-4)
#define AL_ERR_VERSION_MISMATCH (-5)
#define AL_ERR_OVERFLOW         (-6)
#define AL_ERR_STOPPED          (-7)
#define AL_ERR_INVALID_ARGUMENT (-8)

/* ── Frame Flags ─────────────────────────────────────────────────── */

#define AL_FRAME_REQUEST  0x0001
#define AL_FRAME_RESPONSE 0x0002
#define AL_FRAME_NOTIFY   0x0004

/* ── Sync Pattern ────────────────────────────────────────────────── */

#define AL_SYNC_BYTE_0  0xAA
#define AL_SYNC_BYTE_1  0x55

/* ── Frame Layout Constants ──────────────────────────────────────── */

#define AL_SYNC_SIZE         2
#define AL_LEN_SIZE          4
#define AL_FRAME_HEADER_SIZE 24
#define AL_CRC_SIZE          4

#define AL_HDR_OFFSET        (AL_SYNC_SIZE + AL_LEN_SIZE)               /* 6  */
#define AL_PLD_OFFSET        (AL_HDR_OFFSET + AL_FRAME_HEADER_SIZE)     /* 30 */
#define AL_TX_OVERHEAD       (AL_SYNC_SIZE + AL_LEN_SIZE + AL_FRAME_HEADER_SIZE + AL_CRC_SIZE) /* 34 */

#define AL_MAX_FRAME_LENGTH  (AL_FRAME_HEADER_SIZE + AL_MAX_PAYLOAD + AL_CRC_SIZE)
#define AL_MIN_FRAME_LENGTH  (AL_FRAME_HEADER_SIZE + AL_CRC_SIZE) /* 28 */

/* ── Handshake Constants ─────────────────────────────────────────── */

#define AL_HANDSHAKE_SERVICE_ID  0
#define AL_HANDSHAKE_HELLO       1
#define AL_HANDSHAKE_ACK         2
#define AL_PROTOCOL_VERSION      1

/* ── Frame Header (24 bytes, wire format is little-endian) ───────── */

typedef struct
{
    uint16_t version;
    uint16_t flags;
    uint32_t service_id;
    uint32_t message_id;
    uint32_t seq;
    uint32_t payload_bytes;
    uint32_t aux;
} al_frame_header_t;

AL_STATIC_ASSERT(sizeof(al_frame_header_t) == 24, frame_header_size);

/* ── Handshake Hello (12 bytes) ──────────────────────────────────── */

typedef struct
{
    uint16_t version;
    uint16_t reserved0;
    uint32_t max_payload;
    uint32_t reserved1;
} al_handshake_hello_t;

AL_STATIC_ASSERT(sizeof(al_handshake_hello_t) == 12, handshake_hello_size);

/* ── Handshake Ack (16 bytes) ────────────────────────────────────── */

typedef struct
{
    uint16_t version;
    uint16_t status;
    uint32_t max_payload;
    uint32_t reserved0;
    uint32_t reserved1;
} al_handshake_ack_t;

AL_STATIC_ASSERT(sizeof(al_handshake_ack_t) == 16, handshake_ack_size);

#endif /* AETHER_LITE_TYPES_H */
