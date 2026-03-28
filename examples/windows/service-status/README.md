# Service Status Example

Simulates a Windows service monitor over Aether IPC. The server maintains a list
of simulated Windows services (Spooler, BITS, Windows Update, etc.) and
periodically toggles their state, firing `ServiceStateChanged` notifications.

The device is fully simulated — no Windows APIs are used — so it compiles and
runs on any platform.

## Build

```bash
cmake -B build -DAETHER_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

## Run

Start the device (server) in one terminal:

```bash
./build/examples/windows/service-status/svcstat_device
```

Connect the client in another:

```bash
./build/examples/windows/service-status/svcstat_client
```

Client commands:

| Command          | Description                        |
| ---------------- | ---------------------------------- |
| `list`           | List all services with status      |
| `status <name>`  | Show details for a single service  |
| `quit`           | Disconnect and exit                |

State changes are printed automatically when the server fires a
`ServiceStateChanged` notification.

## Tests

```bash
ctest --test-dir build --output-on-failure -R ServiceStatusTest
```
