# GPIO Controller Example

Simulated 16-pin GPIO expander chip exposed as an Aether IPC service.

## IDL Overview

- **Enums:** `PinDirection` (Input/Output), `PinLevel` (Low/High), `EdgeTrigger` (None/Rising/Falling/Both)
- **Struct:** `PinState` — per-pin snapshot (pin number, direction, level, edge trigger)
- **Service:** `GpioController` — `ConfigurePin`, `ReadPin`, `WritePin`, `ReadAllPins`
- **Notification:** `PinInterrupt(pin, edge)` — fires when an input pin changes and its edge trigger matches

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DMS_IPC_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

## Run

Start the device service in one terminal:
```bash
./build/examples/linux/gpio-controller/gpio_device
```

Connect the dashboard in another:
```bash
./build/examples/linux/gpio-controller/gpio_dashboard
```

### Sample Session

```
Connected to GPIO controller.

 Pin | Dir    | Level | Edge
-----+--------+-------+---------
  0  | INPUT  | LOW   | None
  1  | INPUT  | LOW   | None
  ...

Commands: config <pin> <in|out> [edge], read <pin>, write <pin> <0|1>, status, quit
> config 3 out
Pin 3 configured: OUTPUT, edge=None
> write 3 1
Pin 3 written: HIGH
> read 3
Pin 3: HIGH
> config 7 in rising
Pin 7 configured: INPUT, edge=Rising
> status
 Pin | Dir    | Level | Edge
-----+--------+-------+---------
  ...
  3  | OUTPUT | HIGH  | None
  7  | INPUT  | LOW   | Rising
  ...
> quit
Disconnected.
```

Interrupt notifications from the simulation thread appear asynchronously:
```
[interrupt] Pin 7: Rising
```

## Tests

```bash
ctest --test-dir build --output-on-failure -R GpioControllerTest
```

The test suite covers pin configuration, read/write, error handling, interrupt notifications, and RunLoop dispatch mode.
