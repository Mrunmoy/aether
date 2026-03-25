#include "aether_lite_dispatch.h"
#include "aether_lite_framing.h"
#include "aether_lite_endian.h"
#include "aether_lite_crc.h"
#include <string.h>

/* ── Service Table ───────────────────────────────────────────────── */

typedef struct
{
    uint32_t service_id;
    const al_method_entry_t *methods;
    uint8_t method_count;
} al_service_entry_t;

static al_service_entry_t s_services[AL_MAX_SERVICES];
static uint8_t s_service_count;
static const al_hal_t *s_hal;
static al_parser_t s_parser;
static uint32_t s_notify_seq = 0;

/* Static response payload buffer */
static uint8_t s_resp_buf[AL_MAX_PAYLOAD];

/* ── Built-in Handshake ──────────────────────────────────────────── */

static int handle_handshake(const al_frame_header_t *req_hdr,
                            const uint8_t *payload,
                            uint32_t payload_size)
{
    al_handshake_ack_t ack;
    al_frame_header_t resp_hdr;
    uint16_t status = 0;

    if (payload_size < sizeof(al_handshake_hello_t))
    {
        status = 1;  /* malformed */
    }
    else
    {
        al_handshake_hello_t hello;
        memcpy(&hello, payload, sizeof(hello));
        if (al_le_to_host16(hello.version) != AL_PROTOCOL_VERSION)
        {
            status = 2;  /* version mismatch */
        }
    }

    memset(&ack, 0, sizeof(ack));
    ack.version     = al_host_to_le16(AL_PROTOCOL_VERSION);
    ack.status      = al_host_to_le16(status);
    ack.max_payload = al_host_to_le32(AL_MAX_PAYLOAD);

    memset(&resp_hdr, 0, sizeof(resp_hdr));
    resp_hdr.version       = AL_PROTOCOL_VERSION;
    resp_hdr.flags         = AL_FRAME_RESPONSE;
    resp_hdr.service_id    = AL_HANDSHAKE_SERVICE_ID;
    resp_hdr.message_id    = AL_HANDSHAKE_ACK;
    resp_hdr.seq           = req_hdr->seq;
    resp_hdr.payload_bytes = (uint32_t)sizeof(ack);

    return al_frame_send(&resp_hdr, (const uint8_t *)&ack, sizeof(ack));
}

/* ── Dispatch a parsed frame ─────────────────────────────────────── */

static void dispatch_frame(void)
{
    const uint8_t *buf = al_parser_frame_data();
    al_frame_header_t hdr;
    const uint8_t *payload;
    uint8_t si;
    uint8_t mi;
    uint32_t resp_len;
    int rc;
    al_frame_header_t resp_hdr;

    /* Header sits right after the 4-byte length field */
    memcpy(&hdr, buf + AL_LEN_SIZE, AL_FRAME_HEADER_SIZE);
    al_frame_header_from_wire(&hdr);

    payload = buf + AL_LEN_SIZE + AL_FRAME_HEADER_SIZE;

    /* Built-in handshake */
    if (hdr.service_id == AL_HANDSHAKE_SERVICE_ID &&
        hdr.message_id == AL_HANDSHAKE_HELLO)
    {
        handle_handshake(&hdr, payload, s_parser.payload_size);
        return;
    }

    /* Reject payloads exceeding buffer capacity. */
    if (s_parser.payload_size > AL_MAX_PAYLOAD)
    {
        memset(&resp_hdr, 0, sizeof(resp_hdr));
        resp_hdr.version       = AL_PROTOCOL_VERSION;
        resp_hdr.flags         = AL_FRAME_RESPONSE;
        resp_hdr.service_id    = hdr.service_id;
        resp_hdr.message_id    = hdr.message_id;
        resp_hdr.seq           = hdr.seq;
        resp_hdr.aux           = (uint32_t)AL_ERR_OVERFLOW;
        (void)al_frame_send(&resp_hdr, NULL, 0);
        return;
    }

    /* Look up service */
    for (si = 0; si < s_service_count; si++)
    {
        if (s_services[si].service_id == hdr.service_id)
        {
            break;
        }
    }
    if (si == s_service_count)
    {
        memset(&resp_hdr, 0, sizeof(resp_hdr));
        resp_hdr.version       = AL_PROTOCOL_VERSION;
        resp_hdr.flags         = AL_FRAME_RESPONSE;
        resp_hdr.service_id    = hdr.service_id;
        resp_hdr.message_id    = hdr.message_id;
        resp_hdr.seq           = hdr.seq;
        resp_hdr.aux           = (uint32_t)AL_ERR_INVALID_SERVICE;
        (void)al_frame_send(&resp_hdr, NULL, 0);
        return;
    }

    /* Look up method */
    for (mi = 0; mi < s_services[si].method_count; mi++)
    {
        if (s_services[si].methods[mi].message_id == hdr.message_id)
        {
            break;
        }
    }
    if (mi == s_services[si].method_count)
    {
        memset(&resp_hdr, 0, sizeof(resp_hdr));
        resp_hdr.version       = AL_PROTOCOL_VERSION;
        resp_hdr.flags         = AL_FRAME_RESPONSE;
        resp_hdr.service_id    = hdr.service_id;
        resp_hdr.message_id    = hdr.message_id;
        resp_hdr.seq           = hdr.seq;
        resp_hdr.aux           = (uint32_t)AL_ERR_INVALID_METHOD;
        (void)al_frame_send(&resp_hdr, NULL, 0);
        return;
    }

    /* Call handler */
    resp_len = 0;
    rc = s_services[si].methods[mi].handler(
        payload, s_parser.payload_size,
        s_resp_buf, AL_MAX_PAYLOAD,
        &resp_len);

    if (resp_len > AL_MAX_PAYLOAD)
    {
        resp_len = 0;
        rc = AL_ERR_OVERFLOW;
    }

    /* Build and send response */
    memset(&resp_hdr, 0, sizeof(resp_hdr));
    resp_hdr.version       = AL_PROTOCOL_VERSION;
    resp_hdr.flags         = AL_FRAME_RESPONSE;
    resp_hdr.service_id    = hdr.service_id;
    resp_hdr.message_id    = hdr.message_id;
    resp_hdr.seq           = hdr.seq;
    resp_hdr.payload_bytes = resp_len;
    resp_hdr.aux           = (uint32_t)rc;

    (void)al_frame_send(&resp_hdr, s_resp_buf, resp_len);
}

