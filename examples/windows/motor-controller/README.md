# Motor Controller Example

Simulated stepper motor controller using the Aether IPC framework.  The
device is entirely software-simulated so it compiles and runs on any
supported platform (Linux, macOS, Windows).

## Components

| Binary              | Description                                      |
|---------------------|--------------------------------------------------|
| `motor_device`      | IPC server — simulated stepper motor with trapezoidal motion profiles |
| `motor_dashboard`   | Interactive CLI client for commanding the motor  |
| `motor_tests`       | Google Test suite for the motor controller       |

## Simulated Motor

- **Position range:** −10 000 to +10 000 steps
- **Limit switches:** Lower at −10 000, Upper at +10 000
- **Max velocity:** 1 000 steps/s
- **Acceleration:** 500 steps/s²
- **Simulation tick:** 10 ms

## Running

```bash
# Terminal 1 — start the motor device server
./build/examples/windows/motor-controller/motor_device

# Terminal 2 — connect the dashboard
./build/examples/windows/motor-controller/motor_dashboard
```

### Dashboard commands

| Command           | Description                                |
|-------------------|--------------------------------------------|
| `home`            | Start homing sequence (move to lower limit, zero position) |
| `move <pos> [v]`  | Move to position at optional velocity (default 500 steps/s) |
| `jog <vel>`       | Continuous motion at given velocity        |
| `stop`            | Immediately halt the motor                 |
| `status`          | Print full motor status                    |
| `pos`             | Print current position                     |
| `help`            | Show available commands                    |
| `quit`            | Exit the dashboard                         |

## Notifications

The server broadcasts three notifications:

- **MotionComplete(position)** — a MoveTo or Home finished
- **LimitHit(which, position)** — a limit switch was activated
- **StallDetected(position, velocity)** — motor stalled (position unchanged for 500 ms while velocity is non-zero)

## Tests

```bash
ctest --test-dir build --output-on-failure -R MotorControllerTest
```
