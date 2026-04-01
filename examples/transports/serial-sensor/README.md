# Serial Sensor Example

Run a host-side C++ client against a simulated embedded device that speaks
`aether-lite` over a PTY-backed serial link.

## What You'll Learn
- how a host Aether client can talk to a firmware-style `aether-lite` service
- how the serial handshake and framing layer fits between host and device code
- how to evolve from a transport demo into a realistic host/device flow

## Prerequisites
- Linux or macOS
- build from the repository root
- configure CMake with `-DAETHER_BUILD_LITE=ON` so the `aether_lite` target exists

## Files That Matter
| File | Why it matters |
|------|----------------|
| `serial_sensor.cpp` | host-side C++ program that opens the PTY, performs the handshake, and calls device methods |
| `sensor_device.c` | `aether-lite` device loop and request handlers |
| `sensor_device.h` | service and method IDs shared between the host and simulated device |
| `../../SerialTransport.h` | serial framing and handshake implementation used on the host |

## Step 1: Read the IDL
This example does not use the Python IDL generator.

Instead, it hard-codes a small service contract:
- service ID `0xA3B7C901`
- method `1`: `GetTemperature`
- method `2`: `GetHumidity`
- method `3`: `GetDeviceId`

That makes the host/device boundary explicit and keeps the focus on the
transport plus `aether-lite` runtime split.

## Step 2: Generate Code
No code generation step is required.

The host talks to the device using explicit service and method IDs, and the
device handlers in `sensor_device.c` serialize the results manually.

## Step 3: Review the User Code
- `serial_sensor.cpp` creates the PTY pair, configures raw mode, runs the
  device thread, and performs the host-side calls.
- `sensor_device.c` is the embedded side: it polls the byte stream, decodes
  requests, and writes back responses.
- `TransportClientBase` still provides sequencing and timeout handling on the
  host, while `aether-lite` provides the tiny C-side framing and dispatch loop.

## Build
Run from the repository root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DAETHER_BUILD_EXAMPLES=ON -DAETHER_BUILD_LITE=ON
cmake --build build -j"$(nproc)"
```

## Run
Run from the repository root:

```bash
./build/examples/transports/serial-sensor/serial_sensor
```

## Expected Output
You should see output similar to:

```text
=== Serial Sensor Example ===
Handshake complete
Connected to sensor device.
GetTemperature[1]: 22.51 C
GetHumidity: 44.87 %
GetDeviceId: AETHER-SENSOR-001
=== Done ===
```

## What Just Happened
The host C++ program and the simulated device shared a PTY pair instead of
real UART hardware. The host used `TransportClientBase` over `SerialTransport`,
while the device side ran the small `aether-lite` C loop. That lets you test
an embedded integration pattern entirely on a development machine.

## What To Modify Next
- add another sensor method to `sensor_device.c` and call it from the host
- replace the PTY pair with a real serial device once your transport is ready

## Related Examples
- [`../serial-loopback/`](../serial-loopback/) for the transport-only version of the same idea
- [`../mcu-firmware/`](../mcu-firmware/) for the next step toward real firmware integration
