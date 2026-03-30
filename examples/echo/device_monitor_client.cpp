#include "DeviceMonitor.h"

#include <chrono>
#include <cstdio>
#include <thread>

namespace
{

class DeviceMonitorClient : public aether::ipc::DeviceMonitor
{
public:
    using DeviceMonitor::DeviceMonitor;

protected:
    void onDeviceConnected(aether::ipc::DeviceInfo info) override
    {
        std::printf("[client] notification: connected -> %s\n", info.name);
    }

    void onDeviceDisconnected(uint32_t deviceId) override
    {
        std::printf("[client] notification: disconnected -> id=%u\n", deviceId);
    }
};

} // namespace

int main()
{
    DeviceMonitorClient client("device_monitor");
    if (!client.connect())
    {
        std::fprintf(stderr, "[client] Failed to connect to device_monitor\n");
        return 1;
    }

    uint32_t count = 0;
    int rc = client.GetDeviceCount(&count);
    if (rc != aether::ipc::IPC_SUCCESS)
    {
        std::fprintf(stderr, "[client] GetDeviceCount failed: %d\n", rc);
        client.disconnect();
        return 1;
    }

    std::printf("[client] device count: %u\n", count);
    for (uint32_t deviceId = 0; deviceId < count; ++deviceId)
    {
        aether::ipc::DeviceInfo info{};
        rc = client.GetDeviceInfo(deviceId, &info);
        if (rc != aether::ipc::IPC_SUCCESS)
        {
            std::fprintf(stderr, "[client] GetDeviceInfo(%u) failed: %d\n", deviceId, rc);
            client.disconnect();
            return 1;
        }

        std::printf("[client] device %u: %s (vendor=0x%04x product=0x%04x)\n",
                    info.id, info.name, info.vendorId, info.productId);
    }

    std::printf("[client] waiting briefly for notifications...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    client.disconnect();
    std::printf("[client] disconnected.\n");
    return 0;
}
