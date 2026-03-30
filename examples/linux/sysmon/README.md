# SysMon — Linux System Monitor

Query live CPU, memory, and load data from `/proc`, then broadcast threshold
alerts to connected clients.

## What You'll Learn
- how to wrap a real Linux subsystem in a generated Aether service
- how to combine request/response monitoring calls with alert notifications
- how to keep a monitoring client simple while the server owns threshold logic

## Prerequisites
- Linux
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `SysMon.idl` | CPU, memory, and load-average contract |
| `sysmon_device.cpp` | `/proc` reader and alert logic |
| `sysmon_client.cpp` | reporting client that prints the current snapshot |
| `sysmon_test.cpp` | example-level regression coverage |

## Step 1: Read the IDL
`SysMon.idl` defines:
- `CpuUsage`, `MemoryInfo`, and `LoadAverage` structs
- methods to fetch each current snapshot
- `HighCpuAlert` and `LowMemoryAlert` notifications for threshold breaches

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/linux/sysmon/SysMon.idl --outdir examples/linux/sysmon/gen
```

## Step 3: Review the User Code
- `sysmon_device.cpp` reads `/proc`, calculates percentages, and emits alert
  notifications when its thresholds are exceeded.
- `sysmon_client.cpp` queries the current snapshot and prints the result.
- Aether handles all typed marshaling and callback delivery.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/linux/sysmon/sysmon_device

# Terminal 2
./build/examples/linux/sysmon/sysmon_client
```

## Expected Output
Server:

```text
SysMon service running
CPU threshold: 90%
Memory threshold: 85%
```

Client:

```text
Connected to sysmon service.
CPU Usage
Memory
Load Average
```

If a threshold is breached, the client prints the corresponding alert.

## What Just Happened
The server turned live Linux system data into a typed RPC surface and layered
policy on top by deciding when to emit alerts. That split is useful in real
systems: clients stay simple, while the server owns the monitoring logic.

## Testing
Run from the repository root (requires a build with `-e`):

```bash
ctest --test-dir build --output-on-failure -R sysmon_tests
```

## What To Modify Next
- adjust the CPU or memory thresholds in the device and trigger alerts deliberately
- add another `/proc`-backed metric such as disk I/O or network throughput

## Testing
Run from the repository root:

```bash
ctest --test-dir build --output-on-failure -R sysmon_tests
```

## Related Examples
- [`../file-watcher/`](../file-watcher/) for another Linux-specific service
- [`../sensor-hub/`](../sensor-hub/) for a simulated monitoring-style service with notifications
