#include <gtest/gtest.h>
#include "FrameIO.h"

#include <cstring>
#include <memory>
#include <vector>

using namespace ms::ipc;

// Helper: create a FrameHeader with test values.
static FrameHeader makeHeader(uint16_t flags, uint32_t serviceId, uint32_t messageId, uint32_t seq,
                              uint32_t payloadBytes, uint32_t aux = 0)
{
    FrameHeader hdr{};
    hdr.version = kProtocolVersion;
    hdr.flags = flags;
    hdr.serviceId = serviceId;
    hdr.messageId = messageId;
    hdr.seq = seq;
    hdr.payloadBytes = payloadBytes;
    hdr.aux = aux;
    return hdr;
}

// ═════════════════════════════════════════════════════════════════════
// Write a frame, read it back, verify all fields and payload match
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, WriteAndReadFrame)
{
    auto ring = std::make_unique<IpcRing>();

    const uint8_t payload[] = "Hello, IPC!";
    uint32_t payloadLen = sizeof(payload);
    FrameHeader hdr = makeHeader(FRAME_REQUEST, 1, 2, 42, payloadLen, 99);

    ASSERT_EQ(writeFrame(ring.get(), hdr, payload, payloadLen), IPC_SUCCESS);

    FrameHeader outHdr{};
    uint8_t outPayload[64]{};
    ASSERT_EQ(readFrame(ring.get(), &outHdr, outPayload, sizeof(outPayload)), IPC_SUCCESS);

    EXPECT_EQ(outHdr.version, kProtocolVersion);
    EXPECT_EQ(outHdr.flags, FRAME_REQUEST);
    EXPECT_EQ(outHdr.serviceId, 1u);
    EXPECT_EQ(outHdr.messageId, 2u);
    EXPECT_EQ(outHdr.seq, 42u);
    EXPECT_EQ(outHdr.payloadBytes, payloadLen);
    EXPECT_EQ(outHdr.aux, 99u);
    EXPECT_EQ(std::memcmp(outPayload, payload, payloadLen), 0);
}

// ═════════════════════════════════════════════════════════════════════
// Peek does not consume data from the ring buffer
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, PeekDoesNotConsume)
{
    auto ring = std::make_unique<IpcRing>();

    const uint8_t payload[] = {0xAA, 0xBB, 0xCC};
    FrameHeader hdr = makeHeader(FRAME_RESPONSE, 5, 10, 1, sizeof(payload));

    ASSERT_EQ(writeFrame(ring.get(), hdr, payload, sizeof(payload)), IPC_SUCCESS);

    uint32_t availBefore = ring->readAvailable();

    FrameHeader peeked{};
    ASSERT_TRUE(peekFrameHeader(ring.get(), &peeked));
    EXPECT_EQ(peeked.serviceId, 5u);
    EXPECT_EQ(peeked.payloadBytes, sizeof(payload));

    // readAvailable should be unchanged after peek
    EXPECT_EQ(ring->readAvailable(), availBefore);

    // Full read should still work
    FrameHeader outHdr{};
    uint8_t outPayload[16]{};
    ASSERT_EQ(readFrame(ring.get(), &outHdr, outPayload, sizeof(outPayload)), IPC_SUCCESS);
    EXPECT_EQ(outHdr.seq, 1u);
}

// ═════════════════════════════════════════════════════════════════════
// Ring full returns error without modifying ring state
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, RingFullReturnsError)
{
    auto ring = std::make_unique<IpcRing>();

    // Fill ring to near capacity with a large payload
    uint32_t capacity = IpcRing::capacity();
    std::vector<uint8_t> bigPayload(capacity - sizeof(FrameHeader) - 10);
    FrameHeader hdr = makeHeader(FRAME_REQUEST, 1, 1, 1, static_cast<uint32_t>(bigPayload.size()));

    ASSERT_EQ(
        writeFrame(ring.get(), hdr, bigPayload.data(), static_cast<uint32_t>(bigPayload.size())),
        IPC_SUCCESS);

    uint32_t availAfterFill = ring->writeAvailable();

    // Now try to write another frame that won't fit
    uint8_t smallPayload[64]{};
    FrameHeader hdr2 = makeHeader(FRAME_REQUEST, 2, 2, 2, sizeof(smallPayload));

    EXPECT_EQ(writeFrame(ring.get(), hdr2, smallPayload, sizeof(smallPayload)), IPC_ERR_RING_FULL);

    // Ring state unchanged
    EXPECT_EQ(ring->writeAvailable(), availAfterFill);
}

