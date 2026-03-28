# SysMon — Linux System Monitor

A Linux system monitor IPC service that reads `/proc` for real-time CPU,
memory, and load average data. Demonstrates codegen-based RPC methods and
server-to-client threshold notifications.

## IDL Overview

**Methods:**
| Method | Description |
|--------|-------------|
| `GetCpuUsage` | Returns user/system/idle/iowait/total CPU percentages |
| `GetMemoryInfo` | Returns total/free/available/buffers/cached/used memory |
| `GetLoadAverage` | Returns 1/5/15-min load averages and process counts |

**Notifications:**
| Notification | Trigger |
|-------------|---------|
| `HighCpuAlert` | CPU usage exceeds 90% |
| `LowMemoryAlert` | Memory usage exceeds 85% |

## Building

```bash
# From repository root
python3 build.py -e                    # build all examples
# or
cmake -B build -DAETHER_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

## Running

```bash
# Terminal 1 — start the server
./build/examples/linux/sysmon/sysmon_device

# Terminal 2 — start the client
./build/examples/linux/sysmon/sysmon_client
```

## Running Tests

```bash
ctest --test-dir build --output-on-failure -R SysMon
```

## Sample Output

**Server:**
```
╔══════════════════════════════════════╗
║  SysMon service running              ║
║  Service name: sysmon                ║
║  CPU threshold:  90%                 ║
║  Memory threshold: 85%               ║
║  Ctrl-C to stop                      ║
╚══════════════════════════════════════╝
```

**Client:**
```
Connected to sysmon service.

┌──────────────────────────────────────┐
│ CPU Usage                            │
├──────────────────────────────────────┤
  User:            2.3 %
  System:          1.1 %
  IO Wait:         0.0 %
  Idle:           96.6 %
  Total:           3.4 %
├──────────────────────────────────────┤
│ Memory                               │
├──────────────────────────────────────┤
  Total:          15872.0 MB
  Used:            8234.0 MB
  Free:            2048.0 MB
  Available:       7638.0 MB
  Buffers:          512.0 MB
  Cached:          5078.0 MB
  Used:           51.9 %
├──────────────────────────────────────┤
│ Load Average                         │
├──────────────────────────────────────┤
  1 min:           0.45
  5 min:           0.38
  15 min:          0.42
  Procs:          3 / 412
└──────────────────────────────────────┘
```
