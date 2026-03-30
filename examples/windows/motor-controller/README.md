# Motor Controller Example

Control a simulated stepper motor through typed methods and observe motion
events through notifications.

## What You'll Learn
- how to model commands and state queries for a device controller
- how to emit completion, fault, and limit events as notifications
- how a dashboard client can drive a simulation loop through generated APIs

## Prerequisites
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `MotorController.idl` | homing, motion, status, and notification contract |
| `motor_device.cpp` | simulated stepper motor state machine |
| `motor_dashboard.cpp` | interactive control client |
| `motor_test.cpp` | example-level tests |

## Step 1: Read the IDL
`MotorController.idl` defines:
- methods to home, move, jog, stop, and query status
- `MotorStatus` plus enums for motor state and active limit switch
- notifications for motion complete, limit hits, and stall detection

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/windows/motor-controller/MotorController.idl --outdir examples/windows/motor-controller/gen
```

## Step 3: Review the User Code
- `motor_device.cpp` simulates position, velocity, homing, and fault behavior.
- `motor_dashboard.cpp` is the operator-facing REPL built on the generated client.
- Aether handles connection, request dispatch, and notification delivery.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/windows/motor-controller/motor_device

# Terminal 2
./build/examples/windows/motor-controller/motor_dashboard
```

## Expected Output
Client session:

```text
> home
> move 2500 500
> status
State: Moving
Position: ...
```

As the simulation runs, notifications report motion completion, limit hits, or
stall events.

## What Just Happened
The client used typed commands to control the motor simulation, while the
server decided when motion or fault events were important enough to push back
as notifications. This is a clean example of RPC for control and events for
telemetry.

## What To Modify Next
- add another command such as setting acceleration or jerk
- teach the dashboard to subscribe and print a motion timeline

## Related Examples
- [`can-bus-ecu/`](../can-bus-ecu/) for another control-heavy dashboard
- [`../../linux/gpio-controller/`](../../linux/gpio-controller/) for a simpler device-control example
