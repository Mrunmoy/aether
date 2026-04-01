# GPIO Controller Example

Use a simulated 16-pin GPIO expander to see how Aether models device control
plus edge-triggered notifications.

## What You'll Learn
- how to expose a stateful device model through generated methods
- how to publish asynchronous interrupts as notifications
- how to structure a dashboard-style client for device control

## Prerequisites
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `GpioController.idl` | pin configuration, read/write, and interrupt contract |
| `gpio_device.cpp` | simulated GPIO device implementation |
| `gpio_dashboard.cpp` | interactive client for configuring and observing pins |
| `gpio_test.cpp` | example-level tests for the service behavior |

## Step 1: Read the IDL
`GpioController.idl` defines:
- enums for pin direction, level, and edge trigger mode
- `PinState` for full pin snapshots
- methods to configure, read, write, and dump all pins
- one notification, `PinInterrupt`, for matching edge transitions

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/platform/linux/gpio-controller/GpioController.idl --outdir examples/platform/linux/gpio-controller/gen
```

## Step 3: Review the User Code
- `gpio_device.cpp` owns the simulated pin state and interrupt generation.
- `gpio_dashboard.cpp` turns the generated client API into a simple REPL.
- Aether handles connection lifecycle, request routing, and notification
  delivery to the dashboard.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/platform/linux/gpio-controller/gpio_device

# Terminal 2
./build/examples/platform/linux/gpio-controller/gpio_dashboard
```

## Expected Output
Dashboard session:

```text
Connected to GPIO controller.
Commands: config <pin> <in|out> [edge], read <pin>, write <pin> <0|1>, status, quit
> config 3 out
> write 3 1
> read 3
Pin 3: HIGH
```

Asynchronous notifications appear like:

```text
[interrupt] Pin 7: Rising
```

## What Just Happened
The example combines ordinary RPC methods for configuration with notifications
for state changes that the device decides to emit on its own. That pattern
shows up in many hardware-facing services: configure through calls, react
through notifications.

## Testing
Run from the repository root (requires a build with `-e`):

```bash
ctest --test-dir build --output-on-failure -R gpio_tests
```

The suite covers pin configuration, read/write, error handling, interrupt notifications, and RunLoop dispatch.

## What To Modify Next
- add another pin capability such as pull-up configuration
- emit interrupt timestamps and surface them in the dashboard

## Testing
Run from the repository root:

```bash
ctest --test-dir build --output-on-failure -R gpio_tests
```

## Related Examples
- [`../sensor-hub/`](../sensor-hub/) for another simulated device/dashboard pair
- [`../file-watcher/`](../file-watcher/) for a notification-heavy service driven by real OS events
