// BLE Central CLI — connects to the simulated BLE peripheral and provides
// an interactive dashboard for reading characteristics and subscribing
// to notifications.
//
// Run:   ./ble_device       (in another terminal)
//        ./ble_dashboard

#include "BlePeripheral.h"

#include <atomic>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

using namespace aether::ipc;

// ── BLE UUID helpers ────────────────────────────────────────────────

static BleUuid makeBleUuid16(uint16_t uuid16)
{
    BleUuid uuid{};
    uuid.bytes[2] = static_cast<uint8_t>(uuid16 >> 8);
    uuid.bytes[3] = static_cast<uint8_t>(uuid16 & 0xFF);
    uuid.bytes[4] = 0x00;
    uuid.bytes[5] = 0x00;
    uuid.bytes[6] = 0x10;
    uuid.bytes[7] = 0x00;
    uuid.bytes[8] = 0x80;
    uuid.bytes[9] = 0x00;
    uuid.bytes[10] = 0x00;
    uuid.bytes[11] = 0x80;
    uuid.bytes[12] = 0x5F;
    uuid.bytes[13] = 0x9B;
    uuid.bytes[14] = 0x34;
    uuid.bytes[15] = 0xFB;
    return uuid;
}

static uint16_t extractUuid16(const BleUuid &uuid)
{
    return static_cast<uint16_t>((uuid.bytes[2] << 8) | uuid.bytes[3]);
}

static const char *knownServiceName(uint16_t uuid16)
{
    switch (uuid16)
    {
    case 0x180D: return "Heart Rate Service";
    case 0x180F: return "Battery Service";
    default:     return "Unknown Service";
    }
}

static const char *knownCharName(uint16_t uuid16)
{
    switch (uuid16)
    {
    case 0x2A37: return "Heart Rate Measurement";
    case 0x2A38: return "Body Sensor Location";
    case 0x2A19: return "Battery Level";
    default:     return "Unknown Characteristic";
    }
}

// ── Notification-aware client ───────────────────────────────────────

class Dashboard : public BlePeripheral
{
public:
    using BlePeripheral::BlePeripheral;

    struct ServiceInfo
    {
        BleUuid uuid;
        uint8_t charCount;
    };

    std::vector<ServiceInfo> services;

protected:
    void onCharacteristicChanged(BleUuid serviceUuid, BleUuid charUuid,
                                 std::array<uint8_t, 20> value, uint8_t length) override
    {
        uint16_t svcId = extractUuid16(serviceUuid);
        uint16_t charId = extractUuid16(charUuid);
        std::printf("\n  [notify] %s (0x%04X): ", knownCharName(charId), charId);
        if (charId == 0x2A37)
            std::printf("%d BPM", value[0]);
        else if (charId == 0x2A19)
            std::printf("%d%%", value[0]);
        else
        {
            for (uint8_t i = 0; i < length; i++)
                std::printf("%02X ", value[i]);
        }
        (void)svcId;
        std::printf("\n> ");
        std::fflush(stdout);
    }
};

// ── Helper: read characteristic by service/char index ───────────────

static BleUuid getCharUuid(Dashboard &client, uint8_t svcIdx, const BleUuid &svcUuid, uint8_t charIdx)
{
    // We enumerate characteristics by reading them with well-known UUIDs
    // based on the service. For the demo we map indices to known UUIDs.
    uint16_t svcId = extractUuid16(svcUuid);
    if (svcId == 0x180D)
    {
        if (charIdx == 0) return makeBleUuid16(0x2A37);
        if (charIdx == 1) return makeBleUuid16(0x2A38);
    }
    else if (svcId == 0x180F)
    {
        if (charIdx == 0) return makeBleUuid16(0x2A19);
    }
    (void)client;
    (void)svcIdx;
    return BleUuid{};
}

static void printUsage()
{
    std::printf("Commands:\n");
    std::printf("  services                      — list discovered services\n");
    std::printf("  read <svc_idx> <char_idx>     — read a characteristic\n");
    std::printf("  subscribe <svc_idx> <char_idx>   — enable notifications\n");
    std::printf("  unsubscribe <svc_idx> <char_idx> — disable notifications\n");
    std::printf("  quit                          — exit\n");
}

