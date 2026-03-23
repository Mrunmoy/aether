#pragma once

#include "Types.h"
#include <cassert>
#include <cstdint>

namespace aether::ipc
{

    // Compile-time endian detection
    #if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    inline uint16_t leToHost16(uint16_t v) { return __builtin_bswap16(v); }
    inline uint32_t leToHost32(uint32_t v) { return __builtin_bswap32(v); }
    inline uint16_t hostToLe16(uint16_t v) { return __builtin_bswap16(v); }
    inline uint32_t hostToLe32(uint32_t v) { return __builtin_bswap32(v); }
    #else
    inline uint16_t leToHost16(uint16_t v) { return v; }
    inline uint32_t leToHost32(uint32_t v) { return v; }
    inline uint16_t hostToLe16(uint16_t v) { return v; }
    inline uint32_t hostToLe32(uint32_t v) { return v; }
    #endif

    // Convert FrameHeader between host byte order and wire (LE) byte order.
    inline void frameHeaderToWire(FrameHeader *h)
    {
        assert(h != nullptr);
        h->version = hostToLe16(h->version);
        h->flags = hostToLe16(h->flags);
        h->serviceId = hostToLe32(h->serviceId);
        h->messageId = hostToLe32(h->messageId);
        h->seq = hostToLe32(h->seq);
        h->payloadBytes = hostToLe32(h->payloadBytes);
        h->aux = hostToLe32(h->aux);
    }

    inline void frameHeaderFromWire(FrameHeader *h)
    {
        assert(h != nullptr);
        h->version = leToHost16(h->version);
        h->flags = leToHost16(h->flags);
        h->serviceId = leToHost32(h->serviceId);
        h->messageId = leToHost32(h->messageId);
        h->seq = leToHost32(h->seq);
        h->payloadBytes = leToHost32(h->payloadBytes);
        h->aux = leToHost32(h->aux);
    }

} // namespace aether::ipc
