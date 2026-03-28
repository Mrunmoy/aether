# Battery Monitor Example

A simulated battery/power monitor built on the Aether IPC framework. The device
server simulates a battery draining and charging cycle — no platform-specific
APIs are required, so it compiles and runs on **any OS** (Linux, macOS, Windows).

## IDL Overview

The `BatteryMonitor` service exposes:

| Method             | Description                           |
|--------------------|---------------------------------------|
| `GetBatteryStatus` | Returns full `BatteryStatus` struct   |
| `GetPowerSource`   | Returns current `PowerSource` enum    |
| `GetBatteryHealth` | Returns health percentage and cycles  |

Notifications:
- **`BatteryLevelChanged`** — fired when charge percent or charge state changes
- **`PowerSourceChanged`** — fired when the power source changes

## Building

From the repository root:

```bash
cmake -B build -DAETHER_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

## Running

```bash
# Terminal 1 — start the simulated device
./build/examples/macos/battery-monitor/battery_device

# Terminal 2 — connect the CLI client
./build/examples/macos/battery-monitor/battery_client
```

### Sample Output

```
  Battery: 78% [Discharging] ████████░░ 3.92V
  Power:   Battery | Health: 92% (500 cycles)
  Time remaining: 78 min

  ⚡ Alert: Battery now 20% [Charging]
  🔌 Alert: Power source changed to AC
```

## Tests

```bash
ctest --test-dir build --output-on-failure -R BatteryMonitor
```

The test suite covers all three RPC methods, both notification types, and
RunLoop-based dispatch.
