#include "DeviceMonitor.h"

#include <array>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <thread>

namespace
{

std::atomic<bool> g_running{true};

void handleSignal(int)
{
    g_running.store(false);
}

class DeviceMonitorService : public aether::ipc::DeviceMonitor
{
public:
    using DeviceMonitor::DeviceMonitor;

    DeviceMonitorService()
        : DeviceMonitor("device_monitor")
    {
        std::strncpy(m_devices[0].name, "USB Audio Interface", sizeof(m_devices[0].name) - 1);
        std::strncpy(m_devices[1].name, "BLE Sensor Tag", sizeof(m_devices[1].name) - 1);
    }

    void publishDemoNotifications()
    {
        notifyDeviceConnected(m_devices[0]);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        notifyDeviceDisconnected(m_devices[1].id);
    }

protected:
    int handleGetDeviceCount(uint32_t *count) override
    {
        *count = static_cast<uint32_t>(m_devices.size());
        std::printf("[server] GetDeviceCount -> %u\n", *count);
        return aether::ipc::IPC_SUCCESS;
    }

    int handleGetDeviceInfo(uint32_t deviceId, aether::ipc::DeviceInfo *info) override
    {
        if (deviceId >= m_devices.size())
            return aether::ipc::IPC_ERR_INVALID_ARGUMENT;

        *info = m_devices[deviceId];
        std::printf("[server] GetDeviceInfo(%u) -> %s\n", deviceId, info->name);
        return aether::ipc::IPC_SUCCESS;
    }

private:
    std::array<aether::ipc::DeviceInfo, 2> m_devices{{
        {0, aether::ipc::USB, 0x1234, 0x0001, {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x01}, ""},
        {1, aether::ipc::Bluetooth, 0x4321, 0x0002, {0x10, 0x20, 0x30, 0x40, 0x50, 0x60}, ""},
    }};
};

} // namespace

int main()
{
    std::signal(SIGINT, handleSignal);
#if defined(SIGTERM)
    std::signal(SIGTERM, handleSignal);
#endif

    DeviceMonitorService service;
    if (!service.start())
    {
        std::fprintf(stderr, "[server] Failed to start device_monitor\n");
        return 1;
    }

    std::printf("[server] device_monitor is running. Press Ctrl+C to stop.\n");

    std::thread notifier([&service] {
        while (g_running.load() && service.isRunning())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (!g_running.load() || !service.isRunning())
                break;
            service.publishDemoNotifications();
        }
    });

    while (g_running.load() && service.isRunning())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    service.stop();
    if (notifier.joinable())
        notifier.join();

    std::printf("[server] stopped.\n");
    return 0;
}
