/*
 * echo_server.c — Minimal Aether IPC server using the C API.
 *
 * Echoes every request payload back to the client unchanged.
 * Build against the SDK:
 *   cc -o echo_server echo_server.c -laether -lstdc++ -lpthread
 */

#include "aether_ipc.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static volatile sig_atomic_t g_running = 1;

static void onSignal(int sig)
{
    (void)sig;
    g_running = 0;
}

static int onRequest(uint32_t service_id, uint32_t message_id,
                     const uint8_t *req, uint32_t req_len,
                     uint8_t **resp, uint32_t *resp_len,
                     void *user_data)
{
    (void)service_id;
    (void)message_id;
    (void)user_data;

    printf("[server] request msg=%u  %u bytes\n", message_id, req_len);

    if (req_len > 0)
    {
        *resp = (uint8_t *)malloc(req_len);
        if (!*resp)
        {
            *resp_len = 0;
            return -1;
        }
        memcpy(*resp, req, req_len);
        *resp_len = req_len;
    }
    else
    {
        *resp = NULL;
        *resp_len = 0;
    }

    return AETHER_SUCCESS;
}

int main(void)
{
    signal(SIGINT, onSignal);
    signal(SIGTERM, onSignal);

    aether_service_t svc = NULL;
    int rc = aether_service_create("c_echo", onRequest, NULL, &svc);
    if (rc != AETHER_SUCCESS)
    {
        fprintf(stderr, "service_create failed: %d\n", rc);
        return 1;
    }

    rc = aether_service_start(svc);
    if (rc != AETHER_SUCCESS)
    {
        fprintf(stderr, "service_start failed: %d\n", rc);
        aether_service_destroy(svc);
        return 1;
    }

    printf("[server] listening on 'c_echo'...\n");
    while (g_running && aether_service_is_running(svc))
    {
        /* main thread idles; request handling is on internal threads */
    }

    printf("[server] shutting down\n");
    aether_service_destroy(svc);
    return 0;
}
