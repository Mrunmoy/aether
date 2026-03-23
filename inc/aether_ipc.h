/*
 * aether_ipc.h — Stable C API for the Aether IPC library.
 *
 * This is the only public header for SDK consumers.  All C++ internals
 * (ServiceBase, ClientBase, rings, frames) are hidden behind opaque handles.
 * Link against libaether.a (static) or libaether.so (shared).
 *
 * Thread safety
 * -------------
 *   - aether_service_* functions are thread-safe after start().
 *   - aether_client_call() is thread-safe (serialized internally).
 *   - Callbacks are invoked on internal receiver threads; keep them short.
 */

#ifndef AETHER_IPC_H
#define AETHER_IPC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ── Error codes (match aether::ipc::IpcError) ─────────────────────── */

#define AETHER_SUCCESS               0
#define AETHER_ERR_DISCONNECTED     -1
#define AETHER_ERR_TIMEOUT          -2
#define AETHER_ERR_INVALID_SERVICE  -3
#define AETHER_ERR_INVALID_METHOD   -4
#define AETHER_ERR_VERSION_MISMATCH -5
#define AETHER_ERR_RING_FULL        -6
#define AETHER_ERR_STOPPED          -7
#define AETHER_ERR_INVALID_ARGUMENT -8
#define AETHER_ERR_TRANSPORT        -9
#define AETHER_ERR_CRC             -10
#define AETHER_ERR_NOT_SUPPORTED   -11
#define AETHER_ERR_NO_SPACE        -12
#define AETHER_ERR_OVERFLOW        -13

/* ── Opaque handles ────────────────────────────────────────────────── */

typedef struct aether_service_impl *aether_service_t;
typedef struct aether_client_impl  *aether_client_t;

/* ── Server callbacks ──────────────────────────────────────────────── */

/*
 * Called for each incoming request.  Write the response into *resp / *resp_len.
 * The library allocates the response buffer with malloc(); the caller frees it
 * with aether_free().
 *
 * Return AETHER_SUCCESS (0) or a positive/negative application error code.
 * The return value is sent to the client as the response status.
 */
typedef int (*aether_request_cb)(uint32_t service_id,
                                 uint32_t message_id,
                                 const uint8_t *req, uint32_t req_len,
                                 uint8_t **resp, uint32_t *resp_len,
                                 void *user_data);

/* Called when a client connects.  Optional (may be NULL). */
typedef void (*aether_client_connected_cb)(void *user_data);

/* Called when a client disconnects.  Optional (may be NULL). */
typedef void (*aether_client_disconnected_cb)(void *user_data);

/* ── Server API ────────────────────────────────────────────────────── */

int  aether_service_create(const char *name,
                           aether_request_cb on_request,
                           void *user_data,
                           aether_service_t *out);

int  aether_service_start(aether_service_t svc);
void aether_service_stop(aether_service_t svc);
int  aether_service_is_running(aether_service_t svc);
void aether_service_destroy(aether_service_t svc);

/* Broadcast a notification to all connected clients. */
int  aether_service_notify(aether_service_t svc,
                           uint32_t service_id,
                           uint32_t message_id,
                           const uint8_t *payload, uint32_t len);

/* ── Client callbacks ──────────────────────────────────────────────── */

/*
 * Called for each incoming notification from the server.
 * Invoked on the internal receiver thread; keep it short.
 */
typedef void (*aether_notify_cb)(uint32_t service_id,
                                 uint32_t message_id,
                                 const uint8_t *payload, uint32_t len,
                                 void *user_data);

/* ── Client API ────────────────────────────────────────────────────── */

int  aether_client_create(const char *name,
                          aether_notify_cb on_notify,
                          void *user_data,
                          aether_client_t *out);

int  aether_client_connect(aether_client_t cli);
void aether_client_disconnect(aether_client_t cli);
int  aether_client_is_connected(aether_client_t cli);
void aether_client_destroy(aether_client_t cli);

/*
 * Synchronous RPC.  Blocks until a response arrives or timeout_ms expires.
 *
 * resp       — caller-provided buffer for the response payload.
 * resp_cap   — capacity of resp in bytes.
 * resp_len   — on success, set to actual response length.
 * timeout_ms — 0 means use the library default (2000 ms).
 *
 * Returns AETHER_SUCCESS or an error code.
 */
int  aether_client_call(aether_client_t cli,
                        uint32_t service_id,
                        uint32_t message_id,
                        const uint8_t *req, uint32_t req_len,
                        uint8_t *resp, uint32_t resp_cap,
                        uint32_t *resp_len,
                        uint32_t timeout_ms);

/* ── Memory ────────────────────────────────────────────────────────── */

/* Free a buffer allocated by the library (e.g., response from aether_request_cb). */
void aether_free(void *ptr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AETHER_IPC_H */
