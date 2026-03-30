# CAN Bus ECU Example

Model an automotive ECU with typed RPC access to PIDs and notifications for
diagnostic trouble codes.

## What You'll Learn
- how to use Aether for a richer protocol/domain model
- how to mix inventory data, live readings, and fault notifications
- how to build an interactive dashboard on top of generated bindings

## Prerequisites
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `CanBusEcu.idl` | PID, DTC, and vehicle info contract |
| `ecu_device.cpp` | simulated vehicle state machine and fault injection |
| `ecu_dashboard.cpp` | dashboard client for reading PIDs and DTCs |
| `ecu_test.cpp` | example-level tests |

## Step 1: Read the IDL
`CanBusEcu.idl` defines:
- `PidReading`, `DtcEntry`, and `VehicleInfo`
- methods to read a PID, enumerate DTCs, clear them, and fetch vehicle metadata
- notifications for `CheckEngine` and `DtcCleared`

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/windows/can-bus-ecu/CanBusEcu.idl --outdir examples/windows/can-bus-ecu/gen
```

## Step 3: Review the User Code
- `ecu_device.cpp` simulates the ECU state machine and injects trouble codes.
- `ecu_dashboard.cpp` wraps the generated client API in a CLI dashboard.
- Aether handles the typed client/server plumbing and notifications.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/windows/can-bus-ecu/ecu_device

# Terminal 2
./build/examples/windows/can-bus-ecu/ecu_dashboard
```

### Dashboard Commands

| Command | Description |
|---------|-------------|
| `read <pid_hex>` | Read a single PID (e.g. `read 0c` for RPM) |
| `dtc` | List stored Diagnostic Trouble Codes |
| `clear` | Clear all DTCs |
| `vin` | Show vehicle identification info |
| `dashboard` / `d` | Refresh the gauge display |
| `quit` / `q` | Exit |

## Expected Output
Client session:

```text
> read 0c
Engine RPM: 4500 RPM
> vin
VIN: ...
> dtc
```

Periodically, the dashboard receives `CheckEngine` notifications as simulated
faults are injected.

## What Just Happened
The server owned the ECU state machine and fault lifecycle, while the client
used typed methods for queries and notifications for faults that appear on
their own schedule. That pattern maps cleanly to many real device protocols.

## Testing
Run from the repository root (requires a build with `-e`):

```bash
ctest --test-dir build --output-on-failure -R ecu_tests
```

## What To Modify Next
- add another PID or DTC severity to the IDL and regenerate
- change the dashboard to keep a persistent list of active trouble codes

## Related Examples
- [`motor-controller/`](../motor-controller/) for another control-heavy device dashboard
- [`process-monitor/`](../process-monitor/) for a monitoring-oriented dashboard in the same section
