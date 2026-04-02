// Simulated DSP effects processor server.
//
// Run:   ./dsp_device
// Stop:  Ctrl-C

#include "AudioDsp.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <random>
#include <thread>

using namespace aether::ipc;

static std::atomic<bool> g_running{true};

class DspDevice : public AudioDsp
{
public:
    using AudioDsp::AudioDsp;

    void startSimulation()
    {
        m_simRunning.store(true);
        m_simThread = std::thread([this] { simulationLoop(); });
    }

    void stopSimulation()
    {
        m_simRunning.store(false);
        if (m_simThread.joinable())
            m_simThread.join();
    }

private:
    static constexpr uint8_t kMaxSlots = 8;

    std::mutex m_slotMutex;
    EffectSlot m_slots[kMaxSlots]{};
    SignalLevel m_currentLevel{-6.0f, -12.0f, 0};

    std::atomic<bool> m_simRunning{false};
    std::thread m_simThread;

protected:
    int handleAddEffect(uint8_t slotIndex, EffectType type,
                        const char *name, uint8_t *success) override
    {
        if (slotIndex >= kMaxSlots)
            return IPC_ERR_INVALID_ARGUMENT;

        std::lock_guard<std::mutex> lock(m_slotMutex);
        if (m_slots[slotIndex].enabled)
        {
            *success = 0;
            return IPC_SUCCESS;
        }

        m_slots[slotIndex].slotIndex = slotIndex;
        m_slots[slotIndex].type = type;
        m_slots[slotIndex].enabled = 1;
        m_slots[slotIndex].param1 = 0.0f;
        m_slots[slotIndex].param2 = 0.0f;
        m_slots[slotIndex].param3 = 0.0f;
        std::strncpy(m_slots[slotIndex].name, name, 32);
        m_slots[slotIndex].name[32] = '\0';
        *success = 1;
        return IPC_SUCCESS;
    }

    int handleRemoveEffect(uint8_t slotIndex, uint8_t *success) override
    {
        if (slotIndex >= kMaxSlots)
            return IPC_ERR_INVALID_ARGUMENT;

        std::lock_guard<std::mutex> lock(m_slotMutex);
        if (!m_slots[slotIndex].enabled)
        {
            *success = 0;
            return IPC_SUCCESS;
        }

        m_slots[slotIndex] = {};
        *success = 1;
        return IPC_SUCCESS;
    }

    int handleSetParameter(uint8_t slotIndex, uint8_t paramIndex,
                           float value) override
    {
        if (slotIndex >= kMaxSlots)
            return IPC_ERR_INVALID_ARGUMENT;

        std::lock_guard<std::mutex> lock(m_slotMutex);
        if (!m_slots[slotIndex].enabled)
            return IPC_ERR_INVALID_ARGUMENT;

        switch (paramIndex)
        {
        case 0: m_slots[slotIndex].param1 = value; break;
        case 1: m_slots[slotIndex].param2 = value; break;
        case 2: m_slots[slotIndex].param3 = value; break;
        default: return IPC_ERR_INVALID_ARGUMENT;
        }
        return IPC_SUCCESS;
    }

    int handleGetEffectSlot(uint8_t slotIndex, EffectSlot *slot) override
    {
        if (slotIndex >= kMaxSlots)
            return IPC_ERR_INVALID_ARGUMENT;

        std::lock_guard<std::mutex> lock(m_slotMutex);
        *slot = m_slots[slotIndex];
        return IPC_SUCCESS;
    }

    int handleGetSignalLevel(SignalLevel *level) override
    {
        std::lock_guard<std::mutex> lock(m_slotMutex);
        *level = m_currentLevel;
        return IPC_SUCCESS;
    }

    int handleGetEffectCount(uint8_t *count) override
    {
        std::lock_guard<std::mutex> lock(m_slotMutex);
        uint8_t n = 0;
        for (uint8_t i = 0; i < kMaxSlots; ++i)
        {
            if (m_slots[i].enabled)
                ++n;
        }
        *count = n;
        return IPC_SUCCESS;
    }

private:
    void simulationLoop()
    {
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> jitter(-3.0f, 3.0f);

        while (m_simRunning.load() && g_running.load())
        {
            float peak = -6.0f + jitter(rng);
            float rms = -12.0f + jitter(rng);

            {
                std::lock_guard<std::mutex> lock(m_slotMutex);
                for (uint8_t i = 0; i < kMaxSlots; ++i)
                {
                    if (m_slots[i].enabled && m_slots[i].type == Limiter)
                    {
                        if (peak > -3.0f)
                            peak = -3.0f;
                        break;
                    }
                }

                uint8_t clipping = (peak > -1.0f) ? 1 : 0;
                m_currentLevel = {peak, rms, clipping};
            }

            SignalLevel level = m_currentLevel;
            notifyLevelUpdate(level);

            if (level.clipping)
                notifyClipDetected(level.peakDb);

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
};

int main()
{
#if !defined(_WIN32)
    std::signal(SIGINT, [](int) { g_running.store(false); });
#endif

    DspDevice device("audio-dsp");
    if (!device.start())
    {
        std::fprintf(stderr, "Failed to start DSP service\n");
        return 1;
    }

    device.startSimulation();
    std::printf("=== DSP Device running (Ctrl-C to stop) ===\n");
    std::printf("Service: audio-dsp\n");
    std::printf("Slots:   8\n\n");

    while (g_running.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    device.stopSimulation();
    device.stop();
    std::printf("DSP device stopped.\n");
    return 0;
}
