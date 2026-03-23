#include "aether_lite_framing.h"
#include "aether_lite_crc.h"
#include "aether_lite_endian.h"
#include <string.h>

/* ── Static Buffers ──────────────────────────────────────────────── */

static uint8_t s_rx_buf[AL_LEN_SIZE + AL_MAX_FRAME_LENGTH];
static uint8_t s_tx_buf[AL_TX_OVERHEAD + AL_MAX_PAYLOAD];
static const al_hal_t *s_hal;

/* ── HAL accessor ────────────────────────────────────────────────── */

void al_framing_set_hal(const al_hal_t *hal)
{
    s_hal = hal;
}

/* ── Parser ──────────────────────────────────────────────────────── */

void al_parser_init(al_parser_t *p)
{
    memset(p, 0, sizeof(*p));
    p->state = AL_PS_SYNC_0;
}

const uint8_t *al_parser_frame_data(void)
{
    return s_rx_buf;
}

AL_STATIC_ASSERT(AL_MAX_FRAME_LENGTH <= 65535u, accum_pos_fits_uint16);

int al_parser_feed(al_parser_t *p, const uint8_t *data, size_t len,
                   size_t *consumed)
{
    size_t i;

    for (i = 0; i < len; i++)
    {
        uint8_t byte = data[i];

        switch (p->state)
        {
        case AL_PS_SYNC_0:
            if (byte == AL_SYNC_BYTE_0)
            {
                p->state = AL_PS_SYNC_1;
            }
            break;

        case AL_PS_SYNC_1:
            if (byte == AL_SYNC_BYTE_1)
            {
                p->state = AL_PS_LENGTH;
                p->accum_pos = 0;
                p->running_crc = 0xFFFFFFFFu;
            }
            else if (byte != AL_SYNC_BYTE_0)
            {
                p->state = AL_PS_SYNC_0;
            }
            /* If byte == AL_SYNC_BYTE_0, stay in SYNC_1 */
            break;

        case AL_PS_LENGTH:
            s_rx_buf[p->accum_pos] = byte;
            p->accum_pos++;
            if (p->accum_pos == AL_LEN_SIZE)
            {
                uint32_t fl;
                memcpy(&fl, s_rx_buf, AL_LEN_SIZE);
                fl = al_le_to_host32(fl);

                if (fl < (uint32_t)AL_MIN_FRAME_LENGTH ||
                    fl > (uint32_t)AL_MAX_FRAME_LENGTH)
                {
                    p->state = AL_PS_SYNC_0;
#if AL_ENABLE_DIAGNOSTICS
                    p->err_overflow++;
#endif
                    break;
                }

                p->frame_length = fl;
                p->payload_size = fl - AL_FRAME_HEADER_SIZE - AL_CRC_SIZE;
                p->running_crc = al_crc32_update(p->running_crc,
                                                 s_rx_buf, AL_LEN_SIZE);
                p->accum_pos = 0;
                p->state = AL_PS_BODY;

                if (s_hal && s_hal->get_tick_ms)
                {
                    p->rx_start_ms = s_hal->get_tick_ms();
                }
            }
            break;

        case AL_PS_BODY:
            /* Timeout check */
#if AL_RX_TIMEOUT_MS > 0
            if (s_hal && s_hal->get_tick_ms)
            {
                uint32_t now = s_hal->get_tick_ms();
                if (now - p->rx_start_ms > AL_RX_TIMEOUT_MS)
                {
                    p->state = AL_PS_SYNC_0;
#if AL_ENABLE_DIAGNOSTICS
                    p->err_timeout++;
#endif
                    break;
                }
            }
#endif
            s_rx_buf[AL_LEN_SIZE + p->accum_pos] = byte;

            /* Update CRC for header + payload, NOT the CRC field */
            if (p->accum_pos < p->frame_length - AL_CRC_SIZE)
            {
                p->running_crc = al_crc32_update(p->running_crc,
                                                 &byte, 1);
            }

            p->accum_pos++;

            if (p->accum_pos == p->frame_length)
            {
                /* Finalize CRC */
                uint32_t computed = p->running_crc ^ 0xFFFFFFFFu;
                uint32_t received;
                memcpy(&received,
                       &s_rx_buf[AL_LEN_SIZE + p->frame_length
                                 - AL_CRC_SIZE],
                       AL_CRC_SIZE);
                received = al_le_to_host32(received);

                if (computed == received)
                {
                    p->state = AL_PS_SYNC_0;
                    *consumed = i + 1;
                    return 1;
                }
                else
                {
                    p->state = AL_PS_SYNC_0;
#if AL_ENABLE_DIAGNOSTICS
                    p->err_crc++;
#endif
                }
            }
            break;

        case AL_PS_DONE:
            p->state = AL_PS_SYNC_0;
            break;
        }
    }

    *consumed = len;
    return 0;
}

/* ── Frame Builder ───────────────────────────────────────────────── */

int al_frame_send(const al_frame_header_t *header,
                  const uint8_t *payload, uint32_t payload_len)
{
    uint32_t frame_len;
    uint32_t frame_len_le;
    uint32_t crc;
    uint32_t crc_le;
    uint32_t total;
    al_frame_header_t wire_hdr;

    if (payload_len > AL_MAX_PAYLOAD)
    {
        return AL_ERR_OVERFLOW;
    }
    if (payload_len > 0 && !payload)
    {
        return AL_ERR_INVALID_ARGUMENT;
    }
    if (!s_hal || !s_hal->tx_write)
    {
        return AL_ERR_DISCONNECTED;
    }

    /* Sync */
    s_tx_buf[0] = AL_SYNC_BYTE_0;
    s_tx_buf[1] = AL_SYNC_BYTE_1;

    /* Frame length: header + payload + CRC */
    frame_len = AL_FRAME_HEADER_SIZE + payload_len + AL_CRC_SIZE;
    frame_len_le = al_host_to_le32(frame_len);
    memcpy(&s_tx_buf[AL_SYNC_SIZE], &frame_len_le, AL_LEN_SIZE);

    /* Header (convert to wire / little-endian) */
    wire_hdr = *header;
    al_frame_header_to_wire(&wire_hdr);
    memcpy(&s_tx_buf[AL_HDR_OFFSET], &wire_hdr, AL_FRAME_HEADER_SIZE);

    /* Payload */
    if (payload_len > 0 && payload)
    {
        memcpy(&s_tx_buf[AL_PLD_OFFSET], payload, payload_len);
    }

    /* CRC over LENGTH + HEADER + PAYLOAD */
    crc = al_crc32(&s_tx_buf[AL_SYNC_SIZE],
                   AL_LEN_SIZE + AL_FRAME_HEADER_SIZE + payload_len);
    crc_le = al_host_to_le32(crc);
    memcpy(&s_tx_buf[AL_PLD_OFFSET + payload_len], &crc_le, AL_CRC_SIZE);

    /* Total wire bytes */
    total = AL_TX_OVERHEAD + payload_len;

    return s_hal->tx_write(s_tx_buf, total, s_hal->user_ctx);
}