/* ── Public API ──────────────────────────────────────────────────── */

void al_init(const al_hal_t *hal)
{
    s_hal = hal;
    s_service_count = 0;
    al_parser_init(&s_parser);
    al_framing_set_hal(hal);
    memset(s_services, 0, sizeof(s_services));
}

int al_poll(void)
{
    uint8_t rx_tmp[64];
    size_t n;
    size_t i;
    int frames = 0;

    if (!s_hal || !s_hal->rx_read)
    {
        return 0;
    }

    do
    {
        n = s_hal->rx_read(rx_tmp, sizeof(rx_tmp), s_hal->user_ctx);
        for (i = 0; i < n; i++)
        {
            size_t consumed = 0;
            if (al_parser_feed(&s_parser, &rx_tmp[i], 1, &consumed))
            {
                dispatch_frame();
                frames++;
            }
        }
    } while (n > 0);

    return frames;
}

bool al_is_idle(void)
{
    /* Use volatile read so compiler doesn't hoist this out of a polling loop.
       s_parser.state may be modified from ISR context via al_parser_feed(). */
    return (*(volatile uint8_t *)&s_parser.state) == AL_PS_SYNC_0;
}

int al_register_service(uint32_t service_id,
                         const al_method_entry_t *methods,
                         uint8_t method_count)
{
    if (s_service_count >= AL_MAX_SERVICES)
    {
        return AL_ERR_OVERFLOW;
    }
    if (!methods || method_count == 0 || method_count > AL_MAX_METHODS)
    {
        return AL_ERR_INVALID_ARGUMENT;
    }

    s_services[s_service_count].service_id   = service_id;
    s_services[s_service_count].methods      = methods;
    s_services[s_service_count].method_count = method_count;
    s_service_count++;

    return AL_SUCCESS;
}

#if AL_ENABLE_NOTIFICATIONS
int al_send_notify(uint32_t service_id, uint32_t message_id,
                   const uint8_t *payload, uint32_t payload_len)
{
    al_frame_header_t hdr;

    memset(&hdr, 0, sizeof(hdr));
    hdr.version       = AL_PROTOCOL_VERSION;
    hdr.flags         = AL_FRAME_NOTIFY;
    hdr.service_id    = service_id;
    hdr.message_id    = message_id;
    hdr.seq           = s_notify_seq++;
    hdr.payload_bytes = payload_len;

    return al_frame_send(&hdr, payload, payload_len);
}
#endif
