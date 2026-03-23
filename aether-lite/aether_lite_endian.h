#ifndef AETHER_LITE_ENDIAN_H
#define AETHER_LITE_ENDIAN_H

#include "aether_lite_types.h"
#include <string.h>

/* ── Little-endian <-> host conversion (portable C99) ────────────── */

static inline uint16_t al_le_to_host16(uint16_t v)
{
    uint8_t b[2];
    memcpy(b, &v, 2);
    return (uint16_t)((uint16_t)b[0] | ((uint16_t)b[1] << 8));
}

static inline uint32_t al_le_to_host32(uint32_t v)
{
    uint8_t b[4];
    memcpy(b, &v, 4);
    return (uint32_t)b[0]         | ((uint32_t)b[1] << 8)
         | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

static inline uint16_t al_host_to_le16(uint16_t v)
{
    uint16_t r;
    uint8_t *p = (uint8_t *)&r;
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
    return r;
}

static inline uint32_t al_host_to_le32(uint32_t v)
{
    uint32_t r;
    uint8_t *p = (uint8_t *)&r;
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
    p[2] = (uint8_t)((v >> 16) & 0xFFu);
    p[3] = (uint8_t)((v >> 24) & 0xFFu);
    return r;
}

/* ── FrameHeader wire conversion (in-place) ──────────────────────── */

static inline void al_frame_header_to_wire(al_frame_header_t *h)
{
    h->version       = al_host_to_le16(h->version);
    h->flags         = al_host_to_le16(h->flags);
    h->service_id    = al_host_to_le32(h->service_id);
    h->message_id    = al_host_to_le32(h->message_id);
    h->seq           = al_host_to_le32(h->seq);
    h->payload_bytes = al_host_to_le32(h->payload_bytes);
    h->aux           = al_host_to_le32(h->aux);
}

static inline void al_frame_header_from_wire(al_frame_header_t *h)
{
    h->version       = al_le_to_host16(h->version);
    h->flags         = al_le_to_host16(h->flags);
    h->service_id    = al_le_to_host32(h->service_id);
    h->message_id    = al_le_to_host32(h->message_id);
    h->seq           = al_le_to_host32(h->seq);
    h->payload_bytes = al_le_to_host32(h->payload_bytes);
    h->aux           = al_le_to_host32(h->aux);
}

#endif /* AETHER_LITE_ENDIAN_H */
