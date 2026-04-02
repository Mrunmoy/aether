// DSP mixer dashboard — connects to dsp_device and shows effect chain.
//
// Run:   ./dsp_device   (in another terminal)
//        ./dsp_dashboard

#include "AudioDsp.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <mutex>

using namespace aether::ipc;

static const char *effectTypeName(EffectType type)
{
    switch (type)
    {
    case EQ:         return "EQ";
    case Compressor: return "Compressor";
    case Reverb:     return "Reverb";
    case Delay:      return "Delay";
    case Limiter:    return "Limiter";
    default:         return "Unknown";
    }
}

static EffectType parseEffectType(const char *s)
{
    if (std::strcmp(s, "eq") == 0)      return EQ;
    if (std::strcmp(s, "comp") == 0)    return Compressor;
    if (std::strcmp(s, "reverb") == 0)  return Reverb;
    if (std::strcmp(s, "delay") == 0)   return Delay;
    if (std::strcmp(s, "limiter") == 0) return Limiter;
    return static_cast<EffectType>(0xFFu);
}

class DspDashboard : public AudioDsp
{
public:
    using AudioDsp::AudioDsp;

protected:
    void onClipDetected(float peakDb) override
    {
        std::printf("  *** CLIP DETECTED: peak=%.1f dB ***\n", peakDb);
    }

    void onLevelUpdate(SignalLevel level) override
    {
        std::lock_guard<std::mutex> lock(m_levelMutex);
        m_lastLevel = level;
    }

public:
    SignalLevel getLastLevel()
    {
        std::lock_guard<std::mutex> lock(m_levelMutex);
        return m_lastLevel;
    }

private:
    std::mutex m_levelMutex;
    SignalLevel m_lastLevel{};
};

static void showChain(DspDashboard &client)
{
    std::printf("\n=== DSP Effect Chain ===\n");
    for (uint8_t i = 0; i < 8; ++i)
    {
        EffectSlot slot{};
        int rc = client.GetEffectSlot(i, &slot);
        if (rc != IPC_SUCCESS)
        {
            std::printf("  Slot %u: (error %d)\n", i, rc);
            continue;
        }
        if (!slot.enabled)
        {
            std::printf("  Slot %u: (empty)\n", i);
        }
        else
        {
            std::printf("  Slot %u: [%-10s] \"%s\"  P1=%.1f  P2=%.1f  P3=%.1f  [ON]\n",
                        i, effectTypeName(slot.type), slot.name,
                        static_cast<double>(slot.param1),
                        static_cast<double>(slot.param2),
                        static_cast<double>(slot.param3));
        }
    }
}

static void showLevel(DspDashboard &client)
{
    SignalLevel level{};
    int rc = client.GetSignalLevel(&level);
    if (rc != IPC_SUCCESS)
    {
        std::printf("  Signal: (error %d)\n", rc);
        return;
    }
    std::printf("  Signal: Peak=%.1fdB  RMS=%.1fdB  [%s]\n",
                static_cast<double>(level.peakDb),
                static_cast<double>(level.rmsDb),
                level.clipping ? "CLIP" : "OK");
}

int main()
{
    DspDashboard client("audio-dsp");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to audio-dsp service\n");
        return 1;
    }
    std::printf("Connected to audio-dsp service.\n");
    std::printf("Commands: add <slot> <eq|comp|reverb|delay|limiter> <name>\n");
    std::printf("          remove <slot>  |  param <slot> <0-2> <value>\n");
    std::printf("          level  |  chain  |  quit\n\n");

    char line[256];
    while (std::printf("> "), std::fflush(stdout),
           std::fgets(line, sizeof(line), stdin))
    {
        char *nl = std::strchr(line, '\n');
        if (nl)
            *nl = '\0';

        char cmd[32] = {};
        std::sscanf(line, "%31s", cmd);

        if (std::strcmp(cmd, "quit") == 0 || std::strcmp(cmd, "q") == 0)
            break;

        if (std::strcmp(cmd, "chain") == 0)
        {
            showChain(client);
            showLevel(client);
        }
        else if (std::strcmp(cmd, "level") == 0)
        {
            showLevel(client);
        }
        else if (std::strcmp(cmd, "add") == 0)
        {
            unsigned slot = 0;
            char typeStr[32] = {};
            char name[33] = {};
            if (std::sscanf(line, "add %u %31s %32[^\n]", &slot, typeStr, name) < 3)
            {
                std::printf("Usage: add <slot> <eq|comp|reverb|delay|limiter> <name>\n");
                continue;
            }
            EffectType type = parseEffectType(typeStr);
            if (type == static_cast<EffectType>(0xFFu))
            {
                std::printf("Unknown effect type: %s\n", typeStr);
                continue;
            }
            uint8_t success = 0;
            int rc = client.AddEffect(static_cast<uint8_t>(slot), type, name, &success);
            if (rc != IPC_SUCCESS)
                std::printf("  Error: %d\n", rc);
            else if (success)
                std::printf("  Added %s to slot %u\n", effectTypeName(type), slot);
            else
                std::printf("  Slot %u is occupied\n", slot);
        }
        else if (std::strcmp(cmd, "remove") == 0)
        {
            unsigned slot = 0;
            if (std::sscanf(line, "remove %u", &slot) != 1)
            {
                std::printf("Usage: remove <slot>\n");
                continue;
            }
            uint8_t success = 0;
            int rc = client.RemoveEffect(static_cast<uint8_t>(slot), &success);
            if (rc != IPC_SUCCESS)
                std::printf("  Error: %d\n", rc);
            else if (success)
                std::printf("  Removed effect from slot %u\n", slot);
            else
                std::printf("  Slot %u is already empty\n", slot);
        }
        else if (std::strcmp(cmd, "param") == 0)
        {
            unsigned slot = 0;
            unsigned paramIdx = 0;
            float value = 0.0f;
            if (std::sscanf(line, "param %u %u %f", &slot, &paramIdx, &value) != 3)
            {
                std::printf("Usage: param <slot> <0-2> <value>\n");
                continue;
            }
            int rc = client.SetParameter(static_cast<uint8_t>(slot),
                                         static_cast<uint8_t>(paramIdx), value);
            if (rc != IPC_SUCCESS)
                std::printf("  Error: %d\n", rc);
            else
                std::printf("  Set param %u on slot %u to %.3f\n", paramIdx, slot,
                            static_cast<double>(value));
        }
        else if (cmd[0] != '\0')
        {
            std::printf("Unknown command: %s\n", cmd);
        }
    }

    client.disconnect();
    std::printf("Disconnected.\n");
    return 0;
}
