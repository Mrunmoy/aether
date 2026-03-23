#include <gtest/gtest.h>
#include <cstring>
#include <cstdint>

extern "C"
{
#include "aether_lite_types.h"
#include "aether_lite_crc.h"
#include "aether_lite_endian.h"
#include "aether_lite_framing.h"
#include "aether_lite_dispatch.h"
#include "aether_lite_fnv1a.h"
#include "aether_lite_hal.h"
}

/* ── Mock HAL infrastructure ─────────────────────────────────────── */

namespace
{

uint8_t  g_tx_capture[1024];
size_t   g_tx_capture_len;
int      g_tx_return;

uint8_t  g_rx_data[1024];
size_t   g_rx_data_len;
size_t   g_rx_data_pos;

uint32_t g_tick_ms;

size_t mock_rx_read(uint8_t *buf, size_t max_bytes, void * /*ctx*/)
{
    size_t avail = g_rx_data_len - g_rx_data_pos;
    if (avail == 0) return 0;
    size_t n = (avail < max_bytes) ? avail : max_bytes;
    std::memcpy(buf, g_rx_data + g_rx_data_pos, n);
    g_rx_data_pos += n;
    return n;
}

int mock_tx_write(const uint8_t *buf, size_t len, void * /*ctx*/)
{
    std::memcpy(g_tx_capture, buf, len);
    g_tx_capture_len = len;
    return g_tx_return;
}

uint32_t mock_get_tick_ms(void)
{
    return g_tick_ms;
}

void reset_mock()
{
    std::memset(g_tx_capture, 0, sizeof(g_tx_capture));
    g_tx_capture_len = 0;
    g_tx_return = 0;
    std::memset(g_rx_data, 0, sizeof(g_rx_data));
    g_rx_data_len = 0;
    g_rx_data_pos = 0;
    g_tick_ms = 0;
}

al_hal_t make_hal()
{
    al_hal_t hal;
    std::memset(&hal, 0, sizeof(hal));
    hal.rx_read     = mock_rx_read;
    hal.tx_write    = mock_tx_write;
    hal.get_tick_ms = mock_get_tick_ms;
    hal.user_ctx    = nullptr;
    return hal;
}

/* Build a valid wire frame into buf, return total length. */
size_t build_frame(uint8_t *buf,
                   const al_frame_header_t &hdr,
                   const uint8_t *payload, uint32_t payload_len)
{
    buf[0] = AL_SYNC_BYTE_0;
    buf[1] = AL_SYNC_BYTE_1;

    uint32_t frame_len = AL_FRAME_HEADER_SIZE + payload_len + AL_CRC_SIZE;
    uint32_t frame_len_le = al_host_to_le32(frame_len);
    std::memcpy(&buf[AL_SYNC_SIZE], &frame_len_le, AL_LEN_SIZE);

    al_frame_header_t wire_hdr = hdr;
    al_frame_header_to_wire(&wire_hdr);
    std::memcpy(&buf[AL_HDR_OFFSET], &wire_hdr, AL_FRAME_HEADER_SIZE);

    if (payload_len > 0 && payload)
    {
        std::memcpy(&buf[AL_PLD_OFFSET], payload, payload_len);
    }

    uint32_t crc = al_crc32(&buf[AL_SYNC_SIZE],
                            AL_LEN_SIZE + AL_FRAME_HEADER_SIZE + payload_len);
    uint32_t crc_le = al_host_to_le32(crc);
    std::memcpy(&buf[AL_PLD_OFFSET + payload_len], &crc_le, AL_CRC_SIZE);

    return AL_TX_OVERHEAD + payload_len;
}

/* Handler mock state */
bool     g_handler_called;
uint32_t g_handler_req_len;

int mock_handler(const uint8_t *req, uint32_t req_len,
                 uint8_t *resp, uint32_t resp_cap,
                 uint32_t *resp_len)
{
    (void)resp_cap;
    g_handler_called = true;
    g_handler_req_len = req_len;
    /* Echo first byte as response */
    if (req_len > 0 && resp)
    {
        resp[0] = req[0];
        *resp_len = 1;
    }
    else
    {
        *resp_len = 0;
    }
    return 0;
}

} /* anonymous namespace */

