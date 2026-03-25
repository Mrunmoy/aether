#include "SerialTransport.h"
#include "TransportClientBase.h"
#include "sensor_device.h"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <memory>
#include <thread>

#if defined(__APPLE__)
#include <util.h>
#else
#include <pty.h>
#endif
#include <termios.h>
#include <unistd.h>

static constexpr uint32_t kSensorServiceId = 0xA3B7C901u;
static constexpr uint32_t kMethodGetTemperature = 1;
static constexpr uint32_t kMethodGetHumidity = 2;
static constexpr uint32_t kMethodGetDeviceId = 3;

int main()
{
    // Create PTY pair.
    int master_fd = -1;
    int slave_fd = -1;

    if (openpty(&master_fd, &slave_fd, nullptr, nullptr, nullptr) != 0)
    {
        std::fprintf(stderr, "openpty() failed\n");
        return 1;
    }

    // Set PTY to raw mode so binary data passes through unmodified.
    struct termios raw{};
    tcgetattr(master_fd, &raw);
    cfmakeraw(&raw);
    tcsetattr(master_fd, TCSANOW, &raw);

    tcgetattr(slave_fd, &raw);
    cfmakeraw(&raw);
    tcsetattr(slave_fd, TCSANOW, &raw);

    std::printf("=== Serial Sensor Example ===\n");
    std::printf("PTY master=%d, slave=%d\n\n", master_fd, slave_fd);

    // Spawn device thread.
    // std::atomic<int> avoids the data-race UB that volatile int would have.
    // The C HAL interface expects `const volatile int *`, which is layout-
    // compatible with std::atomic<int> on every platform we target.
    std::atomic<int> stop_flag{0};
    std::thread device_thread([slave_fd, &stop_flag]() {
        sensor_device_run(slave_fd,
                          reinterpret_cast<const volatile int *>(&stop_flag));
    });

    // Small delay so the device can init.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Create transport and client.
    auto transport = std::make_unique<aether::ipc::SerialTransport>(master_fd);
    aether::ipc::TransportClientBase client("serial-sensor");

    if (!client.connect(std::move(transport)))
    {
        std::fprintf(stderr, "Failed to connect transport\n");
        stop_flag.store(1, std::memory_order_release);
        device_thread.join();
        close(master_fd);
        close(slave_fd);
        return 1;
    }

    std::printf("Connected to sensor device.\n\n");

    // Call GetTemperature 3 times.
    for (int i = 0; i < 3; i++)
    {
        std::vector<uint8_t> response;
        int rc = client.call(kSensorServiceId, kMethodGetTemperature,
                             {}, &response);
        if (rc == aether::ipc::IPC_SUCCESS && response.size() >= sizeof(float))
        {
            float temp;
            std::memcpy(&temp, response.data(), sizeof(float));
            std::printf("GetTemperature[%d]: %.2f C\n", i + 1, temp);
        }
        else
        {
            std::fprintf(stderr, "GetTemperature[%d] failed: rc=%d\n", i + 1, rc);
        }
    }

    std::printf("\n");

    // Call GetHumidity once.
    {
        std::vector<uint8_t> response;
        int rc = client.call(kSensorServiceId, kMethodGetHumidity,
                             {}, &response);
        if (rc == aether::ipc::IPC_SUCCESS && response.size() >= sizeof(float))
        {
            float hum;
            std::memcpy(&hum, response.data(), sizeof(float));
            std::printf("GetHumidity: %.2f %%\n", hum);
        }
        else
        {
            std::fprintf(stderr, "GetHumidity failed: rc=%d\n", rc);
        }
    }

    std::printf("\n");

    // Call GetDeviceId once.
    {
        std::vector<uint8_t> response;
        int rc = client.call(kSensorServiceId, kMethodGetDeviceId,
                             {}, &response);
        if (rc == aether::ipc::IPC_SUCCESS && !response.empty())
        {
            std::printf("GetDeviceId: %.*s\n", static_cast<int>(response.size()),
                        reinterpret_cast<const char *>(response.data()));
        }
        else
        {
            std::fprintf(stderr, "GetDeviceId failed: rc=%d\n", rc);
        }
    }

    std::printf("\n=== Done ===\n");

    // Disconnect and stop device.
    client.disconnect();
    stop_flag.store(1, std::memory_order_release);
    device_thread.join();

    close(master_fd);
    close(slave_fd);

    return 0;
}
