# Battery Monitor Example

Use a simulated battery and power-source model to see how Aether handles status
queries plus threshold and source-change notifications.

## What You'll Learn
- how to model health/status data as shared IDL structs
- how to publish change notifications without client polling
- how a small monitoring client can stay simple with typed bindings

## Prerequisites
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `BatteryMonitor.idl` | battery status, health, and notification contract |
| `battery_device.cpp` | simulated battery lifecycle and event generation |
| `battery_client.cpp` | CLI client that prints status and notifications |
| `battery_test.cpp` | example-level regression coverage |

## Step 1: Read the IDL
`BatteryMonitor.idl` defines:
- `BatteryStatus` plus enums for power source and charge state
- methods to fetch the current battery snapshot, source, and health metrics
- notifications for level changes and power-source transitions

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/macos/battery-monitor/BatteryMonitor.idl --outdir examples/macos/battery-monitor/gen
```

## Step 3: Review the User Code
- `battery_device.cpp` simulates charging, discharging, and source changes.
- `battery_client.cpp` connects to the generated client API and prints the
  current status plus incoming notifications.
- Aether handles connection management and typed message delivery.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/macos/battery-monitor/battery_device

# Terminal 2
./build/examples/macos/battery-monitor/battery_client
```

## Expected Output
Client output looks like:

```text
Battery: 78% [Discharging]
Power: Battery | Health: 92% (500 cycles)
Alert: Battery now 20% [Charging]
Alert: Power source changed to AC
```

## What Just Happened
The device side owned the simulation and decided when a meaningful state change
occurred. The client used typed methods for snapshots and notifications for
changes, which is the common pattern for any monitor-style service.

## What To Modify Next
- add a low-battery threshold method so clients can configure alert policy
- extend the client to log a full event history instead of printing one line at a time

## Related Examples
- [`../disk-usage/`](../disk-usage/) for another monitor-style service
- [`../../linux/sysmon/`](../../linux/sysmon/) for a Linux-backed real monitor rather than a simulation
