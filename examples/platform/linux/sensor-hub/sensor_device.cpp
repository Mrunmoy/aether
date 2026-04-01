// SensorHub device (server) — simulated multi-sensor fusion device.
//
// Generates accelerometer, gyroscope, and environment data with configurable
// noise, and broadcasts DataReady / MotionDetected notifications at the
// configured sample rate.

#include "SensorHub.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <mutex>
#include <random>
#include <thread>

using namespace aether::ipc;

static std::atomic<bool> g_running{true};

static void sigintHandler(int)
{
    g_running = false;
}

class SensorDevice : public SensorHub
{
public:
    using SensorHub::SensorHub;

    ~SensorDevice() override
    {
        stopDataThread();
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
        *accel = generateAccel();
        return IPC_SUCCESS;
    }

    int handleGetGyroscope(GyroData *gyro) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *gyro = generateGyro();
        return IPC_SUCCESS;
    }

    int handleGetEnvironment(EnvironmentData *env) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *env = generateEnvironment();
        return IPC_SUCCESS;
    }

    int handleSetSampleRate(SampleRate rate) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_sampleRate = rate;
        std::printf("[device] sample rate set to %u Hz\n",
                    static_cast<unsigned>(rate));
        return IPC_SUCCESS;
    }

    int handleCalibrate(uint8_t *success) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_calibrated = true;
        std::printf("[device] calibrated\n");
        *success = 1;
        return IPC_SUCCESS;
    }

private:
    AccelData generateAccel()
    {
        float scale = m_calibrated ? 0.05f : 0.1f;
        std::normal_distribution<float> noise(0.0f, scale);
        return {m_accelBase[0] + noise(m_rng),
                m_accelBase[1] + noise(m_rng),
                m_accelBase[2] + noise(m_rng)};
    }

    GyroData generateGyro()
    {
        float scale = m_calibrated ? 0.25f : 0.5f;
        std::normal_distribution<float> noise(0.0f, scale);
        return {m_gyroBase[0] + noise(m_rng),
                m_gyroBase[1] + noise(m_rng),
                m_gyroBase[2] + noise(m_rng)};
    }

    EnvironmentData generateEnvironment()
    {
        std::normal_distribution<float> tempNoise(0.0f, 0.5f);
        std::normal_distribution<float> pressNoise(0.0f, 0.5f);
        std::normal_distribution<float> humidNoise(0.0f, 2.0f);
        return {22.0f + tempNoise(m_rng),
                1013.25f + pressNoise(m_rng),
                45.0f + humidNoise(m_rng)};
    }

    void dataLoop()
    {
        while (m_dataRunning)
        {
            uint32_t rateHz;
            AccelData accel;
            GyroData gyro;

            {
                std::lock_guard<std::mutex> lock(m_mutex);
                rateHz = static_cast<uint32_t>(m_sampleRate);
                accel = generateAccel();
                gyro = generateGyro();
            }

            notifyDataReady(accel, gyro);

            float mag = std::sqrt(accel.x * accel.x +
                                  accel.y * accel.y +
                                  accel.z * accel.z);
            if (std::fabs(mag - 9.81f) > 2.0f)
            {
                notifyMotionDetected(mag);
            }

            auto interval = std::chrono::milliseconds(1000 / rateHz);
            std::this_thread::sleep_for(interval);
        }
    }

    std::mutex m_mutex;
    std::mt19937 m_rng{std::random_device{}()};
    SampleRate m_sampleRate = Rate_10Hz;
    bool m_calibrated = false;
    std::atomic<bool> m_dataRunning{false};
    std::thread m_dataThread;

    float m_accelBase[3] = {0.0f, 0.0f, 9.81f};
    float m_gyroBase[3] = {0.0f, 0.0f, 0.0f};
};

int main()
{
    std::signal(SIGINT, sigintHandler);

    std::printf("╔══════════════════════════════════╗\n");
    std::printf("║   Aether SensorHub Device        ║\n");
    std::printf("║   IMU + Environment Simulator    ║\n");
    std::printf("╚══════════════════════════════════╝\n\n");

    SensorDevice svc("SensorHub");
    if (!svc.start())
    {
        std::fprintf(stderr, "Failed to start service\n");
        return 1;
    }

    svc.startDataThread();
    std::printf("SensorHub device running (10 Hz default). Press Ctrl-C to stop.\n");

    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::printf("\nShutting down...\n");
    svc.stopDataThread();
    svc.stop();
    return 0;
}
