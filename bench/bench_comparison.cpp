// Comparison benchmarks: Aether IPC vs common IPC mechanisms.
// Measures request/response round-trip latency with the same payload sizes.
//
// The raw transport benchmarks include length-prefixed framing (4-byte
// header) to approximate what any real protocol would need.  Aether
// additionally performs service dispatch, sequence correlation, and
// wakeup signalling, so its overhead reflects the cost of a complete
// IPC framework -- not just raw byte transfer.

#if !defined(_WIN32)

#include "ClientBase.h"
#include "ServiceBase.h"
#include "Types.h"

#include <benchmark/benchmark.h>

#include <atomic>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using namespace aether::ipc;

// -- helpers -----------------------------------------------------------------

static inline bool sendAll(int fd, const void *buf, size_t len)
{
    auto p = static_cast<const uint8_t *>(buf);
    while (len > 0)
    {
        ssize_t w = send(fd, p, len, 0);
        if (w <= 0)
            return false;
        p += w;
        len -= static_cast<size_t>(w);
    }
    return true;
}

static inline bool recvAll(int fd, void *buf, size_t len)
{
    auto p = static_cast<uint8_t *>(buf);
    while (len > 0)
    {
        ssize_t r = recv(fd, p, len, MSG_WAITALL);
        if (r <= 0)
            return false;
        p += r;
        len -= static_cast<size_t>(r);
    }
    return true;
}

static inline bool writeAll(int fd, const void *buf, size_t len)
{
    auto p = static_cast<const uint8_t *>(buf);
    while (len > 0)
    {
        ssize_t w = write(fd, p, len);
        if (w <= 0)
            return false;
        p += w;
        len -= static_cast<size_t>(w);
    }
    return true;
}

static inline bool readAll(int fd, void *buf, size_t len)
{
    auto p = static_cast<uint8_t *>(buf);
    while (len > 0)
    {
        ssize_t r = read(fd, p, len);
        if (r <= 0)
            return false;
        p += r;
        len -= static_cast<size_t>(r);
    }
    return true;
}

// Length-prefixed framing: [uint32_t len][payload]
static inline bool sendFramed(int fd, const uint8_t *data, uint32_t len,
                              bool useSend = true)
{
    uint32_t hdr = len;
    if (useSend)
        return sendAll(fd, &hdr, sizeof(hdr)) && sendAll(fd, data, len);
    return writeAll(fd, &hdr, sizeof(hdr)) && writeAll(fd, data, len);
}

static inline bool recvFramed(int fd, std::vector<uint8_t> &buf,
                              bool useSend = true)
{
    uint32_t hdr = 0;
    if (useSend)
    {
        if (!recvAll(fd, &hdr, sizeof(hdr)))
            return false;
        buf.resize(hdr);
        return recvAll(fd, buf.data(), hdr);
    }
    if (!readAll(fd, &hdr, sizeof(hdr)))
        return false;
    buf.resize(hdr);
    return readAll(fd, buf.data(), hdr);
}

// -- Aether echo service -----------------------------------------------------

