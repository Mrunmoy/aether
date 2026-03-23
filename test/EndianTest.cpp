#include <gtest/gtest.h>
#include <Endian.h>
#include <cstring>

using namespace aether::ipc;

// ── Scalar round-trip ───────────────────────────────────────────────

TEST(EndianTest, RoundTrip16)
{
    uint16_t orig = 0xABCD;
    EXPECT_EQ(leToHost16(hostToLe16(orig)), orig);
}

TEST(EndianTest, RoundTrip32)
{
    uint32_t orig = 0xDEADBEEF;
    EXPECT_EQ(leToHost32(hostToLe32(orig)), orig);
}

// ── Identity on little-endian hosts ─────────────────────────────────

#if !defined(__BYTE_ORDER__) || __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
TEST(EndianTest, IdentityOnLE16)
{
    uint16_t v = 0x1234;
    EXPECT_EQ(hostToLe16(v), v);
    EXPECT_EQ(leToHost16(v), v);
}

TEST(EndianTest, IdentityOnLE32)
{
    uint32_t v = 0x12345678;
    EXPECT_EQ(hostToLe32(v), v);
    EXPECT_EQ(leToHost32(v), v);
}
#endif

// ── FrameHeader wire round-trip ─────────────────────────────────────

TEST(EndianTest, FrameHeaderRoundTrip)
{
    FrameHeader orig{};
    orig.version = 1;
    orig.flags = FRAME_REQUEST;
    orig.serviceId = 0xAABBCCDD;
    orig.messageId = 42;
    orig.seq = 7;
    orig.payloadBytes = 1024;
    orig.aux = 0xFF;

    FrameHeader h = orig;
    frameHeaderToWire(&h);
    frameHeaderFromWire(&h);

    EXPECT_EQ(h.version, orig.version);
    EXPECT_EQ(h.flags, orig.flags);
    EXPECT_EQ(h.serviceId, orig.serviceId);
    EXPECT_EQ(h.messageId, orig.messageId);
    EXPECT_EQ(h.seq, orig.seq);
    EXPECT_EQ(h.payloadBytes, orig.payloadBytes);
    EXPECT_EQ(h.aux, orig.aux);
}
