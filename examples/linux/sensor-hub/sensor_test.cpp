// SensorHub integration tests — exercises the simulated sensor device
// via generated typed client/server over shared-memory IPC.

#include <gtest/gtest.h>
#include "SensorHub.h"
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
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

// ── Test device — deterministic sensor values ───────────────────────

class TestSensorDevice : public SensorHub
{
public:
    using SensorHub::SensorHub;

    // Expose notify for testing.
    using SensorHub::notifyDataReady;
    using SensorHub::notifyMotionDetected;

    void setAccel(float x, float y, float z)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_accel = {x, y, z};
    }

    void setGyro(float x, float y, float z)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_gyro = {x, y, z};
    }

    void setEnvironment(float tempC, float pressHpa, float humPct)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_env = {tempC, pressHpa, humPct};
    }

    void startDataThread()
    {
        m_dataRunning = true;
        m_dataThread = std::thread([this] { dataLoop(); });
    }

    void stopDataThread()
    {
        m_dataRunning = false;
        if (m_dataThread.joinable())
        {
            m_dataThread.join();
        }
    }

protected:
    int handleGetAccelerometer(AccelData *accel) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *accel = m_accel;
        return IPC_SUCCESS;
    }

    int handleGetGyroscope(GyroData *gyro) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *gyro = m_gyro;
        return IPC_SUCCESS;
    }

    int handleGetEnvironment(EnvironmentData *env) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *env = m_env;
        return IPC_SUCCESS;
    }

    int handleSetSampleRate(SampleRate rate) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sampleRate = rate;
        return IPC_SUCCESS;
    }

    int handleCalibrate(uint8_t *success) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_calibrated = true;
        *success = 1;
        return IPC_SUCCESS;
    }

