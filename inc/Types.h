#pragma once

#include <cstdint>
#include <spsc/RingBuffer.h>

namespace aether::ipc
{

    // ── Protocol ────────────────────────────────────────────────────

    constexpr uint16_t kProtocolVersion = 1;
    constexpr uint32_t kRingSize = 256 * 1024; // 256KB per direction

    // Ring buffer type used for IPC data plane.
    using IpcRing = ouroboros::spsc::ByteRingBuffer<kRingSize>;

    // ── Error Codes ─────────────────────────────────────────────────
    // Negative = framework, 0 = success, positive = user-defined.

    enum IpcError : int
    {
        IPC_SUCCESS = 0,
        IPC_ERR_DISCONNECTED = -1,
        IPC_ERR_TIMEOUT = -2,
        IPC_ERR_INVALID_SERVICE = -3,
        IPC_ERR_INVALID_METHOD = -4,
        IPC_ERR_VERSION_MISMATCH = -5,
        IPC_ERR_RING_FULL = -6,
        IPC_ERR_STOPPED = -7,
        IPC_ERR_INVALID_ARGUMENT = -8,
    };

    // ── Frame Header (24 bytes) ─────────────────────────────────────
    // Native endian (same-machine IPC via shared memory).

    struct FrameHeader
    {
        uint16_t version;
        uint16_t flags;
        uint32_t serviceId;
        uint32_t messageId;
        uint32_t seq;
        uint32_t payloadBytes;
        uint32_t aux;
    };

    static_assert(sizeof(FrameHeader) == 24, "FrameHeader must be 24 bytes");

    // ── Frame Flags ─────────────────────────────────────────────────

    enum FrameFlags : uint16_t
    {
        FRAME_REQUEST = 0x0001,
        FRAME_RESPONSE = 0x0002,
        FRAME_NOTIFY = 0x0004,
    };

} // namespace aether::ipc