/* ── 1. CRC32 known vector ───────────────────────────────────────── */

TEST(AetherLiteTest, CRC32KnownVector)
{
    const uint8_t data[] = "123456789";
    uint32_t crc = al_crc32(data, 9);
    EXPECT_EQ(crc, 0xCBF43926u);
}

/* ── 2. Endian round-trip on FrameHeader ─────────────────────────── */

TEST(AetherLiteTest, EndianRoundTrip)
{
    al_frame_header_t orig;
    orig.version       = 1;
    orig.flags         = 0x0001;
    orig.service_id    = 0xDEADBEEF;
    orig.message_id    = 42;
    orig.seq           = 100;
    orig.payload_bytes = 256;
    orig.aux           = 0x12345678;

    al_frame_header_t wire = orig;
    al_frame_header_to_wire(&wire);
    al_frame_header_from_wire(&wire);

    EXPECT_EQ(wire.version,       orig.version);
    EXPECT_EQ(wire.flags,         orig.flags);
    EXPECT_EQ(wire.service_id,    orig.service_id);
    EXPECT_EQ(wire.message_id,    orig.message_id);
    EXPECT_EQ(wire.seq,           orig.seq);
    EXPECT_EQ(wire.payload_bytes, orig.payload_bytes);
    EXPECT_EQ(wire.aux,           orig.aux);
}

/* ── 3. Parser with valid frame ──────────────────────────────────── */