private:
    void dataLoop()
    {
        while (m_dataRunning)
        {
            AccelData accel;
            GyroData gyro;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                accel = m_accel;
                gyro = m_gyro;
            }

            notifyDataReady(accel, gyro);

            float mag = std::sqrt(accel.x * accel.x +
                                  accel.y * accel.y +
                                  accel.z * accel.z);
            if (std::fabs(mag - 9.81f) > 2.0f)
            {
                notifyMotionDetected(mag);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    std::mutex m_mutex;
    AccelData m_accel{0.0f, 0.0f, 9.81f};
    GyroData m_gyro{0.0f, 0.0f, 0.0f};
    EnvironmentData m_env{22.0f, 1013.25f, 45.0f};
    SampleRate m_sampleRate = Rate_10Hz;
    bool m_calibrated = false;
    std::atomic<bool> m_dataRunning{false};
    std::thread m_dataThread;
};

// ── Notification-capturing client ───────────────────────────────────

class NotifyClient : public ClientBase
{
public:
    using ClientBase::ClientBase;

    std::mutex mtx;
    std::condition_variable cv;

    AccelData lastAccel{};
    GyroData lastGyro{};
    float lastMotionMag = 0;
    std::atomic<int> dataReadyCount{0};
    std::atomic<int> motionCount{0};

    bool waitForDataReady(int count, int timeoutMs = 2000)
    {
        std::unique_lock<std::mutex> lock(mtx);
        return cv.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                           [&] { return dataReadyCount.load() >= count; });
    }

    bool waitForMotion(int count, int timeoutMs = 2000)
    {
        std::unique_lock<std::mutex> lock(mtx);
        return cv.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                           [&] { return motionCount.load() >= count; });
    }

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        if (serviceId != SensorHub::kServiceId)
        {
            return;
        }

        if (messageId == SensorHub::kDataReady)
        {
            if (payload.size() < sizeof(AccelData) + sizeof(GyroData))
            {
                return;
            }
            std::lock_guard<std::mutex> lock(mtx);
            std::memcpy(&lastAccel, payload.data(), sizeof(lastAccel));
            std::memcpy(&lastGyro, payload.data() + sizeof(AccelData), sizeof(lastGyro));
            dataReadyCount++;
            cv.notify_all();
        }
        else if (messageId == SensorHub::kMotionDetected)
        {
            if (payload.size() < sizeof(float))
            {
                return;
            }
            std::lock_guard<std::mutex> lock(mtx);
            std::memcpy(&lastMotionMag, payload.data(), sizeof(lastMotionMag));
            motionCount++;
            cv.notify_all();
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
// GetAccelerometer — returns finite float data
// ═════════════════════════════════════════════════════════════════════

TEST(SensorHubTest, GetAccelerometer_ReturnsData)
{
    TestSensorDevice svc(SVC_NAME);
    svc.setAccel(0.01f, -0.05f, 9.83f);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(SensorHub::kServiceId, SensorHub::kGetAccelerometer,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(AccelData));

    AccelData accel;
    std::memcpy(&accel, response.data(), sizeof(accel));
    EXPECT_TRUE(std::isfinite(accel.x));
    EXPECT_TRUE(std::isfinite(accel.y));
    EXPECT_TRUE(std::isfinite(accel.z));
    EXPECT_NEAR(accel.z, 9.83f, 0.001f);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetGyroscope — returns finite float data
// ═════════════════════════════════════════════════════════════════════

TEST(SensorHubTest, GetGyroscope_ReturnsData)
{
    TestSensorDevice svc(SVC_NAME);
    svc.setGyro(0.1f, -0.2f, 0.05f);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(SensorHub::kServiceId, SensorHub::kGetGyroscope,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(GyroData));

    GyroData gyro;
    std::memcpy(&gyro, response.data(), sizeof(gyro));
    EXPECT_TRUE(std::isfinite(gyro.x));
    EXPECT_TRUE(std::isfinite(gyro.y));
    EXPECT_TRUE(std::isfinite(gyro.z));
    EXPECT_NEAR(gyro.x, 0.1f, 0.001f);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetEnvironment — returns reasonable values
// ═════════════════════════════════════════════════════════════════════

TEST(SensorHubTest, GetEnvironment_ReturnsReasonableValues)
{
    TestSensorDevice svc(SVC_NAME);
    svc.setEnvironment(22.3f, 1013.1f, 44.8f);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(SensorHub::kServiceId, SensorHub::kGetEnvironment,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(EnvironmentData));

    EnvironmentData env;
    std::memcpy(&env, response.data(), sizeof(env));
    EXPECT_GT(env.temperatureC, -50.0f);
    EXPECT_LT(env.temperatureC, 100.0f);
    EXPECT_GT(env.pressureHpa, 800.0f);
    EXPECT_LT(env.pressureHpa, 1200.0f);
    EXPECT_GE(env.humidityPercent, 0.0f);
    EXPECT_LE(env.humidityPercent, 100.0f);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// SetSampleRate — returns success for valid rate
// ═════════════════════════════════════════════════════════════════════

TEST(SensorHubTest, SetSampleRate_Accepted)
{
    TestSensorDevice svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    SampleRate rate = Rate_50Hz;
    std::vector<uint8_t> request(sizeof(rate));
    std::memcpy(request.data(), &rate, sizeof(rate));

    std::vector<uint8_t> response;
    int rc = client.call(SensorHub::kServiceId, SensorHub::kSetSampleRate,
                         request, &response);
    EXPECT_EQ(rc, IPC_SUCCESS);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Calibrate — returns success=1
// ═════════════════════════════════════════════════════════════════════

TEST(SensorHubTest, Calibrate_ReturnsSuccess)
{
    TestSensorDevice svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(SensorHub::kServiceId, SensorHub::kCalibrate,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint8_t));

    uint8_t success;
    std::memcpy(&success, response.data(), sizeof(success));
    EXPECT_EQ(success, 1u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// DataReady — notification fires within 2 seconds
// ═════════════════════════════════════════════════════════════════════

TEST(SensorHubTest, DataReady_NotificationFires)
{
    TestSensorDevice svc(SVC_NAME);
    svc.setAccel(0.0f, 0.0f, 9.81f);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    svc.startDataThread();

    ASSERT_TRUE(client.waitForDataReady(1));

    {
        std::lock_guard<std::mutex> lock(client.mtx);
        EXPECT_TRUE(std::isfinite(client.lastAccel.z));
        EXPECT_NEAR(client.lastAccel.z, 9.81f, 0.01f);
    }

    svc.stopDataThread();
    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// MotionDetected — fires when acceleration deviates from gravity
// ═════════════════════════════════════════════════════════════════════

TEST(SensorHubTest, MotionDetected_FiresOnLargeAcceleration)
{
    TestSensorDevice svc(SVC_NAME);
    svc.setAccel(0.0f, 0.0f, 20.0f);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    svc.startDataThread();

    ASSERT_TRUE(client.waitForMotion(1));

    {
        std::lock_guard<std::mutex> lock(client.mtx);
        EXPECT_GT(client.lastMotionMag, 11.0f);
    }

    svc.stopDataThread();
    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop — basic method call works in RunLoop mode
// ═════════════════════════════════════════════════════════════════════

TEST(SensorHubTest, RunLoop_Dispatch)
{
    ms::RunLoop loop;
    loop.init("SrvRL");

    TestSensorDevice svc(SVC_NAME, &loop);
    svc.setAccel(1.0f, 2.0f, 3.0f);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(SensorHub::kServiceId, SensorHub::kGetAccelerometer,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(AccelData));

    AccelData accel;
    std::memcpy(&accel, response.data(), sizeof(accel));
    EXPECT_NEAR(accel.x, 1.0f, 0.001f);
    EXPECT_NEAR(accel.y, 2.0f, 0.001f);
    EXPECT_NEAR(accel.z, 3.0f, 0.001f);

    client.disconnect();
    svc.stop();
}
