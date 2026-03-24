// Minimal IPC echo server using ServiceBase.
//
// Run:   ./echo_server
// Stop:  Ctrl-C

#include "ServiceBase.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <thread>
#if !defined(_WIN32)
#include <unistd.h>
#endif

static std::atomic<bool> g_running{true};

class EchoService : public aether::ipc::ServiceBase
{
public:
    using ServiceBase::ServiceBase;

protected:
    int onRequest(uint32_t messageId, const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override
    {
        std::printf("[server] request: messageId=%u, %zu bytes\n", messageId, request.size());
        *response = request; // echo back
        return aether::ipc::IPC_SUCCESS;
    }
};

int main()
{
#if !defined(_WIN32)
    std::signal(SIGINT, [](int) { g_running.store(false); });
#endif

    EchoService service("echo");
    if (!service.start())
    {
        std::fprintf(stderr, "Failed to start service\n");
        return 1;
    }

    std::printf("Echo server running (Ctrl-C to stop)...\n");
    while (g_running.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    service.stop();
    std::printf("Server stopped.\n");
    return 0;
}
