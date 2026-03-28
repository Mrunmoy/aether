# CAN Bus ECU Example

Simulates an automotive Engine Control Unit (ECU) exposing OBD-II PIDs over Aether IPC.
The device is fully simulated — no hardware required — so it compiles on any platform.

## Components

| Binary | Description |
|--------|-------------|
| `ecu_device` | Simulated ECU server with state-machine engine model |
| `ecu_dashboard` | Interactive OBD-II dashboard CLI |
| `ecu_tests` | Google Test suite for the CanBusEcu service |

## Supported OBD-II PIDs

| PID | Name | Range | Unit |
|-----|------|-------|------|
| 0x0C | Engine RPM | 0–8000 | RPM |
| 0x0D | Vehicle Speed | 0–200 | km/h |
| 0x05 | Coolant Temperature | 80–110 | °C |
| 0x11 | Throttle Position | 0–100 | % |
| 0x2F | Fuel Level | 0–100 | % |

## ECU State Machine

The device cycles through four states every 3 seconds:

```
Idle → Accelerating → Cruising → Braking → Idle → …
```

Each state sets PID values:
- **Idle**: RPM 800, Speed 0, Throttle 0%
- **Accelerating**: RPM 4500, Speed 80, Throttle 70%
- **Cruising**: RPM 2500, Speed 80, Throttle 25%
- **Braking**: RPM 800, Speed 0, Throttle 0%

Every 16 state transitions, a new DTC is injected and a `CheckEngine`
notification is broadcast to connected clients.

## Running

```bash
# Terminal 1 — start the ECU device
./ecu_device

# Terminal 2 — connect the dashboard
./ecu_dashboard
```

### Dashboard Commands

| Command | Description |
|---------|-------------|
| `read <pid_hex>` | Read a single PID (e.g. `read 0c`) |
| `dtc` | List stored DTCs |
| `clear` | Clear all DTCs |
| `vin` | Show vehicle identification |
| `dashboard` / `d` | Refresh the gauge display |
| `quit` / `q` | Exit |

## IDL

The service is defined in `CanBusEcu.idl`. Generated code lives in `gen/`
and should not be edited by hand.

## Building

This example is built as part of the Aether examples:

```bash
python3 build.py -e      # Build library + examples
python3 build.py -t       # Build + run all tests
```
