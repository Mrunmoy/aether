#include <gtest/gtest.h>
#include "MotorController.h"
#include "ClientBase.h"
#include "RunLoop.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <condition_variable>
#include <mutex>
#include <thread>

using namespace aether::ipc;

#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

static void settle()
{
#if defined(_WIN32)
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
#endif
}

// ── Test simulation constants (tighter range for fast tests) ────────

static constexpr int32_t kPositionMin  = -200;
static constexpr int32_t kPositionMax  =  200;
static constexpr float   kMaxVelocity  =  1000.0f;
static constexpr float   kAcceleration =  5000.0f;
static constexpr float   kTickInterval =  0.005f;  // 5 ms

// ── Test motor device (server side) ─────────────────────────────────

class TestMotorDevice : public MotorController
{
public:
    using MotorController::MotorController;

    ~TestMotorDevice() override
    {
        stopSimulation();
    }

    void startSimulation()
    {
        m_simRunning.store(true);
        m_simThread = std::thread(&TestMotorDevice::simulationLoop, this);
    }

    void stopSimulation()
    {
        m_simRunning.store(false);
        if (m_simThread.joinable())
            m_simThread.join();
    }

    // Wait for a specific state with timeout.
    bool waitForState(MotorState target, int timeoutMs)
    {
        auto deadline = std::chrono::steady_clock::now()
                      + std::chrono::milliseconds(timeoutMs);
        while (std::chrono::steady_clock::now() < deadline)
        {
            {
                std::lock_guard<std::mutex> lock(m_mtx);
                if (m_state == target)
                    return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        return false;
    }

    // Force a stall condition for testing.
    void simulateStall()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_forceStall = true;
    }

    void clearStall()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_forceStall = false;
    }

protected:
    int handleHome() override
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_state = Homing;
        m_targetVelocity = -kMaxVelocity;
        m_homing = true;
        m_homed = false;
        return IPC_SUCCESS;
    }

    int handleMoveTo(int32_t position, float velocity) override
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        float vel = std::fabs(velocity);
        if (vel > kMaxVelocity) vel = kMaxVelocity;
        if (vel < 1.0f) vel = 1.0f;

        m_state = Moving;
        m_targetPosition = position;
        m_moveToActive = true;
        m_jogActive = false;
        m_targetVelocity = (position >= m_position) ? vel : -vel;
        return IPC_SUCCESS;
    }

    int handleJog(float velocity) override
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        float vel = velocity;
        if (std::fabs(vel) > kMaxVelocity)
            vel = (vel > 0.0f) ? kMaxVelocity : -kMaxVelocity;

        m_state = Moving;
        m_targetVelocity = vel;
        m_moveToActive = false;
        m_jogActive = true;
        return IPC_SUCCESS;
    }

    int handleStop() override
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_state = Stopped;
        m_velocity = 0.0f;
        m_targetVelocity = 0.0f;
        m_moveToActive = false;
        m_jogActive = false;
        m_homing = false;
        return IPC_SUCCESS;
    }

    int handleGetStatus(MotorStatus *status) override
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        status->state = m_state;
        status->position = m_position;
        status->velocity = m_velocity;
        status->activeLimits = activeLimits();
        status->homed = m_homed ? 1 : 0;
        return IPC_SUCCESS;
    }

    int handleGetPosition(int32_t *position) override
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        *position = m_position;
        return IPC_SUCCESS;
    }

