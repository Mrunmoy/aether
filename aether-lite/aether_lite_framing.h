#ifndef AETHER_LITE_FRAMING_H
#define AETHER_LITE_FRAMING_H

#include "aether_lite_types.h"
#include "aether_lite_hal.h"
#include <stddef.h>

/* ── Parser States ───────────────────────────────────────────────── */

typedef enum
{
    AL_PS_SYNC_0,
    AL_PS_SYNC_1,
    AL_PS_LENGTH,
    AL_PS_BODY,
    AL_PS_DONE
} al_parse_state_t;

/* ── Parser ──────────────────────────────────────────────────────── */

typedef struct
{
    al_parse_state_t state;
    uint16_t accum_pos;
    uint32_t frame_length;
    uint32_t payload_size;
    uint32_t running_crc;
    uint32_t rx_start_ms;      /* timestamp when frame reception started */
#if AL_ENABLE_DIAGNOSTICS
    uint32_t err_crc;
    uint32_t err_overflow;
    uint32_t err_timeout;
#endif
} al_parser_t;

/* Initialize / reset the parser. */
void al_parser_init(al_parser_t *p);

/* Feed bytes to parser. Returns 1 if a complete valid frame is ready.
 * *consumed is set to the number of bytes consumed (including the last
 * byte of the completed frame). Caller must re-feed remaining bytes.
 * Internal — prefer al_poll() for application use. */
int al_parser_feed(al_parser_t *p, const uint8_t *data, size_t len,
                   size_t *consumed);

/* Pointer to internal rx buffer (valid after al_parser_feed returns 1).
 * Layout: [LENGTH:4][HEADER:24][PAYLOAD:N][CRC:4] */
const uint8_t *al_parser_frame_data(void);

/* Build a frame in the static TX buffer and transmit via HAL. */
int al_frame_send(const al_frame_header_t *header,
                  const uint8_t *payload, uint32_t payload_len);

/* Internal: set HAL pointer for framing module (called by al_init). */
void al_framing_set_hal(const al_hal_t *hal);

#endif /* AETHER_LITE_FRAMING_H */