// ═════════════════════════════════════════════════════════════════════
// Empty ring returns appropriate errors
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, EmptyRingReturnsError)
{
    auto ring = std::make_unique<IpcRing>();

    FrameHeader hdr{};
    EXPECT_FALSE(peekFrameHeader(ring.get(), &hdr));

    uint8_t buf[64]{};
    EXPECT_EQ(readFrame(ring.get(), &hdr, buf, sizeof(buf)), IPC_ERR_DISCONNECTED);
}

// ═════════════════════════════════════════════════════════════════════
// Multiple frames written and read back in order
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, MultipleFrames)
{
    auto ring = std::make_unique<IpcRing>();

    for (uint32_t i = 0; i < 3; ++i)
    {
        uint8_t payload[4];
        std::memcpy(payload, &i, sizeof(i));
        FrameHeader hdr = makeHeader(FRAME_REQUEST, i, i * 10, i, sizeof(payload));
        ASSERT_EQ(writeFrame(ring.get(), hdr, payload, sizeof(payload)), IPC_SUCCESS);
    }

    for (uint32_t i = 0; i < 3; ++i)
    {
        FrameHeader outHdr{};
        uint8_t outPayload[4]{};
        ASSERT_EQ(readFrame(ring.get(), &outHdr, outPayload, sizeof(outPayload)), IPC_SUCCESS);

        EXPECT_EQ(outHdr.serviceId, i);
        EXPECT_EQ(outHdr.messageId, i * 10);
        EXPECT_EQ(outHdr.seq, i);

        uint32_t val = 0;
        std::memcpy(&val, outPayload, sizeof(val));
        EXPECT_EQ(val, i);
    }

    // Ring should be empty now
    EXPECT_TRUE(ring->isEmpty());
}

// ═════════════════════════════════════════════════════════════════════
// Zero-length payload
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, ZeroPayload)
{
    auto ring = std::make_unique<IpcRing>();

    FrameHeader hdr = makeHeader(FRAME_NOTIFY, 7, 77, 100, 0);
    ASSERT_EQ(writeFrame(ring.get(), hdr, nullptr, 0), IPC_SUCCESS);

    FrameHeader outHdr{};
    uint8_t dummy[1]{};
    ASSERT_EQ(readFrame(ring.get(), &outHdr, dummy, 0), IPC_SUCCESS);

    EXPECT_EQ(outHdr.flags, FRAME_NOTIFY);
    EXPECT_EQ(outHdr.serviceId, 7u);
    EXPECT_EQ(outHdr.messageId, 77u);
    EXPECT_EQ(outHdr.seq, 100u);
    EXPECT_EQ(outHdr.payloadBytes, 0u);
}

// ═════════════════════════════════════════════════════════════════════
// readFrameAlloc — vector-based convenience read
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, ReadFrameAlloc)
{
    auto ring = std::make_unique<IpcRing>();

    const uint8_t payload[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE};
    FrameHeader hdr = makeHeader(FRAME_RESPONSE, 3, 30, 5, sizeof(payload), 200);

    ASSERT_EQ(writeFrame(ring.get(), hdr, payload, sizeof(payload)), IPC_SUCCESS);

    FrameHeader outHdr{};
    std::vector<uint8_t> outPayload;
    ASSERT_EQ(readFrameAlloc(ring.get(), &outHdr, &outPayload), IPC_SUCCESS);

    EXPECT_EQ(outHdr.serviceId, 3u);
    EXPECT_EQ(outHdr.aux, 200u);
    ASSERT_EQ(outPayload.size(), sizeof(payload));
    EXPECT_EQ(std::memcmp(outPayload.data(), payload, sizeof(payload)), 0);
}