private:
    LimitSwitch activeLimits() const
    {
        if (m_position <= kPositionMin) return Lower;
        if (m_position >= kPositionMax) return Upper;
        return None;
    }

    void simulationLoop()
    {
        using Clock = std::chrono::steady_clock;
        auto lastTick = Clock::now();
        int stallMs = 0;

        while (m_simRunning.load())
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(static_cast<int>(kTickInterval * 1000.0f)));

            auto now = Clock::now();
            float dt = std::chrono::duration<float>(now - lastTick).count();
            lastTick = now;

            std::lock_guard<std::mutex> lock(m_mtx);

            if (m_state != Moving && m_state != Homing)
            {
                stallMs = 0;
                continue;
            }

            int32_t prevPosition = m_position;

            // Velocity update
            if (m_moveToActive)
            {
                float decelDist = (m_velocity * m_velocity) / (2.0f * kAcceleration);
                float remaining = static_cast<float>(m_targetPosition - m_position);
                float absRemaining = std::fabs(remaining);

                if (absRemaining <= 0.5f)
                {
                    m_position = m_targetPosition;
                    m_velocity = 0.0f;
                    m_moveToActive = false;
                    m_state = Idle;
                    notifyMotionComplete(m_position);
                    stallMs = 0;
                    continue;
                }

                if (absRemaining <= decelDist)
                {
                    float decel = kAcceleration * dt;
                    if (m_velocity > 0.0f)
                        m_velocity = std::max(0.0f, m_velocity - decel);
                    else
                        m_velocity = std::min(0.0f, m_velocity + decel);

                    if (std::fabs(m_velocity) < 1.0f && absRemaining < 2.0f)
                    {
                        m_position = m_targetPosition;
                        m_velocity = 0.0f;
                        m_moveToActive = false;
                        m_state = Idle;
                        notifyMotionComplete(m_position);
                        stallMs = 0;
                        continue;
                    }
                }
                else
                {
                    float accel = kAcceleration * dt;
                    if (m_velocity < m_targetVelocity)
                        m_velocity = std::min(m_velocity + accel, m_targetVelocity);
                    else if (m_velocity > m_targetVelocity)
                        m_velocity = std::max(m_velocity - accel, m_targetVelocity);
                }
            }
            else
            {
                float accel = kAcceleration * dt;
                if (m_velocity < m_targetVelocity)
                    m_velocity = std::min(m_velocity + accel, m_targetVelocity);
                else if (m_velocity > m_targetVelocity)
                    m_velocity = std::max(m_velocity - accel, m_targetVelocity);
            }

            // Position update (suppress if stall is forced)
            if (!m_forceStall)
            {
                float posFloat = static_cast<float>(m_position) + m_velocity * dt;
                m_position = static_cast<int32_t>(std::round(posFloat));
            }

            // Limit switch checks
            if (m_position <= kPositionMin)
            {
                m_position = kPositionMin;
                m_velocity = 0.0f;
                m_targetVelocity = 0.0f;
                m_moveToActive = false;
                m_jogActive = false;

                if (m_homing)
                {
                    m_position = 0;
                    m_homing = false;
                    m_homed = true;
                    m_state = Idle;
                    notifyMotionComplete(m_position);
                    stallMs = 0;
                    continue;
                }

                m_state = Idle;
                notifyLimitHit(Lower, m_position);
                stallMs = 0;
                continue;
            }

            if (m_position >= kPositionMax)
            {
                m_position = kPositionMax;
                m_velocity = 0.0f;
                m_targetVelocity = 0.0f;
                m_moveToActive = false;
                m_jogActive = false;
                m_homing = false;
                m_state = Idle;
                notifyLimitHit(Upper, m_position);
                stallMs = 0;
                continue;
            }

            // Stall detection
            if (std::fabs(m_velocity) > 1.0f && m_position == prevPosition)
            {
                stallMs += static_cast<int>(dt * 1000.0f);
                if (stallMs >= 500)
                {
                    m_state = Fault;
                    float savedVel = m_velocity;
                    m_velocity = 0.0f;
                    m_targetVelocity = 0.0f;
                    m_moveToActive = false;
                    m_jogActive = false;
                    m_homing = false;
                    notifyStallDetected(m_position, savedVel);
                    stallMs = 0;
                    continue;
                }
            }
            else
            {
                stallMs = 0;
            }
        }
    }

    std::mutex m_mtx;
    MotorState m_state = Idle;
    int32_t m_position = 0;
    float m_velocity = 0.0f;
    float m_targetVelocity = 0.0f;
    int32_t m_targetPosition = 0;
    bool m_moveToActive = false;
    bool m_jogActive = false;
    bool m_homing = false;
    bool m_homed = false;
    bool m_forceStall = false;

    std::atomic<bool> m_simRunning{false};
    std::thread m_simThread;
};

// ── Notification-receiving test client ──────────────────────────────

class TestMotorClient : public ClientBase
{
public:
    using ClientBase::ClientBase;

    std::mutex mtx;
    std::condition_variable cv;
    int32_t lastCompletePos = 0;
    LimitSwitch lastLimit = None;
    int32_t lastLimitPos = 0;
    int32_t lastStallPos = 0;
    float lastStallVel = 0.0f;
    std::atomic<int> completeCount{0};
    std::atomic<int> limitCount{0};
    std::atomic<int> stallCount{0};

