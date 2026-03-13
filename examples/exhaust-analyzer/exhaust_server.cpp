// exhaust_server.cpp
//
// Simulated car exhaust gas analyzer service. Produces realistic sensor
// readings at 10 Hz and broadcasts them to all connected clients via
// IPC notifications. Designed to look and feel like a real device you'd
// find on a garage emissions bench or dyno tuning rig.

#include "ExhaustAnalyzer.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <mutex>
#include <random>
#include <thread>

using namespace ms::ipc;
using Clock = std::chrono::steady_clock;

static std::atomic<bool> g_quit{false};

static void onSignal(int) { g_quit.store(true); }

// ── Simulated sensor model ──────────────────────────────────────────
// Generates smoothly drifting values that resemble a real gasoline
// engine idling / revving, with small random perturbation.

class SensorSim
{
public:
    SensorSim() : m_rng(std::random_device{}()) {}

    GasReadings sample()
    {
        m_phase += 0.1;

        GasReadings r{};
        r.o2Percent = drift(1.2, 0.8, 0.3, m_phase * 0.7);
        r.coPercent = drift(1.5, 1.0, 0.4, m_phase * 0.5);
        r.co2Percent = drift(13.5, 1.0, 0.3, m_phase * 0.3);
        r.noxPpm = drift(1200.0, 600.0, 80.0, m_phase * 0.4);
        r.hcPpm = drift(160.0, 80.0, 20.0, m_phase * 0.6);
        r.lambda = drift(1.00, 0.03, 0.005, m_phase * 0.9);
        r.exhaustTempC = drift(420.0, 60.0, 8.0, m_phase * 0.2);
        r.timestampMs = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count());
        return r;
    }

private:
    double drift(double center, double amplitude, double noise, double phase)
    {
        std::normal_distribution<double> jitter(0.0, noise);
        return center + amplitude * std::sin(phase) + jitter(m_rng);
    }

    std::mt19937 m_rng;
    double m_phase = 0.0;
};

// ── Service implementation ──────────────────────────────────────────

class ExhaustAnalyzerServer : public ExhaustAnalyzer
{
public:
    using ExhaustAnalyzer::ExhaustAnalyzer;

    ~ExhaustAnalyzerServer() override { stopSim(); }

    void runSimulation()
    {
        setStatus(Warming);
        printf("[analyzer] warming up...\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));

        setStatus(Ready);
        printf("[analyzer] ready — waiting for clients\n");

        while (!g_quit.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        stopSim();
    }

protected:
    int handleGetStatus(AnalyzerStatus *status) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *status = m_status;
        return IPC_SUCCESS;
    }

    int handleGetCurrentReadings(GasReadings *readings) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *readings = m_latest;
        return IPC_SUCCESS;
    }

    int handleStartMeasurement(bool *started) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_status != Ready)
        {
            *started = false;
            return IPC_SUCCESS;
        }
        *started = true;
        m_status = Measuring;
        startSim();
        return IPC_SUCCESS;
    }

    int handleStopMeasurement(bool *stopped) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_status != Measuring)
        {
            *stopped = false;
            return IPC_SUCCESS;
        }
        *stopped = true;
        stopSimLocked();
        m_status = Ready;
        return IPC_SUCCESS;
    }

private:
    void setStatus(AnalyzerStatus s)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_status = s;
        }
        notifyStatusChanged(s);
    }

    void startSim()
    {
        m_simRunning.store(true);
        m_simThread = std::thread([this]
        {
            printf("[analyzer] measurement started — broadcasting at 10 Hz\n");
            notifyStatusChanged(Measuring);

            SensorSim sim;
            while (m_simRunning.load())
            {
                GasReadings r = sim.sample();
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_latest = r;
                }
                notifyReadingsUpdated(r);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }

    void stopSim()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        stopSimLocked();
    }

    void stopSimLocked()
    {
        if (m_simRunning.exchange(false))
        {
            // Must release mutex before joining since sim thread acquires it.
            m_mutex.unlock();
            if (m_simThread.joinable())
            {
                m_simThread.join();
            }
            m_mutex.lock();
            printf("[analyzer] measurement stopped\n");
        }
    }

    std::mutex m_mutex;
    AnalyzerStatus m_status = Offline;
    GasReadings m_latest{};
    std::atomic<bool> m_simRunning{false};
    std::thread m_simThread;
};

// ── main ────────────────────────────────────────────────────────────

int main()
{
    signal(SIGINT, onSignal);
    signal(SIGTERM, onSignal);

    ExhaustAnalyzerServer svc("exhaust_analyzer");
    if (!svc.start())
    {
        fprintf(stderr, "Failed to start service\n");
        return 1;
    }

    printf("═══════════════════════════════════════════════\n");
    printf("  Exhaust Gas Analyzer Service\n");
    printf("  Connect with: ./exhaust_client\n");
    printf("  Press Ctrl-C to stop\n");
    printf("═══════════════════════════════════════════════\n");

    svc.runSimulation();

    svc.stop();
    printf("[analyzer] shutdown complete\n");
    return 0;
}
