#pragma once

#include "Types.h"
#include <cstdint>
#include <cstring>
#include <vector>

namespace ms::ipc
{

    // ── Frame I/O ───────────────────────────────────────────────────
    //
    // Read and write framed messages (FrameHeader + payload) through
    // ring buffers. No endian conversion — same-machine IPC only.

    // Maximum payload that can fit in a single frame (ring capacity minus header).
    static_assert(kRingSize > sizeof(FrameHeader), "Ring must be larger than a frame header");
    constexpr uint32_t kMaxPayload = kRingSize - static_cast<uint32_t>(sizeof(FrameHeader));

    // Write a complete frame (header + payload) to a ring buffer.
    // The write is atomic: either the entire frame is written, or
    // nothing is written if insufficient space.
    // Returns IPC_SUCCESS or IPC_ERR_RING_FULL.
    inline int writeFrame(IpcRing *ring, const FrameHeader &header, const uint8_t *payload,
                          uint32_t payloadBytes)
    {
        // Guard against integer overflow in the totalBytes computation.
        if (payloadBytes > kMaxPayload)
        {
            return IPC_ERR_RING_FULL;
        }

        uint32_t totalBytes = static_cast<uint32_t>(sizeof(FrameHeader)) + payloadBytes;
        if (ring->writeAvailable() < totalBytes)
        {
            return IPC_ERR_RING_FULL;
        }

        ring->write(reinterpret_cast<const uint8_t *>(&header), sizeof(FrameHeader));

        if (payloadBytes > 0 && payload != nullptr)
        {
            ring->write(payload, payloadBytes);
        }

        return IPC_SUCCESS;
    }

    // Peek at the next frame header without consuming it.
    // Returns true if a complete header is available, false otherwise.
    inline bool peekFrameHeader(const IpcRing *ring, FrameHeader *header)
    {
        if (ring->readAvailable() < sizeof(FrameHeader))
        {
            return false;
        }

        return ring->peek(reinterpret_cast<uint8_t *>(header), sizeof(FrameHeader));
    }

    // Read a complete frame (header + payload) from a ring buffer.
    // Caller provides a payload buffer of at least the frame's payloadBytes.
    //
    // Usage:
    //   1. peekFrameHeader() to learn payloadBytes
    //   2. Allocate or select a buffer >= payloadBytes
    //   3. readFrame() to consume header + payload
    //
    // Returns IPC_SUCCESS, or IPC_ERR_DISCONNECTED if not enough data,
    // or IPC_ERR_RING_FULL if payload buffer is too small.
    inline int readFrame(IpcRing *ring, FrameHeader *header, uint8_t *payload,
                         uint32_t payloadBufSize)
    {
        FrameHeader hdr{};
        if (!peekFrameHeader(ring, &hdr))
        {
            return IPC_ERR_DISCONNECTED;
        }

        // Guard against integer overflow in the totalBytes computation.
        if (hdr.payloadBytes > kMaxPayload)
        {
            return IPC_ERR_DISCONNECTED;
        }

        uint32_t totalBytes = static_cast<uint32_t>(sizeof(FrameHeader)) + hdr.payloadBytes;
        if (ring->readAvailable() < totalBytes)
        {
            return IPC_ERR_DISCONNECTED;
        }

        if (hdr.payloadBytes > payloadBufSize)
        {
            return IPC_ERR_RING_FULL;
        }

        // Consume header
        ring->skip(sizeof(FrameHeader));

        // Read payload
        if (hdr.payloadBytes > 0)
        {
            ring->read(payload, hdr.payloadBytes);
        }

        *header = hdr;
        return IPC_SUCCESS;
    }

    // Read a complete frame with heap-allocated payload.
    // Convenience wrapper that resizes the vector to fit.
    // Returns IPC_SUCCESS or IPC_ERR_DISCONNECTED.
    int readFrameAlloc(IpcRing *ring, FrameHeader *header, std::vector<uint8_t> *payload);

} // namespace ms::ipc