    bool waitForComplete(int timeoutMs)
    {
        std::unique_lock<std::mutex> lock(mtx);
        int before = completeCount.load();
        return cv.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                           [&] { return completeCount.load() > before; });
    }

    bool waitForLimit(int timeoutMs)
    {
        std::unique_lock<std::mutex> lock(mtx);
        int before = limitCount.load();
        return cv.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                           [&] { return limitCount.load() > before; });
    }

    bool waitForStall(int timeoutMs)
    {
        std::unique_lock<std::mutex> lock(mtx);
        int before = stallCount.load();
        return cv.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                           [&] { return stallCount.load() > before; });
    }

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        if (serviceId != MotorController::kServiceId)
            return;

        std::lock_guard<std::mutex> lock(mtx);

        switch (messageId)
        {
        case MotorController::kMotionComplete:
        {
            if (payload.size() >= sizeof(int32_t))
            {
                std::memcpy(&lastCompletePos, payload.data(), sizeof(int32_t));
                completeCount++;
                cv.notify_all();
            }
            break;
        }
        case MotorController::kLimitHit:
        {
            if (payload.size() >= sizeof(LimitSwitch) + sizeof(int32_t))
            {
                std::memcpy(&lastLimit, payload.data(), sizeof(LimitSwitch));
                std::memcpy(&lastLimitPos, payload.data() + sizeof(LimitSwitch), sizeof(int32_t));
                limitCount++;
                cv.notify_all();
            }
            break;
        }
        case MotorController::kStallDetected:
        {
            if (payload.size() >= sizeof(int32_t) + sizeof(float))
            {
                std::memcpy(&lastStallPos, payload.data(), sizeof(int32_t));
                std::memcpy(&lastStallVel, payload.data() + sizeof(int32_t), sizeof(float));
                stallCount++;
                cv.notify_all();
            }
            break;
        }
        default:
            break;
        }
    }
};

// ── RunLoop RAII guard ──────────────────────────────────────────────

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

// ═════════════════════════════════════════════════════════════════════
// Home_CompletesSuccessfully
// ═════════════════════════════════════════════════════════════════════

