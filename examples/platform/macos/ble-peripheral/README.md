# BLE Peripheral Example

Use a simulated BLE-style peripheral to see how Aether can model hierarchical
service and characteristic interactions with typed RPC calls.

## What You'll Learn
- how to encode a GATT-like model in IDL structs and methods
- how to represent reads, writes, and subscriptions in a generated client API
- how to build a dashboard around a more nested domain model

## Prerequisites
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `BlePeripheral.idl` | service/characteristic data model and API |
| `ble_device.cpp` | simulated BLE database and notification source |
| `ble_dashboard.cpp` | interactive client for discovery, reads, and subscriptions |
| `ble_test.cpp` | example-level tests |

## Step 1: Read the IDL
`BlePeripheral.idl` defines:
- `BleUuid`, `BleCharacteristic`, and `BleService`
- methods to enumerate services, read characteristics, write values, and enable notifications
- one notification, `CharacteristicChanged`, for subscribed updates

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/platform/macos/ble-peripheral/BlePeripheral.idl --outdir examples/platform/macos/ble-peripheral/gen
```

## Step 3: Review the User Code
- `ble_device.cpp` owns the simulated BLE services, values, and update loop.
- `ble_dashboard.cpp` wraps the generated client in a REPL for discovery,
  reads, writes, and subscriptions.
- Aether handles the transport and typed marshaling; the example focuses on the
  application model.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/platform/macos/ble-peripheral/ble_device

# Terminal 2
./build/examples/platform/macos/ble-peripheral/ble_dashboard
```

### Dashboard Commands

| Command | Description |
|---------|-------------|
| `services` | Rediscover and list all services |
| `read <svc_idx> <char_idx>` | Read a characteristic value |
| `subscribe <svc_idx> <char_idx>` | Enable notifications for a characteristic |
| `unsubscribe <svc_idx> <char_idx>` | Disable notifications |
| `quit` | Exit |

## Expected Output
Client session:

```text
Connected to BLE peripheral.
Discovered 2 services
> read 1 0
Heart Rate Measurement: 72 BPM
> subscribe 1 0
[notify] Heart Rate Measurement: 85 BPM
```

## What Just Happened
This example uses generated bindings for a more nested data model than the
basic examples. The client still sees ordinary typed methods and notifications,
but the IDL captures the hierarchy of service UUIDs, characteristics, and
subscription-driven updates.

## Testing
Run from the repository root (requires a build with `-e`):

```bash
ctest --test-dir build --output-on-failure -R ble_tests
```

The suite verifies service discovery, characteristic read/write, notification enable/disable, and error handling.

## What To Modify Next
- add another characteristic and expose it through the dashboard
- add client-side pretty-printing for UUIDs instead of index-based navigation

## Testing
Run from the repository root:

```bash
ctest --test-dir build --output-on-failure -R ble_tests
```

## Related Examples
- [`../audio-dsp/`](../audio-dsp/) for another stateful dashboard-style service
- [`../../echo/`](../../echo/) for the simpler generated client/server path
