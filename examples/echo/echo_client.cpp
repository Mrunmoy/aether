// Minimal IPC echo client using ClientBase.
//
// Run:   ./echo_server   (in another terminal)
//        ./echo_client

#include "ClientBase.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

int main()
{
    ms::ipc::ClientBase client("echo");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to echo service\n");
        return 1;
    }
    std::printf("Connected to echo service.\n");

    // Send a few echo requests.
    const char *messages[] = {"Hello", "World", "ms-ipc works!"};
    for (const char *msg : messages)
    {
        std::vector<uint8_t> request(msg, msg + std::strlen(msg));
        std::vector<uint8_t> response;

        int rc = client.call(1, 1, request, &response);
        if (rc == ms::ipc::IPC_SUCCESS)
        {
            std::string reply(response.begin(), response.end());
            std::printf("[client] sent: \"%s\"  ->  received: \"%s\"\n", msg, reply.c_str());
        }
        else
        {
            std::fprintf(stderr, "[client] call failed: %d\n", rc);
        }
    }

    client.disconnect();
    std::printf("Disconnected.\n");
    return 0;
}
