# Serial Sensor Example

Demonstrates a realistic host-to-device IPC scenario where a host-side C++
client talks to a simulated embedded temperature/humidity sensor over a PTY
serial link.

- **Host side** uses `aether::ipc::TransportClientBase` with a custom
  `SerialTransport` that implements `ITransport` over a file descriptor.
- **Device side** uses aether-lite's C99 framing and dispatch to handle
  incoming requests and send responses.

## Architecture

```
  Host (C++)                       PTY pair                  Device (C99)
 ┌──────────────┐             ┌──────────────┐          ┌──────────────────┐
 │ TransportCli │  master fd  │              │ slave fd  │  al_init()       │
 │   entBase    │◄───────────►│   openpty()  │◄────────►│  al_poll() loop  │
 │ + Serial     │             │              │          │  sensor handlers │
 │   Transport  │             └──────────────┘          └──────────────────┘
 └──────────────┘
```

## Sensor Methods

| Method         | ID | Returns                              |
|----------------|----|--------------------------------------|
| GetTemperature | 1  | `float` (~22.5 + random variation)   |
| GetHumidity    | 2  | `float` (~45.0 + random variation)   |
| GetDeviceId    | 3  | String `"AETHER-SENSOR-001"`         |

## Building

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
      -DAETHER_BUILD_EXAMPLES=ON \
      -DAETHER_BUILD_LITE=ON
cmake --build build -j$(nproc) --target serial_sensor
./build/examples/serial-sensor/serial_sensor
```

Linux only (uses PTY via `openpty()`).
