# Process Monitor Example

Simulates a Windows process list monitor over Aether IPC. The server maintains a
list of simulated processes (System, explorer.exe, svchost.exe, etc.) and
periodically spawns or kills processes, firing `ProcessStarted` and
`ProcessExited` notifications.

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
./build/examples/windows/process-monitor/procmon_device
```

Connect the client in another:

```bash
./build/examples/windows/process-monitor/procmon_client
```

Client commands:

| Command      | Description                          |
| ------------ | ------------------------------------ |
| `list`       | List all processes with details      |
| `info <pid>` | Show details for a single process    |
| `quit`       | Disconnect and exit                  |

Process start/exit events are printed automatically when the server fires
notifications.

## Tests

```bash
ctest --test-dir build --output-on-failure -R ProcessMonitorTest
```
