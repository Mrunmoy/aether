// Simulated stepper motor controller server.
//
// Implements trapezoidal motion profiles over a MotorController IPC service.
// The device is fully simulated — no hardware dependencies.
//
// Run:   ./motor_device
// Stop:  Ctrl-C

#include "MotorController.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <csignal>
#include <mutex>
#include <thread>

using namespace aether::ipc;

// ── Simulation constants ────────────────────────────────────────────

static constexpr int32_t kPositionMin   = -10000;  // Lower limit switch
static constexpr int32_t kPositionMax   =  10000;  // Upper limit switch
static constexpr float   kMaxVelocity   =  1000.0f; // steps/s
static constexpr float   kAcceleration  =   500.0f; // steps/s²
static constexpr float   kTickInterval  =  0.010f;  // 10 ms
static constexpr int     kStallTimeoutMs =   500;

static std::atomic<bool> g_running{true};

// ── Motor device implementation ─────────────────────────────────────

class MotorDevice : public MotorController
{
public:
    using MotorController::MotorController;

    ~MotorDevice() override
    {
        stopSimulation();
    }

    void startSimulation()
    {
        m_simRunning.store(true);
        m_simThread = std::thread(&MotorDevice::simulationLoop, this);
    }

    void stopSimulation()
    {
        m_simRunning.store(false);
        if (m_simThread.joinable())
            m_simThread.join();
    }

protected:
    int handleHome() override
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_state = Homing;
        m_targetVelocity = -kMaxVelocity * 0.5f;
        m_homing = true;
        m_homed = false;
        std::printf("[device] Homing started\n");
        return IPC_SUCCESS;
    }

    int handleMoveTo(int32_t position, float velocity) override
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        float vel = std::fabs(velocity);
        if (vel > kMaxVelocity)
            vel = kMaxVelocity;
        if (vel < 1.0f)
            vel = 1.0f;

        m_state = Moving;
        m_targetPosition = position;
        m_moveToActive = true;
        m_jogActive = false;
        // Direction toward target
        m_targetVelocity = (position >= m_position) ? vel : -vel;
        std::printf("[device] MoveTo position=%d velocity=%.1f\n", position, vel);
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
        std::printf("[device] Jog velocity=%.1f\n", vel);
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
        std::printf("[device] Stop\n");
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

        while (m_simRunning.load())
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(static_cast<int>(kTickInterval * 1000.0f)));

            auto now = Clock::now();
            float dt = std::chrono::duration<float>(now - lastTick).count();
            lastTick = now;

            std::lock_guard<std::mutex> lock(m_mtx);

            if (m_state != Moving && m_state != Homing)
                continue;

            int32_t prevPosition = m_position;

            // --- Velocity update (trapezoidal acceleration) ---
            if (m_moveToActive)
            {
                // Deceleration distance at current velocity
                float decelDist = (m_velocity * m_velocity) / (2.0f * kAcceleration);
                float remaining = static_cast<float>(m_targetPosition - m_position);
                float absRemaining = std::fabs(remaining);

                if (absRemaining <= 0.5f)
                {
                    // Arrived
                    m_position = m_targetPosition;
                    m_velocity = 0.0f;
                    m_moveToActive = false;
                    m_state = Idle;
                    notifyMotionComplete(m_position);
                    continue;
                }

                if (absRemaining <= decelDist)
                {
                    // Decelerate
                    float decel = kAcceleration * dt;
                    if (m_velocity > 0.0f)
                        m_velocity = std::max(0.0f, m_velocity - decel);
                    else
                        m_velocity = std::min(0.0f, m_velocity + decel);

                    // Snap to zero if very slow
                    if (std::fabs(m_velocity) < 1.0f && absRemaining < 2.0f)
                    {
                        m_position = m_targetPosition;
                        m_velocity = 0.0f;
                        m_moveToActive = false;
                        m_state = Idle;
                        notifyMotionComplete(m_position);
                        continue;
                    }
                }
                else
                {
                    // Accelerate toward target velocity
                    float accel = kAcceleration * dt;
                    if (m_velocity < m_targetVelocity)
                        m_velocity = std::min(m_velocity + accel, m_targetVelocity);
                    else if (m_velocity > m_targetVelocity)
                        m_velocity = std::max(m_velocity - accel, m_targetVelocity);
                }
            }
            else
            {
                // Jog or homing: ramp toward target velocity
                float accel = kAcceleration * dt;
                if (m_velocity < m_targetVelocity)
                    m_velocity = std::min(m_velocity + accel, m_targetVelocity);
                else if (m_velocity > m_targetVelocity)
                    m_velocity = std::max(m_velocity - accel, m_targetVelocity);
            }

            // --- Position update ---
            float posFloat = static_cast<float>(m_position) + m_velocity * dt;
            m_position = static_cast<int32_t>(std::round(posFloat));

            // --- Limit switch checks ---
            if (m_position <= kPositionMin)
            {
                m_position = kPositionMin;
                m_velocity = 0.0f;
                m_targetVelocity = 0.0f;
                m_moveToActive = false;
                m_jogActive = false;

                if (m_homing)
                {
                    // Home complete — set origin
                    m_position = 0;
                    m_homing = false;
                    m_homed = true;
                    m_state = Idle;
                    notifyMotionComplete(m_position);
                    continue;
                }

                m_state = Idle;
                notifyLimitHit(Lower, m_position);
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
                continue;
            }

            // --- Stall detection ---
            if (std::fabs(m_velocity) > 1.0f && m_position == prevPosition)
            {
                if (!m_stallTimerActive)
                {
                    m_stallTimerActive = true;
                    m_stallStart = std::chrono::steady_clock::now();
                }
                else
                {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - m_stallStart);
                    if (elapsed.count() >= kStallTimeoutMs)
                    {
                        m_state = Fault;
                        float savedVel = m_velocity;
                        m_velocity = 0.0f;
                        m_targetVelocity = 0.0f;
                        m_moveToActive = false;
                        m_jogActive = false;
                        m_homing = false;
                        m_stallTimerActive = false;
                        notifyStallDetected(m_position, savedVel);
                        continue;
                    }
                }
            }
            else
            {
                m_stallTimerActive = false;
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

    // Stall detection
    bool m_stallTimerActive = false;
    std::chrono::steady_clock::time_point m_stallStart;

    // Simulation thread
    std::atomic<bool> m_simRunning{false};
    std::thread m_simThread;
};

// ── main ────────────────────────────────────────────────────────────

int main()
{
#if !defined(_WIN32)
    std::signal(SIGINT, [](int) { g_running.store(false); });
#endif

    MotorDevice device("motor");
    if (!device.start())
    {
        std::fprintf(stderr, "Failed to start MotorController service\n");
        return 1;
    }
    device.startSimulation();

    std::printf("Motor controller running (Ctrl-C to stop)...\n");
    while (g_running.load())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    device.stopSimulation();
    device.stop();
    std::printf("Motor controller stopped.\n");
    return 0;
}
