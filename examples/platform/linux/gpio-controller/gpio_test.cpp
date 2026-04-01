#include <gtest/gtest.h>
#include "GpioController.h"
#include "ClientBase.h"
#include "RunLoop.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <thread>

using namespace aether::ipc;

#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

static void settle()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

static constexpr int kPinCount = 16;
static constexpr int kErrInvalidPin = 1;
static constexpr int kErrPinNotOutput = 2;

// ── Test GPIO device (server) ──────────────────────────────────────

class TestGpioDevice : public GpioController
{
public:
    using GpioController::GpioController;
    using GpioController::notifyPinInterrupt;

    void simulatePinChange(uint8_t pin, PinLevel level)
    {
        if (pin >= kPinCount)
            return;

        std::lock_guard<std::mutex> lock(m_mutex);
        PinLevel oldLevel = m_pins[pin].level;
        m_pins[pin].level = level;

        EdgeTrigger edge = m_pins[pin].edge;
        if (edge == None || oldLevel == level)
            return;

        bool fire = false;
        EdgeTrigger reportEdge = None;

        if (edge == Both)
        {
            fire = true;
            reportEdge = (level == High) ? Rising : Falling;
        }
        else if (edge == Rising && level == High)
        {
            fire = true;
            reportEdge = Rising;
        }
        else if (edge == Falling && level == Low)
        {
            fire = true;
            reportEdge = Falling;
        }

        if (fire)
            notifyPinInterrupt(pin, reportEdge);
    }

protected:
    int handleConfigurePin(uint8_t pin, PinDirection direction, EdgeTrigger edge) override
    {
        if (pin >= kPinCount)
            return kErrInvalidPin;

        std::lock_guard<std::mutex> lock(m_mutex);
        m_pins[pin].direction = direction;
        m_pins[pin].edge = edge;
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
    void initPins()
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

// ── Notification-receiving client (raw ClientBase) ─────────────────

class TestGpioClient : public ClientBase
{
public:
    using ClientBase::ClientBase;

    std::mutex mtx;
    std::condition_variable cv;
    uint8_t lastPin = 0;
    EdgeTrigger lastEdge = None;
    std::atomic<int> interruptCount{0};

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        if (serviceId != GpioController::kServiceId)
            return;
        if (messageId != GpioController::kPinInterrupt)
            return;
        if (payload.size() < sizeof(uint8_t) + sizeof(EdgeTrigger))
            return;

        std::lock_guard<std::mutex> lock(mtx);
        std::memcpy(&lastPin, payload.data(), sizeof(lastPin));
        std::memcpy(&lastEdge, payload.data() + sizeof(lastPin), sizeof(lastEdge));
        interruptCount++;
        cv.notify_all();
    }
};

// ── RunLoop RAII guard ─────────────────────────────────────────────

struct RunLoopGuard
{
    ms::RunLoop &loop;
    std::thread thread;

    explicit RunLoopGuard(ms::RunLoop &l) : loop(l), thread([&l] { l.run(); }) {}

