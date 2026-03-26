// Full IPC round-trip latency and throughput benchmarks.

#include "ClientBase.h"
#include "ServiceBase.h"
#include "Types.h"

#include <benchmark/benchmark.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

using namespace aether::ipc;

// ── Minimal echo service ────────────────────────────────────────────

class BenchEchoService : public ServiceBase
{
public:
    explicit BenchEchoService(const char *name) : ServiceBase(name) {}

protected:
    int onRequest(uint32_t, const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override
    {
        *response = request;
        return IPC_SUCCESS;
    }
};

static std::atomic<int> s_nameCounter{0};

static std::string uniqueName()
{
    return "bench_rt_" + std::to_string(s_nameCounter.fetch_add(1));
}

// ── Call latency (varying payload) ──────────────────────────────────

static void BM_CallLatency(benchmark::State &state)
{
    auto payloadSize = static_cast<uint32_t>(state.range(0));
    std::string name = uniqueName();

    BenchEchoService svc(name.c_str());
    svc.start();

    ClientBase client(name.c_str());
    client.connect();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::vector<uint8_t> request(payloadSize, 0xAA);
    std::vector<uint8_t> response;

    for (auto _ : state)
    {
        response.clear();
        int rc = client.call(0xDEADBEEF, 1, request, &response, 5000);
        benchmark::DoNotOptimize(rc);
    }

    state.SetBytesProcessed(
        state.iterations() * static_cast<int64_t>(payloadSize) * 2);

    client.disconnect();
    svc.stop();
}

BENCHMARK(BM_CallLatency)->Arg(0)->Arg(64)->Arg(1024)->Arg(16384)
    ->Unit(benchmark::kMicrosecond);

// ── Call throughput (fixed payload, measure rate) ────────────────────

static void BM_CallThroughput(benchmark::State &state)
{
    std::string name = uniqueName();

    BenchEchoService svc(name.c_str());
    svc.start();

    ClientBase client(name.c_str());
    client.connect();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::vector<uint8_t> request(64, 0xBB);
    std::vector<uint8_t> response;

    for (auto _ : state)
    {
        response.clear();
        int rc = client.call(0xDEADBEEF, 1, request, &response, 5000);
        benchmark::DoNotOptimize(rc);
    }

    state.counters["calls/s"] = benchmark::Counter(
        static_cast<double>(state.iterations()),
        benchmark::Counter::kIsRate);

    client.disconnect();
    svc.stop();
}

BENCHMARK(BM_CallThroughput)->Unit(benchmark::kMicrosecond);

// ── Connect + disconnect cycle ──────────────────────────────────────

static void BM_ConnectDisconnect(benchmark::State &state)
{
    std::string name = uniqueName();

    BenchEchoService svc(name.c_str());
    svc.start();

    for (auto _ : state)
    {
        ClientBase client(name.c_str());
        client.connect();
        client.disconnect();
    }

    svc.stop();
}

BENCHMARK(BM_ConnectDisconnect)->Unit(benchmark::kMicrosecond);

// ── Parallel clients ────────────────────────────────────────────────

static void BM_ParallelClients(benchmark::State &state)
{
    auto numClients = static_cast<int>(state.range(0));
    std::string name = uniqueName();

    BenchEchoService svc(name.c_str());
    svc.start();

    std::vector<std::unique_ptr<ClientBase>> clients;
    for (int i = 0; i < numClients; ++i)
    {
        auto c = std::make_unique<ClientBase>(name.c_str());
        c->connect();
        clients.push_back(std::move(c));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    std::vector<uint8_t> request(64, 0xCC);

    for (auto _ : state)
    {
        for (auto &c : clients)
        {
            std::vector<uint8_t> response;
            int rc = c->call(0xDEADBEEF, 1, request, &response, 5000);
            benchmark::DoNotOptimize(rc);
        }
    }

    state.SetItemsProcessed(state.iterations() * numClients);
    state.counters["clients"] = static_cast<double>(numClients);

    for (auto &c : clients)
        c->disconnect();
    svc.stop();
}

BENCHMARK(BM_ParallelClients)->Arg(1)->Arg(2)->Arg(4)->Arg(8)
    ->Unit(benchmark::kMicrosecond);
