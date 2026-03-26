// Ring-buffer read/write micro-benchmarks.

#include "FrameIO.h"
#include "Types.h"

#include <benchmark/benchmark.h>

#include <cstring>
#include <memory>
#include <vector>

using namespace aether::ipc;

// ── WriteFrame ──────────────────────────────────────────────────────

static void BM_WriteFrame(benchmark::State &state)
{
    auto payloadSize = static_cast<uint32_t>(state.range(0));
    auto ring = std::make_unique<IpcRing>();
    std::vector<uint8_t> payload(payloadSize, 0xAB);
    FrameHeader hdr{};
    hdr.version = 1;
    hdr.serviceId = 0xDEADBEEF;
    hdr.messageId = 1;
    hdr.payloadBytes = payloadSize;

    for (auto _ : state)
    {
        // Drain ring to keep space available
        ring = std::make_unique<IpcRing>();
        int rc = writeFrame(ring.get(), hdr, payload.data(), payloadSize);
        benchmark::DoNotOptimize(rc);
    }

    state.SetBytesProcessed(
        state.iterations() * static_cast<int64_t>(sizeof(FrameHeader) + payloadSize));
}

BENCHMARK(BM_WriteFrame)->Arg(0)->Arg(64)->Arg(1024)->Arg(16384)->Arg(65536)
    ->Unit(benchmark::kNanosecond);

// ── ReadFrameAlloc ──────────────────────────────────────────────────

static void BM_ReadFrameAlloc(benchmark::State &state)
{
    auto payloadSize = static_cast<uint32_t>(state.range(0));
    std::vector<uint8_t> payload(payloadSize, 0xCD);
    FrameHeader hdr{};
    hdr.version = 1;
    hdr.serviceId = 0xDEADBEEF;
    hdr.messageId = 1;
    hdr.payloadBytes = payloadSize;

    for (auto _ : state)
    {
        auto ring = std::make_unique<IpcRing>();
        writeFrame(ring.get(), hdr, payload.data(), payloadSize);

        FrameHeader readHdr{};
        std::vector<uint8_t> readPayload;
        int rc = readFrameAlloc(ring.get(), &readHdr, &readPayload);
        benchmark::DoNotOptimize(rc);
        benchmark::DoNotOptimize(readPayload.data());
    }

    state.SetBytesProcessed(
        state.iterations() * static_cast<int64_t>(sizeof(FrameHeader) + payloadSize));
}

BENCHMARK(BM_ReadFrameAlloc)->Arg(0)->Arg(64)->Arg(1024)->Arg(16384)->Arg(65536)
    ->Unit(benchmark::kNanosecond);

// ── WriteRead round-trip ────────────────────────────────────────────

static void BM_WriteReadRoundTrip(benchmark::State &state)
{
    auto payloadSize = static_cast<uint32_t>(state.range(0));
    std::vector<uint8_t> payload(payloadSize, 0xEF);
    FrameHeader hdr{};
    hdr.version = 1;
    hdr.serviceId = 0xDEADBEEF;
    hdr.messageId = 1;
    hdr.payloadBytes = payloadSize;

    for (auto _ : state)
    {
        auto ring = std::make_unique<IpcRing>();
        writeFrame(ring.get(), hdr, payload.data(), payloadSize);

        FrameHeader readHdr{};
        std::vector<uint8_t> readPayload;
        readFrameAlloc(ring.get(), &readHdr, &readPayload);
        benchmark::DoNotOptimize(readPayload.data());
    }

    state.SetBytesProcessed(
        state.iterations() * static_cast<int64_t>(sizeof(FrameHeader) + payloadSize) * 2);
}

BENCHMARK(BM_WriteReadRoundTrip)->Arg(0)->Arg(64)->Arg(1024)->Arg(16384)
    ->Unit(benchmark::kNanosecond);

// ── PeekFrameHeader ────────────────────────────────────────────────

static void BM_PeekFrameHeader(benchmark::State &state)
{
    auto ring = std::make_unique<IpcRing>();
    FrameHeader hdr{};
    hdr.version = 1;
    hdr.serviceId = 0xDEADBEEF;
    hdr.messageId = 1;
    hdr.payloadBytes = 64;
    std::vector<uint8_t> payload(64, 0x00);
    writeFrame(ring.get(), hdr, payload.data(), 64);

    for (auto _ : state)
    {
        FrameHeader peekHdr{};
        bool ok = peekFrameHeader(ring.get(), &peekHdr);
        benchmark::DoNotOptimize(ok);
    }
}

BENCHMARK(BM_PeekFrameHeader)->Unit(benchmark::kNanosecond);

// ── Multi-frame pipeline ────────────────────────────────────────────

static void BM_MultiFramePipeline(benchmark::State &state)
{
    auto payloadSize = static_cast<uint32_t>(state.range(0));
    std::vector<uint8_t> payload(payloadSize, 0x42);
    FrameHeader hdr{};
    hdr.version = 1;
    hdr.serviceId = 0xDEADBEEF;
    hdr.messageId = 1;
    hdr.payloadBytes = payloadSize;

    for (auto _ : state)
    {
        auto ring = std::make_unique<IpcRing>();
        int written = 0;

        // Write as many frames as the ring can hold
        while (writeFrame(ring.get(), hdr, payload.data(), payloadSize) == IPC_SUCCESS)
            ++written;

        // Read them all back
        for (int i = 0; i < written; ++i)
        {
            FrameHeader readHdr{};
            std::vector<uint8_t> readPayload;
            readFrameAlloc(ring.get(), &readHdr, &readPayload);
        }

        state.counters["frames"] = static_cast<double>(written);
    }
}

BENCHMARK(BM_MultiFramePipeline)->Arg(64)->Arg(1024)
    ->Unit(benchmark::kMicrosecond);
