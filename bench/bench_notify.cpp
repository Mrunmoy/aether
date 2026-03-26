// Micro-benchmarks for notification broadcast latency and throughput.

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

// ── Service that exposes sendNotify via a public helper ──────────────

class BenchNotifyService : public ServiceBase
{
public:
    explicit BenchNotifyService(const char *name) : ServiceBase(name) {}

    int notify(uint32_t messageId, const uint8_t *payload, uint32_t payloadBytes)
    {
        return sendNotify(0xBEEF, messageId, payload, payloadBytes);
    }

protected:
    int onRequest(uint32_t, const std::vector<uint8_t> &,
                  std::vector<uint8_t> *) override
    {
        return IPC_SUCCESS;
    }
};

// ── Notification-receiving client ───────────────────────────────────

class BenchNotifyClient : public ClientBase
{
public:
    explicit BenchNotifyClient(const char *name) : ClientBase(name) {}
    std::atomic<uint64_t> received{0};

protected:
    void onNotification(uint32_t, uint32_t,
                        const std::vector<uint8_t> &) override
    {
        received.fetch_add(1, std::memory_order_relaxed);
    }
};

static std::atomic<int> s_nameCounter{0};

static std::string uniqueName()
{
    return "bench_ntf_" + std::to_string(s_nameCounter.fetch_add(1));
}

// ── sendNotify latency (single client, varying payload) ──────────────

static void BM_NotifyLatency(benchmark::State &state)
{
    auto payloadSize = static_cast<uint32_t>(state.range(0));
    std::string name = uniqueName();

    BenchNotifyService svc(name.c_str());
    svc.start();

    BenchNotifyClient client(name.c_str());
    client.connect();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::vector<uint8_t> payload(payloadSize, 0xDD);

    for (auto _ : state)
    {
        int rc = svc.notify(1, payload.data(), payloadSize);
        benchmark::DoNotOptimize(rc);
    }

    state.SetBytesProcessed(
        state.iterations() * static_cast<int64_t>(payloadSize));

    client.disconnect();
    svc.stop();
}

BENCHMARK(BM_NotifyLatency)->Arg(0)->Arg(64)->Arg(1024)->Arg(16384)
    ->Unit(benchmark::kMicrosecond);

// ── Broadcast to N clients ──────────────────────────────────────────

static void BM_NotifyBroadcast(benchmark::State &state)
{
    auto numClients = static_cast<int>(state.range(0));
    std::string name = uniqueName();

    BenchNotifyService svc(name.c_str());
    svc.start();

    std::vector<std::unique_ptr<BenchNotifyClient>> clients;
    for (int i = 0; i < numClients; ++i)
    {
        auto c = std::make_unique<BenchNotifyClient>(name.c_str());
        c->connect();
        clients.push_back(std::move(c));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    std::vector<uint8_t> payload(64, 0xEE);

    for (auto _ : state)
    {
        int rc = svc.notify(1, payload.data(), 64);
        benchmark::DoNotOptimize(rc);
    }

    state.SetItemsProcessed(state.iterations() * numClients);
    state.counters["clients"] = static_cast<double>(numClients);

    for (auto &c : clients)
        c->disconnect();
    svc.stop();
}

BENCHMARK(BM_NotifyBroadcast)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16)
    ->Unit(benchmark::kMicrosecond);

// ── Notification throughput (saturate ring) ──────────────────────────

static void BM_NotifyThroughput(benchmark::State &state)
{
    std::string name = uniqueName();

    BenchNotifyService svc(name.c_str());
    svc.start();

    BenchNotifyClient client(name.c_str());
    client.connect();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::vector<uint8_t> payload(64, 0xFF);

    for (auto _ : state)
    {
        int rc = svc.notify(1, payload.data(), 64);
        benchmark::DoNotOptimize(rc);
    }

    state.counters["notifications/s"] = benchmark::Counter(
        static_cast<double>(state.iterations()),
        benchmark::Counter::kIsRate);

    client.disconnect();
    svc.stop();
}

BENCHMARK(BM_NotifyThroughput)->Unit(benchmark::kMicrosecond);
