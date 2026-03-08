#include "FrameIO.h"

namespace ms::ipc
{

    int readFrameAlloc(IpcRing *ring, FrameHeader *header, std::vector<uint8_t> *payload)
    {
        FrameHeader hdr{};
        if (!peekFrameHeader(ring, &hdr))
        {
            return IPC_ERR_DISCONNECTED;
        }

        // Guard against integer overflow: reject payloads that could not have been
        // written by a valid writeFrame() call.
        static_assert(kRingSize > sizeof(FrameHeader), "Ring must be larger than a frame header");
        constexpr uint32_t kMaxPayload = kRingSize - static_cast<uint32_t>(sizeof(FrameHeader));
        if (hdr.payloadBytes > kMaxPayload)
        {
            return IPC_ERR_DISCONNECTED;
        }

        uint32_t totalBytes = static_cast<uint32_t>(sizeof(FrameHeader)) + hdr.payloadBytes;
        if (ring->readAvailable() < totalBytes)
        {
            return IPC_ERR_DISCONNECTED;
        }

        // Consume header
        ring->skip(sizeof(FrameHeader));

        // Read payload
        payload->resize(hdr.payloadBytes);
        if (hdr.payloadBytes > 0)
        {
            ring->read(payload->data(), hdr.payloadBytes);
        }

        *header = hdr;
        return IPC_SUCCESS;
    }

} // namespace ms::ipc
