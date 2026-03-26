// Comparison benchmarks: Aether IPC vs common Linux IPC mechanisms.
// Measures request/response round-trip latency with the same payload sizes.

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

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

using namespace aether::ipc;

// ── Aether echo service (reused from bench_roundtrip) ──────────────

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

// ── Aether round-trip ──────────────────────────────────────────────

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

// ── Unix Domain Socket (SOCK_STREAM) round-trip ────────────────────

static void BM_UDS_RoundTrip(benchmark::State &state)
{
    auto payloadSize = static_cast<size_t>(state.range(0));
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
    {
        state.SkipWithError("socketpair failed");
        return;
    }

    // Echo thread: read N bytes, write them back.
    std::atomic<bool> running{true};
    std::thread echoThread([&]() {
        std::vector<uint8_t> buf(payloadSize > 0 ? payloadSize : 1);
        while (running.load(std::memory_order_relaxed))
        {
            ssize_t n = recv(sv[1], buf.data(), buf.size(), MSG_WAITALL);
            if (n <= 0) break;
            ssize_t sent = 0;
            while (sent < n)
            {
                ssize_t w = send(sv[1], buf.data() + sent,
                                 static_cast<size_t>(n - sent), 0);
                if (w <= 0) break;
                sent += w;
            }
        }
    });

    std::vector<uint8_t> request(payloadSize > 0 ? payloadSize : 1, 0xAB);
    std::vector<uint8_t> response(request.size());

    for (auto _ : state)
    {
        send(sv[0], request.data(), request.size(), 0);
        ssize_t total = 0;
        while (total < static_cast<ssize_t>(request.size()))
        {
            ssize_t r = recv(sv[0], response.data() + total,
                             response.size() - static_cast<size_t>(total),
                             MSG_WAITALL);
            if (r <= 0) break;
            total += r;
        }
    }

    running.store(false, std::memory_order_relaxed);
    shutdown(sv[0], SHUT_RDWR);
    echoThread.join();
    close(sv[0]);
    close(sv[1]);
}

// ── Pipe round-trip ────────────────────────────────────────────────

static void BM_Pipe_RoundTrip(benchmark::State &state)
{
    auto payloadSize = static_cast<size_t>(state.range(0));
    // Two pipes: client→server and server→client.
    int toServer[2], toClient[2];
    if (pipe(toServer) < 0 || pipe(toClient) < 0)
    {
        state.SkipWithError("pipe failed");
        return;
    }

    std::atomic<bool> running{true};
    std::thread echoThread([&]() {
        std::vector<uint8_t> buf(payloadSize > 0 ? payloadSize : 1);
        while (running.load(std::memory_order_relaxed))
        {
            ssize_t total = 0;
            while (total < static_cast<ssize_t>(buf.size()))
            {
                ssize_t r = read(toServer[0], buf.data() + total,
                                 buf.size() - static_cast<size_t>(total));
                if (r <= 0) goto done;
                total += r;
            }
            ssize_t sent = 0;
            while (sent < static_cast<ssize_t>(buf.size()))
            {
                ssize_t w = write(toClient[1], buf.data() + sent,
                                  buf.size() - static_cast<size_t>(sent));
                if (w <= 0) goto done;
                sent += w;
            }
        }
        done:;
    });

    std::vector<uint8_t> request(payloadSize > 0 ? payloadSize : 1, 0xAB);
    std::vector<uint8_t> response(request.size());

    for (auto _ : state)
    {
        ssize_t wr = write(toServer[1], request.data(), request.size());
        (void)wr;
        ssize_t total = 0;
        while (total < static_cast<ssize_t>(request.size()))
        {
            ssize_t r = read(toClient[0], response.data() + total,
                             response.size() - static_cast<size_t>(total));
            if (r <= 0) break;
            total += r;
        }
    }

    running.store(false, std::memory_order_relaxed);
    close(toServer[1]);
    close(toClient[0]);
    echoThread.join();
    close(toServer[0]);
    close(toClient[1]);
}

// ── TCP Loopback round-trip ────────────────────────────────────────

static void BM_TCP_RoundTrip(benchmark::State &state)
{
    auto payloadSize = static_cast<size_t>(state.range(0));

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
    addr.sin_port = 0; // kernel picks a free port

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

        std::vector<uint8_t> buf(payloadSize > 0 ? payloadSize : 1);
        while (running.load(std::memory_order_relaxed))
        {
            ssize_t total = 0;
            while (total < static_cast<ssize_t>(buf.size()))
            {
                ssize_t r = recv(clientFd, buf.data() + total,
                                 buf.size() - static_cast<size_t>(total),
                                 MSG_WAITALL);
                if (r <= 0) goto done;
                total += r;
            }
            ssize_t sent = 0;
            while (sent < static_cast<ssize_t>(buf.size()))
            {
                ssize_t w = send(clientFd, buf.data() + sent,
                                 buf.size() - static_cast<size_t>(sent), 0);
                if (w <= 0) goto done;
                sent += w;
            }
        }
        done:
        close(clientFd);
    });

    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    int flag = 1;
    setsockopt(clientFd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    connect(clientFd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));

    std::vector<uint8_t> request(payloadSize > 0 ? payloadSize : 1, 0xAB);
    std::vector<uint8_t> response(request.size());

    for (auto _ : state)
    {
        send(clientFd, request.data(), request.size(), 0);
        ssize_t total = 0;
        while (total < static_cast<ssize_t>(request.size()))
        {
            ssize_t r = recv(clientFd, response.data() + total,
                             response.size() - static_cast<size_t>(total),
                             MSG_WAITALL);
            if (r <= 0) break;
            total += r;
        }
    }

    running.store(false, std::memory_order_relaxed);
    shutdown(clientFd, SHUT_RDWR);
    close(clientFd);
    echoThread.join();
    close(listenFd);
}

// ── Registration ───────────────────────────────────────────────────

BENCHMARK(BM_Aether_RoundTrip)
    ->Arg(64)->Arg(1024)->Arg(16384)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_UDS_RoundTrip)
    ->Arg(64)->Arg(1024)->Arg(16384)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_Pipe_RoundTrip)
    ->Arg(64)->Arg(1024)->Arg(16384)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_TCP_RoundTrip)
    ->Arg(64)->Arg(1024)->Arg(16384)
    ->Unit(benchmark::kMicrosecond);

#endif // !defined(_WIN32)