class CompEchoService : public ServiceBase
{
public:
    explicit CompEchoService(const char *name) : ServiceBase(name) {}

protected:
    int onRequest(uint32_t, const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override
    {
        *response = request;
        return IPC_SUCCESS;
    }
};

static std::atomic<int> s_counter{0};

static std::string uniqueName()
{
    return "bench_cmp_" + std::to_string(s_counter.fetch_add(1));
}

// -- Aether round-trip -------------------------------------------------------

static void BM_Aether_RoundTrip(benchmark::State &state)
{
    auto payloadSize = static_cast<uint32_t>(state.range(0));
    std::string name = uniqueName();

    CompEchoService svc(name.c_str());
    svc.start();

    ClientBase client(name.c_str());
    client.connect();

    std::vector<uint8_t> request(payloadSize, 0xAB);
    std::vector<uint8_t> response;

    for (auto _ : state)
    {
        client.call(1, 1, request, &response, 2000);
    }

    client.disconnect();
    svc.stop();
}

// -- Unix Domain Socket (SOCK_STREAM) round-trip -----------------------------

static void BM_UDS_RoundTrip(benchmark::State &state)
{
    auto payloadSize = static_cast<uint32_t>(state.range(0));
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
    {
        state.SkipWithError("socketpair failed");
        return;
    }

    std::atomic<bool> running{true};
    std::thread echoThread([&]() {
        std::vector<uint8_t> buf;
        while (running.load(std::memory_order_relaxed))
        {
            if (!recvFramed(sv[1], buf))
                break;
            if (!sendFramed(sv[1], buf.data(), static_cast<uint32_t>(buf.size())))
                break;
        }
    });

    std::vector<uint8_t> request(payloadSize, 0xAB);
    std::vector<uint8_t> response;

    for (auto _ : state)
    {
        sendFramed(sv[0], request.data(), payloadSize);
        recvFramed(sv[0], response);
    }

    running.store(false, std::memory_order_relaxed);
    shutdown(sv[0], SHUT_RDWR);
    echoThread.join();
    close(sv[0]);
    close(sv[1]);
}

// -- Pipe round-trip ---------------------------------------------------------

static void BM_Pipe_RoundTrip(benchmark::State &state)
{
    auto payloadSize = static_cast<uint32_t>(state.range(0));
    int toServer[2], toClient[2];
    if (pipe(toServer) < 0 || pipe(toClient) < 0)
    {
        state.SkipWithError("pipe failed");
        return;
    }

    std::atomic<bool> running{true};
    std::thread echoThread([&]() {
        std::vector<uint8_t> buf;
        while (running.load(std::memory_order_relaxed))
        {
            if (!recvFramed(toServer[0], buf, false))
                break;
            if (!sendFramed(toClient[1], buf.data(),
                            static_cast<uint32_t>(buf.size()), false))
                break;
        }
    });

    std::vector<uint8_t> request(payloadSize, 0xAB);
    std::vector<uint8_t> response;

    for (auto _ : state)
    {
        sendFramed(toServer[1], request.data(), payloadSize, false);
        recvFramed(toClient[0], response, false);
    }

    running.store(false, std::memory_order_relaxed);
    close(toServer[1]);
    close(toClient[0]);
    echoThread.join();
    close(toServer[0]);
    close(toClient[1]);
}

// -- TCP Loopback round-trip -------------------------------------------------

static void BM_TCP_RoundTrip(benchmark::State &state)
{
    auto payloadSize = static_cast<uint32_t>(state.range(0));

    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
    {
        state.SkipWithError("socket failed");
        return;
    }

    int opt = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(listenFd, reinterpret_cast<struct sockaddr *>(&addr),
             sizeof(addr)) < 0)
    {
        close(listenFd);
        state.SkipWithError("bind failed");
        return;
    }

    socklen_t addrLen = sizeof(addr);
    getsockname(listenFd, reinterpret_cast<struct sockaddr *>(&addr), &addrLen);
    listen(listenFd, 1);

    std::atomic<bool> running{true};
    std::thread echoThread([&]() {
        int clientFd = accept(listenFd, nullptr, nullptr);
        if (clientFd < 0) return;
        int flag = 1;
        setsockopt(clientFd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

        std::vector<uint8_t> buf;
        while (running.load(std::memory_order_relaxed))
        {
            if (!recvFramed(clientFd, buf))
                break;
            if (!sendFramed(clientFd, buf.data(),
                            static_cast<uint32_t>(buf.size())))
                break;
        }
        close(clientFd);
    });

    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    int flag = 1;
    setsockopt(clientFd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    connect(clientFd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));

    std::vector<uint8_t> request(payloadSize, 0xAB);
    std::vector<uint8_t> response;

    for (auto _ : state)
    {
        sendFramed(clientFd, request.data(), payloadSize);
        recvFramed(clientFd, response);
    }

    running.store(false, std::memory_order_relaxed);
    shutdown(clientFd, SHUT_RDWR);
    close(clientFd);
    echoThread.join();
    close(listenFd);
}

// -- Registration ------------------------------------------------------------

BENCHMARK(BM_Aether_RoundTrip)
    ->Arg(64)->Arg(1024)->Arg(16384)->Arg(65536)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_UDS_RoundTrip)
    ->Arg(64)->Arg(1024)->Arg(16384)->Arg(65536)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Pipe_RoundTrip)
    ->Arg(64)->Arg(1024)->Arg(16384)->Arg(65536)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_TCP_RoundTrip)
    ->Arg(64)->Arg(1024)->Arg(16384)->Arg(65536)
    ->Unit(benchmark::kMicrosecond);

#endif // !defined(_WIN32)
