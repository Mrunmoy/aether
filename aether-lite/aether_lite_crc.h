#ifndef AETHER_LITE_CRC_H
#define AETHER_LITE_CRC_H

#include <stdint.h>
#include <stddef.h>

/* IEEE 802.3 CRC-32 (polynomial 0x04C11DB7, reflected,
 * init 0xFFFFFFFF, final XOR 0xFFFFFFFF).
 * Test vector: "123456789" -> 0xCBF43926 */

uint32_t al_crc32(const uint8_t *data, size_t len);
uint32_t al_crc32_update(uint32_t crc, const uint8_t *data, size_t len);

#endif /* AETHER_LITE_CRC_H */
