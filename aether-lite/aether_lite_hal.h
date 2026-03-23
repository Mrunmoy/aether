#ifndef AETHER_LITE_HAL_H
#define AETHER_LITE_HAL_H

#include <stdint.h>
#include <stddef.h>

typedef struct al_hal
{
    size_t   (*rx_read)(uint8_t *buf, size_t max_bytes, void *user_ctx);
    int      (*tx_write)(const uint8_t *buf, size_t len, void *user_ctx);
    uint32_t (*get_tick_ms)(void);
    void     *user_ctx;
} al_hal_t;

#endif /* AETHER_LITE_HAL_H */