TEST(AetherLiteTest, ParserValidFrame)
{
    reset_mock();
    al_hal_t hal = make_hal();
    al_framing_set_hal(&hal);

    al_parser_t parser;
    al_parser_init(&parser);

    al_frame_header_t hdr;
    std::memset(&hdr, 0, sizeof(hdr));
    hdr.version       = 1;
    hdr.flags         = AL_FRAME_REQUEST;
    hdr.service_id    = 100;
    hdr.message_id    = 5;
    hdr.seq           = 1;
    hdr.payload_bytes = 4;

    uint8_t payload[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t frame[128];
    size_t frame_len = build_frame(frame, hdr, payload, 4);

    size_t consumed = 0;
    int result = al_parser_feed(&parser, frame, frame_len, &consumed);
    EXPECT_EQ(result, 1);
    EXPECT_EQ(consumed, frame_len);

    /* Verify parsed header */
    const uint8_t *data = al_parser_frame_data();
    al_frame_header_t parsed_hdr;
    std::memcpy(&parsed_hdr, data + AL_LEN_SIZE, AL_FRAME_HEADER_SIZE);
    al_frame_header_from_wire(&parsed_hdr);
    EXPECT_EQ(parsed_hdr.service_id, 100u);
    EXPECT_EQ(parsed_hdr.message_id, 5u);

    /* Verify parsed payload */
    const uint8_t *parsed_payload = data + AL_LEN_SIZE + AL_FRAME_HEADER_SIZE;
    EXPECT_EQ(parsed_payload[0], 0x01);
    EXPECT_EQ(parsed_payload[3], 0x04);
}

/* ── 4. Parser with bad CRC ──────────────────────────────────────── */

TEST(AetherLiteTest, ParserBadCRC)
{
    reset_mock();
    al_hal_t hal = make_hal();
    al_framing_set_hal(&hal);

    al_parser_t parser;
    al_parser_init(&parser);

    al_frame_header_t hdr;
    std::memset(&hdr, 0, sizeof(hdr));
    hdr.version    = 1;
    hdr.flags      = AL_FRAME_REQUEST;
    hdr.service_id = 1;

    uint8_t frame[128];
    size_t frame_len = build_frame(frame, hdr, nullptr, 0);

    /* Corrupt one byte of the CRC */
    frame[frame_len - 1] ^= 0xFF;

    size_t consumed = 0;
    int result = al_parser_feed(&parser, frame, frame_len, &consumed);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(consumed, frame_len);

#if AL_ENABLE_DIAGNOSTICS
    EXPECT_EQ(parser.err_crc, 1u);
#endif
}

/* ── 5. Parser timeout / re-sync ─────────────────────────────────── */

TEST(AetherLiteTest, ParserTimeout)
{
    reset_mock();
    al_hal_t hal = make_hal();
    al_framing_set_hal(&hal);

    al_parser_t parser;
    al_parser_init(&parser);

    /* Build a valid frame but only send the first part */
    al_frame_header_t hdr;
    std::memset(&hdr, 0, sizeof(hdr));
    hdr.version    = 1;
    hdr.flags      = AL_FRAME_REQUEST;
    hdr.service_id = 1;

    uint8_t frame[128];
    size_t frame_len = build_frame(frame, hdr, nullptr, 0);

    /* Feed sync + length + partial body */
    size_t consumed = 0;
    g_tick_ms = 100;
    al_parser_feed(&parser, frame, 10, &consumed);  /* partial */
    EXPECT_EQ(parser.state, AL_PS_BODY);

    /* Advance time past the timeout */
    g_tick_ms = 100 + AL_RX_TIMEOUT_MS + 1;

    /* Feed one more byte — should trigger timeout */
    uint8_t dummy = 0x00;
    al_parser_feed(&parser, &dummy, 1, &consumed);
    EXPECT_EQ(parser.state, AL_PS_SYNC_0);

#if AL_ENABLE_DIAGNOSTICS
    EXPECT_EQ(parser.err_timeout, 1u);
#endif
}

/* ── 6. Embedded 0xAA55 in payload does not false-sync ───────────── */

TEST(AetherLiteTest, ParserNoFalseSyncInPayload)
{
    reset_mock();
    al_hal_t hal = make_hal();
    al_framing_set_hal(&hal);

    al_parser_t parser;
    al_parser_init(&parser);

    al_frame_header_t hdr;
    std::memset(&hdr, 0, sizeof(hdr));
    hdr.version       = 1;
    hdr.flags         = AL_FRAME_REQUEST;
    hdr.service_id    = 1;
    hdr.payload_bytes = 4;

    /* Payload contains 0xAA 0x55 */
    uint8_t payload[4] = {0xAA, 0x55, 0xAA, 0x55};
    uint8_t frame[128];
    size_t frame_len = build_frame(frame, hdr, payload, 4);

    size_t consumed = 0;
    int result = al_parser_feed(&parser, frame, frame_len, &consumed);
    EXPECT_EQ(result, 1);

    /* Verify payload was correctly received */
    const uint8_t *data = al_parser_frame_data();
    const uint8_t *parsed_pld = data + AL_LEN_SIZE + AL_FRAME_HEADER_SIZE;
    EXPECT_EQ(parsed_pld[0], 0xAA);
    EXPECT_EQ(parsed_pld[1], 0x55);
    EXPECT_EQ(parsed_pld[2], 0xAA);
    EXPECT_EQ(parsed_pld[3], 0x55);
}

/* ── 7. Consecutive 0xAA bytes before 0x55 ───────────────────────── */

TEST(AetherLiteTest, ParserConsecutiveSync0)
{
    reset_mock();
    al_hal_t hal = make_hal();
    al_framing_set_hal(&hal);

    al_parser_t parser;
    al_parser_init(&parser);

    al_frame_header_t hdr;
    std::memset(&hdr, 0, sizeof(hdr));
    hdr.version    = 1;
    hdr.flags      = AL_FRAME_REQUEST;
    hdr.service_id = 1;

    uint8_t frame[128];
    size_t frame_len = build_frame(frame, hdr, nullptr, 0);

    /* Prepend multiple 0xAA bytes (the parser should stay in SYNC_1
     * for each 0xAA and only proceed when 0x55 arrives). */
    uint8_t prefixed[256];
    prefixed[0] = 0xAA;
    prefixed[1] = 0xAA;
    prefixed[2] = 0xAA;
    /* Now the real frame starts (which also begins with 0xAA 0x55) */
    std::memcpy(prefixed + 3, frame, frame_len);

    size_t consumed = 0;
    int result = al_parser_feed(&parser, prefixed, 3 + frame_len, &consumed);
    EXPECT_EQ(result, 1);
}

/* ── 8. Frame builder (al_frame_send via mock HAL) ───────────────── */

TEST(AetherLiteTest, FrameBuilder)
{
    reset_mock();
    al_hal_t hal = make_hal();
    al_framing_set_hal(&hal);

    al_frame_header_t hdr;
    std::memset(&hdr, 0, sizeof(hdr));
    hdr.version       = 1;
    hdr.flags         = AL_FRAME_RESPONSE;
    hdr.service_id    = 42;
    hdr.message_id    = 7;
    hdr.seq           = 10;
    hdr.payload_bytes = 2;

    uint8_t payload[2] = {0xCA, 0xFE};
    int rc = al_frame_send(&hdr, payload, 2);
    EXPECT_EQ(rc, 0);

    /* Verify sync bytes */
    EXPECT_EQ(g_tx_capture[0], AL_SYNC_BYTE_0);
    EXPECT_EQ(g_tx_capture[1], AL_SYNC_BYTE_1);

    /* Verify total length */
    EXPECT_EQ(g_tx_capture_len, (size_t)(AL_TX_OVERHEAD + 2));

    /* Verify the transmitted frame can be parsed back */
    al_parser_t parser;
    al_parser_init(&parser);
    size_t consumed = 0;
    int result = al_parser_feed(&parser, g_tx_capture, g_tx_capture_len, &consumed);
    EXPECT_EQ(result, 1);

    const uint8_t *data = al_parser_frame_data();
    al_frame_header_t parsed;
    std::memcpy(&parsed, data + AL_LEN_SIZE, AL_FRAME_HEADER_SIZE);
    al_frame_header_from_wire(&parsed);
    EXPECT_EQ(parsed.service_id, 42u);
    EXPECT_EQ(parsed.message_id, 7u);
    EXPECT_EQ(parsed.seq, 10u);
}

/* ── 9. Dispatch with mock service ───────────────────────────────── */

TEST(AetherLiteTest, DispatchMockService)
{
    reset_mock();
    g_handler_called = false;
    al_hal_t hal = make_hal();
    al_init(&hal);

    const uint32_t svc_id = 0x12345678;
    al_method_entry_t methods[1];
    methods[0].message_id = 99;
    methods[0].handler    = mock_handler;

    int rc = al_register_service(svc_id, methods, 1);
    EXPECT_EQ(rc, AL_SUCCESS);

    /* Build a request frame */
    al_frame_header_t req_hdr;
    std::memset(&req_hdr, 0, sizeof(req_hdr));
    req_hdr.version       = 1;
    req_hdr.flags         = AL_FRAME_REQUEST;
    req_hdr.service_id    = svc_id;
    req_hdr.message_id    = 99;
    req_hdr.seq           = 1;
    req_hdr.payload_bytes = 3;

    uint8_t payload[3] = {0x42, 0x43, 0x44};
    size_t frame_len = build_frame(g_rx_data, req_hdr, payload, 3);
    g_rx_data_len = frame_len;
    g_rx_data_pos = 0;

    int frames = al_poll();
    EXPECT_EQ(frames, 1);
    EXPECT_TRUE(g_handler_called);
    EXPECT_EQ(g_handler_req_len, 3u);

    /* Verify a response was sent */
    EXPECT_GT(g_tx_capture_len, 0u);
    EXPECT_EQ(g_tx_capture[0], AL_SYNC_BYTE_0);

    /* Parse the response */
    al_parser_t rp;
    al_parser_init(&rp);
    size_t consumed = 0;
    int res = al_parser_feed(&rp, g_tx_capture, g_tx_capture_len, &consumed);
    EXPECT_EQ(res, 1);

    const uint8_t *rdata = al_parser_frame_data();
    al_frame_header_t resp_hdr;
    std::memcpy(&resp_hdr, rdata + AL_LEN_SIZE, AL_FRAME_HEADER_SIZE);
    al_frame_header_from_wire(&resp_hdr);
    EXPECT_EQ(resp_hdr.flags, (uint16_t)AL_FRAME_RESPONSE);
    EXPECT_EQ(resp_hdr.service_id, svc_id);
    EXPECT_EQ(resp_hdr.message_id, 99u);
    EXPECT_EQ(resp_hdr.seq, 1u);
}

/* ── 10. Built-in handshake handler ──────────────────────────────── */

TEST(AetherLiteTest, HandshakeBuiltIn)
{
    reset_mock();
    al_hal_t hal = make_hal();
    al_init(&hal);

    /* Build HandshakeHello request */
    al_handshake_hello_t hello;
    std::memset(&hello, 0, sizeof(hello));
    hello.version     = al_host_to_le16(AL_PROTOCOL_VERSION);
    hello.max_payload = al_host_to_le32(256);

    al_frame_header_t req_hdr;
    std::memset(&req_hdr, 0, sizeof(req_hdr));
    req_hdr.version       = AL_PROTOCOL_VERSION;
    req_hdr.flags         = AL_FRAME_REQUEST;
    req_hdr.service_id    = AL_HANDSHAKE_SERVICE_ID;
    req_hdr.message_id    = AL_HANDSHAKE_HELLO;
    req_hdr.seq           = 42;
    req_hdr.payload_bytes = sizeof(hello);

    size_t frame_len = build_frame(g_rx_data, req_hdr,
                                   (const uint8_t *)&hello, sizeof(hello));
    g_rx_data_len = frame_len;
    g_rx_data_pos = 0;

    int frames = al_poll();
    EXPECT_EQ(frames, 1);

    /* Verify HandshakeAck response */
    EXPECT_GT(g_tx_capture_len, 0u);

    al_parser_t rp;
    al_parser_init(&rp);
    size_t consumed = 0;
    int res = al_parser_feed(&rp, g_tx_capture, g_tx_capture_len, &consumed);
    EXPECT_EQ(res, 1);

    const uint8_t *rdata = al_parser_frame_data();
    al_frame_header_t resp_hdr;
    std::memcpy(&resp_hdr, rdata + AL_LEN_SIZE, AL_FRAME_HEADER_SIZE);
    al_frame_header_from_wire(&resp_hdr);

    EXPECT_EQ(resp_hdr.flags, (uint16_t)AL_FRAME_RESPONSE);
    EXPECT_EQ(resp_hdr.service_id, (uint32_t)AL_HANDSHAKE_SERVICE_ID);
    EXPECT_EQ(resp_hdr.message_id, (uint32_t)AL_HANDSHAKE_ACK);
    EXPECT_EQ(resp_hdr.seq, 42u);

    /* Parse the ack payload */
    al_handshake_ack_t ack;
    const uint8_t *ack_data = rdata + AL_LEN_SIZE + AL_FRAME_HEADER_SIZE;
    std::memcpy(&ack, ack_data, sizeof(ack));
    EXPECT_EQ(al_le_to_host16(ack.version), (uint16_t)AL_PROTOCOL_VERSION);
    EXPECT_EQ(al_le_to_host16(ack.status), 0u);
    EXPECT_EQ(al_le_to_host32(ack.max_payload), (uint32_t)AL_MAX_PAYLOAD);
}

/* ── 11. al_is_idle ──────────────────────────────────────────────── */

TEST(AetherLiteTest, IsIdleWhenNoPending)
{
    reset_mock();
    al_hal_t hal = make_hal();
    al_init(&hal);

    EXPECT_TRUE(al_is_idle());

    /* Feed a single sync byte — should no longer be idle */
    uint8_t sync0 = AL_SYNC_BYTE_0;
    g_rx_data[0] = sync0;
    g_rx_data_len = 1;
    g_rx_data_pos = 0;
    al_poll();

    /* Parser saw 0xAA, now in SYNC_1 — not idle */
    EXPECT_FALSE(al_is_idle());
}