// ═════════════════════════════════════════════════════════════════════
// Large payload near ring capacity
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, LargePayload)
{
    auto ring = std::make_unique<IpcRing>();

    // Use ~200KB payload (ring is 256KB)
    constexpr uint32_t payloadSize = 200 * 1024;
    std::vector<uint8_t> payload(payloadSize);
    for (uint32_t i = 0; i < payloadSize; ++i)
    {
        payload[i] = static_cast<uint8_t>(i & 0xFF);
    }

    FrameHeader hdr = makeHeader(FRAME_REQUEST, 1, 1, 1, payloadSize);
    ASSERT_EQ(writeFrame(ring.get(), hdr, payload.data(), payloadSize), IPC_SUCCESS);

    FrameHeader outHdr{};
    std::vector<uint8_t> outPayload;
    ASSERT_EQ(readFrameAlloc(ring.get(), &outHdr, &outPayload), IPC_SUCCESS);

    ASSERT_EQ(outPayload.size(), payloadSize);
    EXPECT_EQ(std::memcmp(outPayload.data(), payload.data(), payloadSize), 0);
}

// ═════════════════════════════════════════════════════════════════════
// Payload buffer too small for readFrame
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, PayloadTooLargeForBuffer)
{
    auto ring = std::make_unique<IpcRing>();

    const uint8_t payload[] = "This payload is larger than the read buffer";
    FrameHeader hdr = makeHeader(FRAME_REQUEST, 1, 1, 1, sizeof(payload));
    ASSERT_EQ(writeFrame(ring.get(), hdr, payload, sizeof(payload)), IPC_SUCCESS);

    FrameHeader outHdr{};
    uint8_t smallBuf[4]{};
    // Buffer is too small for the payload
    EXPECT_EQ(readFrame(ring.get(), &outHdr, smallBuf, sizeof(smallBuf)), IPC_ERR_RING_FULL);

    // Frame should still be in the ring (not consumed)
    EXPECT_FALSE(ring->isEmpty());

    // Read with adequate buffer should succeed
    uint8_t bigBuf[128]{};
    ASSERT_EQ(readFrame(ring.get(), &outHdr, bigBuf, sizeof(bigBuf)), IPC_SUCCESS);
    EXPECT_EQ(std::memcmp(bigBuf, payload, sizeof(payload)), 0);
}

// ═════════════════════════════════════════════════════════════════════
// Oversized payloadBytes in header (integer overflow guard)
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, OversizedPayloadBytesRejected)
{
    auto ring = std::make_unique<IpcRing>();

    // Craft a FrameHeader whose payloadBytes exceeds the ring capacity.
    // This simulates a corrupted or malicious header that could cause
    // integer overflow in the totalBytes computation.
    FrameHeader hdr = makeHeader(FRAME_REQUEST, 1, 1, 1, 0);
    hdr.payloadBytes = kRingSize; // exactly equal to ring size — no room for header

    ring->write(reinterpret_cast<const uint8_t *>(&hdr), sizeof(FrameHeader));

    // Both readFrame and readFrameAlloc should reject the oversized payload.
    FrameHeader outHdr{};
    uint8_t buf[64]{};
    EXPECT_EQ(readFrame(ring.get(), &outHdr, buf, sizeof(buf)), IPC_ERR_DISCONNECTED);

    // Ring was consumed by the skip-less peek, so reset
    auto ring2 = std::make_unique<IpcRing>();
    ring2->write(reinterpret_cast<const uint8_t *>(&hdr), sizeof(FrameHeader));

    std::vector<uint8_t> payload;
    EXPECT_EQ(readFrameAlloc(ring2.get(), &outHdr, &payload), IPC_ERR_DISCONNECTED);
}

// ═════════════════════════════════════════════════════════════════════
// writeFrame rejects payloads that would overflow the frame size
// ═════════════════════════════════════════════════════════════════════

TEST(FrameIOTest, WriteFrameOversizedPayloadRejected)
{
    auto ring = std::make_unique<IpcRing>();

    // payloadBytes > kRingSize - sizeof(FrameHeader) must be rejected.
    constexpr uint32_t kMaxPayload = kRingSize - static_cast<uint32_t>(sizeof(FrameHeader));
    std::vector<uint8_t> bigPayload(kMaxPayload + 1, 0xAB);

    FrameHeader hdr = makeHeader(FRAME_REQUEST, 1, 1, 1, static_cast<uint32_t>(bigPayload.size()));
    EXPECT_EQ(writeFrame(ring.get(), hdr, bigPayload.data(), static_cast<uint32_t>(bigPayload.size())),
              IPC_ERR_RING_FULL);

    // Ring should be untouched.
    EXPECT_TRUE(ring->isEmpty());
}