static void discoverServices(Dashboard &client)
{
    uint8_t count = 0;
    int rc = client.GetServiceCount(&count);
    if (rc != IPC_SUCCESS)
    {
        std::printf("  Error getting service count: %d\n", rc);
        return;
    }

    std::printf("Discovered %d services:\n", count);
    client.services.clear();
    for (uint8_t i = 0; i < count; i++)
    {
        BleService svc{};
        rc = client.GetService(i, &svc);
        if (rc != IPC_SUCCESS)
        {
            std::printf("  [%d] Error: %d\n", i + 1, rc);
            continue;
        }
        uint16_t uuid16 = extractUuid16(svc.uuid);
        std::printf("  [%d] %s (0x%04X) — %d characteristic%s\n",
                    i + 1, knownServiceName(uuid16), uuid16,
                    svc.characteristicCount,
                    svc.characteristicCount == 1 ? "" : "s");
        client.services.push_back({svc.uuid, svc.characteristicCount});
    }
}

int main()
{
    Dashboard client("BlePeripheral");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to BLE peripheral\n");
        return 1;
    }
    std::printf("Connected to BLE peripheral.\n\n");

    discoverServices(client);
    std::printf("\n");
    printUsage();

    std::string line;
    std::printf("> ");
    std::fflush(stdout);

    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "quit" || cmd == "q")
            break;

        if (cmd == "services")
        {
            discoverServices(client);
        }
        else if (cmd == "read")
        {
            int svcIdx = 0, charIdx = 0;
            if (!(iss >> svcIdx >> charIdx) || svcIdx < 1 ||
                svcIdx > static_cast<int>(client.services.size()))
            {
                std::printf("  Usage: read <svc_idx> <char_idx>\n");
            }
            else
            {
                auto &svc = client.services[static_cast<size_t>(svcIdx - 1)];
                BleUuid charUuid = getCharUuid(client, static_cast<uint8_t>(svcIdx - 1),
                                               svc.uuid, static_cast<uint8_t>(charIdx));
                BleCharacteristic ch{};
                int rc = client.ReadCharacteristic(svc.uuid, charUuid, &ch);
                if (rc != IPC_SUCCESS)
                {
                    std::printf("  Read error: %d\n", rc);
                }
                else
                {
                    uint16_t cid = extractUuid16(ch.uuid);
                    std::printf("  %s (0x%04X): ", knownCharName(cid), cid);
                    if (cid == 0x2A37)
                        std::printf("%d BPM", ch.value[0]);
                    else if (cid == 0x2A19)
                        std::printf("%d%%", ch.value[0]);
                    else if (cid == 0x2A38)
                    {
                        const char *locations[] = {"Other", "Chest", "Wrist", "Finger", "Hand",
                                                    "Ear Lobe", "Foot"};
                        int loc = ch.value[0];
                        if (loc >= 0 && loc < 7)
                            std::printf("%s", locations[loc]);
                        else
                            std::printf("Unknown (%d)", loc);
                    }
                    else
                    {
                        for (uint8_t i = 0; i < ch.valueLength; i++)
                            std::printf("%02X ", ch.value[i]);
                    }
                    std::printf(" [props=0x%02X, len=%d]\n", ch.properties, ch.valueLength);
                }
            }
        }
        else if (cmd == "subscribe" || cmd == "unsubscribe")
        {
            int svcIdx = 0, charIdx = 0;
            if (!(iss >> svcIdx >> charIdx) || svcIdx < 1 ||
                svcIdx > static_cast<int>(client.services.size()))
            {
                std::printf("  Usage: %s <svc_idx> <char_idx>\n", cmd.c_str());
            }
            else
            {
                auto &svc = client.services[static_cast<size_t>(svcIdx - 1)];
                BleUuid charUuid = getCharUuid(client, static_cast<uint8_t>(svcIdx - 1),
                                               svc.uuid, static_cast<uint8_t>(charIdx));
                uint8_t enable = (cmd == "subscribe") ? 1 : 0;
                int rc = client.EnableNotifications(svc.uuid, charUuid, enable);
                if (rc != IPC_SUCCESS)
                    std::printf("  Error: %d\n", rc);
                else
                    std::printf("  %s notifications for char %d of service %d\n",
                                enable ? "Enabled" : "Disabled", charIdx, svcIdx);
            }
        }
        else if (!cmd.empty())
        {
            printUsage();
        }

        std::printf("> ");
        std::fflush(stdout);
    }

    client.disconnect();
    std::printf("Disconnected.\n");
    return 0;
}
