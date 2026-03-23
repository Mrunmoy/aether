#ifndef AETHER_LITE_FNV1A_H
#define AETHER_LITE_FNV1A_H

#include <stdint.h>

static inline uint32_t al_fnv1a_32(const char *str)
{
    uint32_t hash = 0x811c9dc5u;
    if (!str) return hash;
    while (*str)
    {
        hash ^= (uint8_t)*str++;
        hash *= 0x01000193u;
    }
    return hash;
}

#endif /* AETHER_LITE_FNV1A_H */