    ~RunLoopGuard()
    {
        loop.stop();
        if (thread.joinable())
            thread.join();
    }
};

// ── Helpers for raw-client marshal/unmarshal ───────────────────────

static int callConfigurePin(ClientBase &client, uint8_t pin,
                            PinDirection direction, EdgeTrigger edge)
{
    std::vector<uint8_t> request(sizeof(pin) + sizeof(direction) + sizeof(edge));
    std::memcpy(request.data(), &pin, sizeof(pin));
    std::memcpy(request.data() + sizeof(pin), &direction, sizeof(direction));
    std::memcpy(request.data() + sizeof(pin) + sizeof(direction), &edge, sizeof(edge));

    std::vector<uint8_t> response;
    return client.call(GpioController::kServiceId, GpioController::kConfigurePin,
                       request, &response);
}

static int callReadPin(ClientBase &client, uint8_t pin, PinLevel *level)
{
    std::vector<uint8_t> request(sizeof(pin));
    std::memcpy(request.data(), &pin, sizeof(pin));

    std::vector<uint8_t> response;
    int rc = client.call(GpioController::kServiceId, GpioController::kReadPin,
                         request, &response);
    if (rc == IPC_SUCCESS && response.size() >= sizeof(PinLevel))
        std::memcpy(level, response.data(), sizeof(PinLevel));
    return rc;
}

static int callWritePin(ClientBase &client, uint8_t pin, PinLevel level)
{
    std::vector<uint8_t> request(sizeof(pin) + sizeof(level));
    std::memcpy(request.data(), &pin, sizeof(pin));
    std::memcpy(request.data() + sizeof(pin), &level, sizeof(level));

    std::vector<uint8_t> response;
    return client.call(GpioController::kServiceId, GpioController::kWritePin,
                       request, &response);
}

static int callReadAllPins(ClientBase &client, PinState *pins)
{
    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(GpioController::kServiceId, GpioController::kReadAllPins,
                         request, &response);
    if (rc == IPC_SUCCESS && response.size() >= kPinCount * sizeof(PinState))
        std::memcpy(pins, response.data(), kPinCount * sizeof(PinState));
    return rc;
}

// ═══════════════════════════════════════════════════════════════════
// ConfigurePin_ValidPin
// ═══════════════════════════════════════════════════════════════════

TEST(GpioControllerTest, ConfigurePin_ValidPin)
{
    TestGpioDevice svc(SVC_NAME);
    svc.initPins();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    int rc = callConfigurePin(client, 0, Output, None);
    EXPECT_EQ(rc, IPC_SUCCESS);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// ConfigurePin_InvalidPin
// ═══════════════════════════════════════════════════════════════════

TEST(GpioControllerTest, ConfigurePin_InvalidPin)
{
    TestGpioDevice svc(SVC_NAME);
    svc.initPins();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    int rc = callConfigurePin(client, 99, Output, None);
    EXPECT_EQ(rc, kErrInvalidPin);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// WriteAndReadBack
// ═══════════════════════════════════════════════════════════════════

TEST(GpioControllerTest, WriteAndReadBack)
{
    TestGpioDevice svc(SVC_NAME);
    svc.initPins();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Configure pin 3 as Output first.
    int rc = callConfigurePin(client, 3, Output, None);
    ASSERT_EQ(rc, IPC_SUCCESS);

    rc = callWritePin(client, 3, High);
    ASSERT_EQ(rc, IPC_SUCCESS);

    PinLevel level{};
    rc = callReadPin(client, 3, &level);
    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(level, High);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// WriteInputPin_Fails
// ═══════════════════════════════════════════════════════════════════

TEST(GpioControllerTest, WriteInputPin_Fails)
{
    TestGpioDevice svc(SVC_NAME);
    svc.initPins();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Pin 5 defaults to Input — writing should fail.
    int rc = callWritePin(client, 5, High);
    EXPECT_EQ(rc, kErrPinNotOutput);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// ReadAllPins_Returns16
// ═══════════════════════════════════════════════════════════════════

TEST(GpioControllerTest, ReadAllPins_Returns16)
{
    TestGpioDevice svc(SVC_NAME);
    svc.initPins();
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    PinState pins[kPinCount]{};
    int rc = callReadAllPins(client, pins);
    ASSERT_EQ(rc, IPC_SUCCESS);

    for (int i = 0; i < kPinCount; i++)
    {
        EXPECT_EQ(pins[i].pin, static_cast<uint8_t>(i));
        EXPECT_EQ(pins[i].direction, Input);
        EXPECT_EQ(pins[i].level, Low);
    }

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// PinInterrupt_NotificationFires
// ═══════════════════════════════════════════════════════════════════

TEST(GpioControllerTest, PinInterrupt_NotificationFires)
{
    TestGpioDevice svc(SVC_NAME);
    svc.initPins();
    ASSERT_TRUE(svc.start());
    settle();

    TestGpioClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Configure pin 7 as Input with Rising edge.
    int rc = callConfigurePin(client, 7, Input, Rising);
    ASSERT_EQ(rc, IPC_SUCCESS);

    // Simulate a Low->High transition.
    svc.simulatePinChange(7, High);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(2000),
                                        [&] { return client.interruptCount > 0; }));
    }

    EXPECT_EQ(client.lastPin, 7);
    EXPECT_EQ(client.lastEdge, Rising);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// NoInterrupt_WhenEdgeNone
// ═══════════════════════════════════════════════════════════════════

TEST(GpioControllerTest, NoInterrupt_WhenEdgeNone)
{
    TestGpioDevice svc(SVC_NAME);
    svc.initPins();
    ASSERT_TRUE(svc.start());
    settle();

    TestGpioClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Pin 4 defaults to Input with None edge — no interrupt expected.
    svc.simulatePinChange(4, High);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        bool fired = client.cv.wait_for(lock, std::chrono::milliseconds(200),
                                         [&] { return client.interruptCount > 0; });
        EXPECT_FALSE(fired);
    }
    EXPECT_EQ(client.interruptCount.load(), 0);

    client.disconnect();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════
// RunLoop_Dispatch
// ═══════════════════════════════════════════════════════════════════

TEST(GpioControllerTest, RunLoop_Dispatch)
{
    ms::RunLoop loop;
    loop.init("SrvRL");

    TestGpioDevice svc(SVC_NAME, &loop);
    svc.initPins();
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // ConfigurePin via RunLoop-driven server.
    int rc = callConfigurePin(client, 2, Output, None);
    ASSERT_EQ(rc, IPC_SUCCESS);

    // Write and read back.
    rc = callWritePin(client, 2, High);
    ASSERT_EQ(rc, IPC_SUCCESS);

    PinLevel level{};
    rc = callReadPin(client, 2, &level);
    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(level, High);

    client.disconnect();
    svc.stop();
}
