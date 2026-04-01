# MCU Firmware Template (aether-lite)

Use this template when you want to move from the simulated serial examples into
real firmware that embeds `aether-lite` on a microcontroller.

## What You'll Learn
- how to structure a small UART-backed `aether-lite` firmware service
- what code belongs in your HAL glue versus your RPC handlers
- how to port the desktop serial examples into a bare-metal project

## Prerequisites
- a firmware project outside this repository
- an MCU toolchain such as `arm-none-eabi-gcc`
- access to the `aether-lite/` headers in your firmware build

## Files That Matter
| File | Why it matters |
|------|----------------|
| `main.c` | startup code, UART/HAL integration points, ring buffer, and `al_poll()` loop |
| `device_handlers.h` | service IDs, wire structs, and handler declarations |
| `device_handlers.c` | example handler implementations and notification logic |

## Step 1: Read the IDL
This template does not use `.idl` files or the Python generator.

The service contract is expressed directly in `device_handlers.h`:
- method `1`: `GetDeviceInfo`
- method `2`: `ReadSensor`
- method `3`: `SetLedState`
- notification `1`: `SensorUpdate`

That is deliberate. On the firmware side you often start by proving the runtime,
memory, and UART path first, then decide whether to add generated bindings.

## Step 2: Generate Code
No code generation step is required.

You copy these files into your firmware tree and adapt the HAL stubs to your
board support package.

## Step 3: Review the User Code
- `main.c` owns the boot path, byte transport, and polling loop.
- `device_handlers.c` owns the actual service behavior: reporting device info,
  reading the sensor, toggling the LED, and sending periodic updates.
- `aether-lite` handles frame parsing, CRC, request dispatch, and response
  encoding once your HAL callbacks feed it bytes.

## Build
There is no desktop build target for this directory.

Instead:

```bash
# from your firmware project
cp /path/to/aether/examples/transports/mcu-firmware/main.c .
cp /path/to/aether/examples/transports/mcu-firmware/device_handlers.[ch] .
```

Then add those files to your firmware build and put the `aether-lite/`
headers on the include path.

## Run
After you wire the HAL stubs:

1. flash the firmware onto your board
2. connect the UART transport used by your host
3. start the firmware main loop so `al_poll()` can process frames

## Expected Output
This template does not print desktop output by itself. Success usually looks
like:

```text
host connects over UART
GetDeviceInfo returns device metadata
ReadSensor returns a float value
SetLedState toggles the target GPIO
SensorUpdate notifications arrive periodically
```

## What Just Happened
This template separates the embedded responsibilities cleanly: your board code
feeds bytes in and out, your handler file implements business logic, and
`aether-lite` handles the protocol machinery. It is the firmware counterpart to
the simulated [`serial-sensor/`](../serial-sensor/) example.

## What To Modify Next
- replace the `/* >>> REPLACE ... */` HAL stubs with your board-specific UART and GPIO calls
- add one more RPC method and one more notification before integrating a host client

## Related Examples
- [`../serial-sensor/`](../serial-sensor/) for a host/device serial setup that runs entirely on a dev machine
- [`../serial-loopback/`](../serial-loopback/) for the smallest custom-transport example
