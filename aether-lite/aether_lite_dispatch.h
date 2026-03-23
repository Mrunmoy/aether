#ifndef AETHER_LITE_DISPATCH_H
#define AETHER_LITE_DISPATCH_H

#include "aether_lite_types.h"
#include "aether_lite_hal.h"
#include "aether_lite_framing.h"
#include <stdbool.h>
#include <stdint.h>

/* ── Method handler signature ────────────────────────────────────── */

typedef int (*al_method_handler_t)(
    const uint8_t *req, uint32_t req_len,
    uint8_t *resp, uint32_t resp_cap,
    uint32_t *resp_len);

typedef struct
{
    uint32_t message_id;
    al_method_handler_t handler;
} al_method_entry_t;

/* ── Public API ──────────────────────────────────────────────────── */

/*
 * Threading contract
 * ------------------
 * NOT reentrant. All al_* functions must be called from the same
 * execution context (e.g., main loop). Do NOT call from ISR.
 * al_is_idle() may be called from ISR to decide whether to sleep.
 */

void al_init(const al_hal_t *hal);
int  al_poll(void);           /* Returns number of frames processed */
bool al_is_idle(void);        /* True if parser is in SYNC state */

int  al_register_service(uint32_t service_id,
                          const al_method_entry_t *methods,
                          uint8_t method_count);

#if AL_ENABLE_NOTIFICATIONS
int  al_send_notify(uint32_t service_id, uint32_t message_id,
                    const uint8_t *payload, uint32_t payload_len);
#endif

#endif /* AETHER_LITE_DISPATCH_H */
