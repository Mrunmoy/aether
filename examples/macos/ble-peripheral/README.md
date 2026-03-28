# BLE Heart Rate Monitor (Simulated)

A simulated Bluetooth Low Energy (BLE) Heart Rate Monitor peripheral using Aether IPC. Demonstrates the GATT service model over shared-memory IPC with IDL-generated code.

## GATT Profile

| Service                | UUID   | Characteristics                                                    |
|------------------------|--------|--------------------------------------------------------------------|
| Heart Rate Service     | 0x180D | Heart Rate Measurement (0x2A37) R/N, Body Sensor Location (0x2A38) R |
| Battery Service        | 0x180F | Battery Level (0x2A19) R/N                                         |

**R** = Read, **N** = Notify

## IDL

The service interface is defined in [`BlePeripheral.idl`](BlePeripheral.idl) and generates typed server/client code under `gen/`.

## Build & Run

```bash
# From repository root
cmake -B build -DAETHER_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)

# Terminal 1 — start the device
./build/examples/macos/ble-peripheral/ble_device

# Terminal 2 — connect the dashboard
./build/examples/macos/ble-peripheral/ble_dashboard
```

## Dashboard Commands

```
> services                      — rediscover services
> read <svc_idx> <char_idx>     — read a characteristic value
> subscribe <svc_idx> <char_idx>   — enable notifications
> unsubscribe <svc_idx> <char_idx> — disable notifications
> quit                          — exit
```

## Sample Output

```
Connected to BLE peripheral.

Discovered 2 services:
  [1] Heart Rate Service (0x180D) — 2 characteristics
  [2] Battery Service (0x180F) — 1 characteristic

> read 1 0
  Heart Rate Measurement (0x2A37): 72 BPM [props=0x12, len=1]
> subscribe 1 0
  Enabled notifications for char 0 of service 1

  [notify] Heart Rate Measurement (0x2A37): 85 BPM
  [notify] Heart Rate Measurement (0x2A37): 63 BPM
> read 2 0
  Battery Level (0x2A19): 100% [props=0x12, len=1]
```

## Tests

```bash
ctest --test-dir build --output-on-failure -R BlePeripheral
```

Tests verify service discovery, characteristic read/write, notification enable/disable, and error handling — all without the simulation thread.
