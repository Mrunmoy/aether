/*
 * echo_client.c — Minimal Aether IPC client using the C API.
 *
 * Sends a few messages to the echo server and prints the replies.
 * Build against the SDK:
 *   cc -o echo_client echo_client.c -laether -lstdc++ -lpthread
 */

#include "aether_ipc.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    aether_client_t cli = NULL;
    int rc = aether_client_create("c_echo", NULL, NULL, &cli);
    if (rc != AETHER_SUCCESS)
    {
        fprintf(stderr, "client_create failed: %d\n", rc);
        return 1;
    }

    rc = aether_client_connect(cli);
    if (rc != AETHER_SUCCESS)
    {
        fprintf(stderr, "client_connect failed: %d\n", rc);
        aether_client_destroy(cli);
        return 1;
    }

    printf("[client] connected to 'c_echo'\n");

    const char *messages[] = {"Hello", "Aether", "IPC"};
    for (int i = 0; i < 3; ++i)
    {
        const uint8_t *req = (const uint8_t *)messages[i];
        uint32_t req_len = (uint32_t)strlen(messages[i]);

        uint8_t resp[256] = {0};
        uint32_t resp_len = 0;

        rc = aether_client_call(cli, 0, 1, req, req_len,
                                resp, sizeof(resp), &resp_len, 2000);
        if (rc == AETHER_SUCCESS)
        {
            printf("[client] sent '%s' → got '%.*s'\n",
                   messages[i], (int)resp_len, (const char *)resp);
        }
        else
        {
            fprintf(stderr, "[client] call failed: %d\n", rc);
        }
    }

    aether_client_destroy(cli);
    return 0;
}