TEST(MotorControllerTest, Home_CompletesSuccessfully)
{
    TestMotorDevice svc(SVC_NAME);
    svc.startSimulation();
    ASSERT_TRUE(svc.start());
    settle();

    TestMotorClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Issue Home RPC
    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(MotorController::kServiceId, MotorController::kHome, request, &response);
    ASSERT_EQ(rc, IPC_SUCCESS);

    // Wait for MotionComplete notification
    ASSERT_TRUE(client.waitForComplete(5000));
    EXPECT_EQ(client.lastCompletePos, 0);

    // Verify position is 0
    std::vector<uint8_t> posReq;
    std::vector<uint8_t> posResp;
    rc = client.call(MotorController::kServiceId, MotorController::kGetPosition, posReq, &posResp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(posResp.size(), sizeof(int32_t));
    int32_t pos;
    std::memcpy(&pos, posResp.data(), sizeof(pos));
    EXPECT_EQ(pos, 0);

    client.disconnect();
    svc.stop();
    svc.stopSimulation();
}

// ═════════════════════════════════════════════════════════════════════
// MoveTo_ReachesTarget
// ═════════════════════════════════════════════════════════════════════

TEST(MotorControllerTest, MoveTo_ReachesTarget)
{
    TestMotorDevice svc(SVC_NAME);
    svc.startSimulation();
    ASSERT_TRUE(svc.start());
    settle();

    TestMotorClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Home first
    std::vector<uint8_t> req, resp;
    int rc = client.call(MotorController::kServiceId, MotorController::kHome, req, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_TRUE(client.waitForComplete(5000));

    // MoveTo 500 at 800 steps/s
    int32_t targetPos = 100;
    float targetVel = 800.0f;
    std::vector<uint8_t> moveReq(sizeof(targetPos) + sizeof(targetVel));
    std::memcpy(moveReq.data(), &targetPos, sizeof(targetPos));
    std::memcpy(moveReq.data() + sizeof(targetPos), &targetVel, sizeof(targetVel));

    resp.clear();
    rc = client.call(MotorController::kServiceId, MotorController::kMoveTo, moveReq, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);

    ASSERT_TRUE(client.waitForComplete(5000));
    EXPECT_EQ(client.lastCompletePos, 100);

    client.disconnect();
    svc.stop();
    svc.stopSimulation();
}

// ═════════════════════════════════════════════════════════════════════
// Stop_HaltsMotion
// ═════════════════════════════════════════════════════════════════════

TEST(MotorControllerTest, Stop_HaltsMotion)
{
    TestMotorDevice svc(SVC_NAME);
    svc.startSimulation();
    ASSERT_TRUE(svc.start());
    settle();

    TestMotorClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Start jogging
    float jogVel = 200.0f;
    std::vector<uint8_t> jogReq(sizeof(jogVel));
    std::memcpy(jogReq.data(), &jogVel, sizeof(jogVel));

    std::vector<uint8_t> resp;
    int rc = client.call(MotorController::kServiceId, MotorController::kJog, jogReq, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);

    // Let it move a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Stop
    std::vector<uint8_t> stopReq;
    resp.clear();
    rc = client.call(MotorController::kServiceId, MotorController::kStop, stopReq, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);

    // Verify state is Stopped
    std::vector<uint8_t> statusReq;
    resp.clear();
    rc = client.call(MotorController::kServiceId, MotorController::kGetStatus, statusReq, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(resp.size(), sizeof(MotorStatus));

    MotorStatus status;
    std::memcpy(&status, resp.data(), sizeof(status));
    EXPECT_EQ(status.state, Stopped);

    client.disconnect();
    svc.stop();
    svc.stopSimulation();
}

// ═════════════════════════════════════════════════════════════════════
// GetStatus_ReturnsValidState
// ═════════════════════════════════════════════════════════════════════

TEST(MotorControllerTest, GetStatus_ReturnsValidState)
{
    TestMotorDevice svc(SVC_NAME);
    svc.startSimulation();
    ASSERT_TRUE(svc.start());
    settle();

    TestMotorClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> req, resp;
    int rc = client.call(MotorController::kServiceId, MotorController::kGetStatus, req, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(resp.size(), sizeof(MotorStatus));

    MotorStatus status;
    std::memcpy(&status, resp.data(), sizeof(status));
    EXPECT_TRUE(status.state == Idle || status.state == Homing || status.state == Moving ||
                status.state == Stopped || status.state == Fault);

    client.disconnect();
    svc.stop();
    svc.stopSimulation();
}

// ═════════════════════════════════════════════════════════════════════
// GetPosition_AfterHome
// ═════════════════════════════════════════════════════════════════════

TEST(MotorControllerTest, GetPosition_AfterHome)
{
    TestMotorDevice svc(SVC_NAME);
    svc.startSimulation();
    ASSERT_TRUE(svc.start());
    settle();

    TestMotorClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Home
    std::vector<uint8_t> req, resp;
    int rc = client.call(MotorController::kServiceId, MotorController::kHome, req, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_TRUE(client.waitForComplete(5000));

    // GetPosition
    req.clear();
    resp.clear();
    rc = client.call(MotorController::kServiceId, MotorController::kGetPosition, req, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(resp.size(), sizeof(int32_t));

    int32_t pos;
    std::memcpy(&pos, resp.data(), sizeof(pos));
    EXPECT_EQ(pos, 0);

    client.disconnect();
    svc.stop();
    svc.stopSimulation();
}

// ═════════════════════════════════════════════════════════════════════
// LimitHit_NotificationFires
// ═════════════════════════════════════════════════════════════════════

TEST(MotorControllerTest, LimitHit_NotificationFires)
{
    TestMotorDevice svc(SVC_NAME);
    svc.startSimulation();
    ASSERT_TRUE(svc.start());
    settle();

    TestMotorClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Jog toward upper limit at max speed
    float jogVel = kMaxVelocity;
    std::vector<uint8_t> jogReq(sizeof(jogVel));
    std::memcpy(jogReq.data(), &jogVel, sizeof(jogVel));

    std::vector<uint8_t> resp;
    int rc = client.call(MotorController::kServiceId, MotorController::kJog, jogReq, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);

    ASSERT_TRUE(client.waitForLimit(5000));
    EXPECT_EQ(client.lastLimit, Upper);
    EXPECT_EQ(client.lastLimitPos, kPositionMax);

    client.disconnect();
    svc.stop();
    svc.stopSimulation();
}

// ═════════════════════════════════════════════════════════════════════
// MotionComplete_NotificationFires
// ═════════════════════════════════════════════════════════════════════

TEST(MotorControllerTest, MotionComplete_NotificationFires)
{
    TestMotorDevice svc(SVC_NAME);
    svc.startSimulation();
    ASSERT_TRUE(svc.start());
    settle();

    TestMotorClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // MoveTo 100
    int32_t targetPos = 100;
    float targetVel = 500.0f;
    std::vector<uint8_t> moveReq(sizeof(targetPos) + sizeof(targetVel));
    std::memcpy(moveReq.data(), &targetPos, sizeof(targetPos));
    std::memcpy(moveReq.data() + sizeof(targetPos), &targetVel, sizeof(targetVel));

    std::vector<uint8_t> resp;
    int rc = client.call(MotorController::kServiceId, MotorController::kMoveTo, moveReq, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);

    ASSERT_TRUE(client.waitForComplete(5000));
    EXPECT_EQ(client.lastCompletePos, 100);

    client.disconnect();
    svc.stop();
    svc.stopSimulation();
}

// ═════════════════════════════════════════════════════════════════════
// StallDetected_NotificationFires
// ═════════════════════════════════════════════════════════════════════

TEST(MotorControllerTest, StallDetected_NotificationFires)
{
    TestMotorDevice svc(SVC_NAME);
    svc.startSimulation();
    ASSERT_TRUE(svc.start());
    settle();

    TestMotorClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Start jogging, then force stall
    float jogVel = 200.0f;
    std::vector<uint8_t> jogReq(sizeof(jogVel));
    std::memcpy(jogReq.data(), &jogVel, sizeof(jogVel));

    std::vector<uint8_t> resp;
    int rc = client.call(MotorController::kServiceId, MotorController::kJog, jogReq, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);

    // Let velocity ramp up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Force stall — position stops updating but velocity remains non-zero
    svc.simulateStall();

    // Stall should fire within ~600ms (500ms timeout + margin)
    ASSERT_TRUE(client.waitForStall(2000));
    EXPECT_GT(std::fabs(client.lastStallVel), 0.0f);

    svc.clearStall();
    client.disconnect();
    svc.stop();
    svc.stopSimulation();
}

// ═════════════════════════════════════════════════════════════════════
// MoveWithoutHome_Allowed
// ═════════════════════════════════════════════════════════════════════

TEST(MotorControllerTest, MoveWithoutHome_Allowed)
{
    TestMotorDevice svc(SVC_NAME);
    svc.startSimulation();
    ASSERT_TRUE(svc.start());
    settle();

    TestMotorClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // MoveTo without homing first — should still work
    int32_t targetPos = 50;
    float targetVel = 500.0f;
    std::vector<uint8_t> moveReq(sizeof(targetPos) + sizeof(targetVel));
    std::memcpy(moveReq.data(), &targetPos, sizeof(targetPos));
    std::memcpy(moveReq.data() + sizeof(targetPos), &targetVel, sizeof(targetVel));

    std::vector<uint8_t> resp;
    int rc = client.call(MotorController::kServiceId, MotorController::kMoveTo, moveReq, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);

    ASSERT_TRUE(client.waitForComplete(5000));
    EXPECT_EQ(client.lastCompletePos, 50);

    // Verify homed flag is 0
    std::vector<uint8_t> statusReq;
    resp.clear();
    rc = client.call(MotorController::kServiceId, MotorController::kGetStatus, statusReq, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(resp.size(), sizeof(MotorStatus));

    MotorStatus status;
    std::memcpy(&status, resp.data(), sizeof(status));
    EXPECT_EQ(status.homed, 0);

    client.disconnect();
    svc.stop();
    svc.stopSimulation();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop_Dispatch
// ═════════════════════════════════════════════════════════════════════

TEST(MotorControllerTest, RunLoop_Dispatch)
{
    ms::RunLoop loop;
    loop.init("SrvRL");

    TestMotorDevice svc(SVC_NAME, &loop);
    svc.startSimulation();
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // GetStatus via RunLoop-driven server
    std::vector<uint8_t> req, resp;
    int rc = client.call(MotorController::kServiceId, MotorController::kGetStatus, req, &resp);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(resp.size(), sizeof(MotorStatus));

    MotorStatus status;
    std::memcpy(&status, resp.data(), sizeof(status));
    EXPECT_EQ(status.state, Idle);

    client.disconnect();
    svc.stop();
    svc.stopSimulation();
}
