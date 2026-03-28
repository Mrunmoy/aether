// Simulated 16-pin GPIO expander IPC service.
//
// Run:   ./gpio_device
// Stop:  Ctrl-C

#include "GpioController.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <mutex>
#include <random>
#include <thread>

using namespace aether::ipc;

static constexpr int kPinCount = 16;
static constexpr int kErrInvalidPin = 1;
static constexpr int kErrPinNotOutput = 2;

static std::atomic<bool> g_running{true};

class GpioDevice : public GpioController
{
public:
    using GpioController::GpioController;

    void runSimulation()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> pinDist(0, kPinCount - 1);

        while (g_running.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (!g_running.load())
                break;

            int pin = pinDist(gen);

            bool fire = false;
            EdgeTrigger reportEdge = None;
            uint8_t reportPin = 0;

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto &ps = m_pins[pin];
                if (ps.direction != Input)
                    continue;

                PinLevel oldLevel = ps.level;
                PinLevel newLevel = (oldLevel == Low) ? High : Low;
                ps.level = newLevel;

                const char *oldStr = (oldLevel == Low) ? "LOW" : "HIGH";
                const char *newStr = (newLevel == Low) ? "LOW" : "HIGH";

                if (ps.edge == Both)
                {
                    fire = true;
                    reportEdge = (newLevel == High) ? Rising : Falling;
                }
                else if (ps.edge == Rising && newLevel == High)
                {
                    fire = true;
                    reportEdge = Rising;
                }
                else if (ps.edge == Falling && newLevel == Low)
                {
                    fire = true;
                    reportEdge = Falling;
                }

                reportPin = static_cast<uint8_t>(pin);

                if (fire)
                {
                    const char *edgeStr = (reportEdge == Rising) ? "Rising" : "Falling";
                    std::printf("[gpio] Pin %d: %s -> %s (interrupt: %s)\n",
                                pin, oldStr, newStr, edgeStr);
                }
                else
                {
                    std::printf("[gpio] Pin %d: %s -> %s\n", pin, oldStr, newStr);
                }
            }

            if (fire)
            {
                notifyPinInterrupt(reportPin, reportEdge);
            }
        }
    }

protected:
    int handleConfigurePin(uint8_t pin, PinDirection direction, EdgeTrigger edge) override
    {
        if (pin >= kPinCount)
            return kErrInvalidPin;

        std::lock_guard<std::mutex> lock(m_mutex);
        m_pins[pin].direction = direction;
        m_pins[pin].edge = edge;
        std::printf("[gpio] Pin %d configured: dir=%s edge=%d\n",
                    pin, (direction == Input) ? "INPUT" : "OUTPUT",
                    static_cast<int>(edge));
        return IPC_SUCCESS;
    }

    int handleReadPin(uint8_t pin, PinLevel *level) override
    {
        if (pin >= kPinCount)
            return kErrInvalidPin;

        std::lock_guard<std::mutex> lock(m_mutex);
        *level = m_pins[pin].level;
        return IPC_SUCCESS;
    }

    int handleWritePin(uint8_t pin, PinLevel level) override
    {
        if (pin >= kPinCount)
            return kErrInvalidPin;

        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_pins[pin].direction != Output)
            return kErrPinNotOutput;

        m_pins[pin].level = level;
        std::printf("[gpio] Pin %d written: %s\n", pin,
                    (level == Low) ? "LOW" : "HIGH");
        return IPC_SUCCESS;
    }

    int handleReadAllPins(PinState *pins) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (int i = 0; i < kPinCount; i++)
            pins[i] = m_pins[i];
        return IPC_SUCCESS;
    }

private:
    std::mutex m_mutex;
    PinState m_pins[kPinCount] = {};

public:
    GpioDevice(const char *name)
        : GpioController(name)
    {
        for (int i = 0; i < kPinCount; i++)
        {
            m_pins[i].pin = static_cast<uint8_t>(i);
            m_pins[i].direction = Input;
            m_pins[i].level = Low;
            m_pins[i].edge = None;
        }
    }
};

int main()
{
    std::signal(SIGINT, [](int) { g_running.store(false); });

    GpioDevice device("gpio");
    if (!device.start())
    {
        std::fprintf(stderr, "Failed to start GPIO service\n");
        return 1;
    }

    std::printf("[gpio] GPIO controller running (16 pins, Ctrl-C to stop)...\n");

    std::thread simThread([&device] { device.runSimulation(); });

    while (g_running.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    device.stop();
    if (simThread.joinable())
        simThread.join();

    std::printf("[gpio] Server stopped.\n");
    return 0;
}
