# SensorHub Example

Explore a simulated IMU plus environment sensor service with both on-demand
queries and live motion/data-ready notifications.

## What You'll Learn
- how to model multiple sensor domains in one generated service
- how notifications complement query-style methods
- how to build a dashboard client that mixes commands with background events

## Prerequisites
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `SensorHub.idl` | service contract for sensor queries, sample rate control, and notifications |
| `sensor_device.cpp` | simulated data source and service implementation |
| `sensor_dashboard.cpp` | interactive client for polling and control |
| `sensor_test.cpp` | tests for service behavior and notifications |

## Step 1: Read the IDL
`SensorHub.idl` defines:
- structs for accelerometer, gyroscope, and environment data
- methods to query those values, change sample rate, and trigger calibration
- `DataReady` and `MotionDetected` notifications for event-driven updates

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/linux/sensor-hub/SensorHub.idl --outdir examples/linux/sensor-hub/gen
```

## Step 3: Review the User Code
- `sensor_device.cpp` simulates the sensors, owns the sample-rate state, and
  fires notifications when new data or motion thresholds are hit.
- `sensor_dashboard.cpp` issues typed RPC calls and prints notification events.
- Aether handles all transport, framing, and callback dispatch between them.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/linux/sensor-hub/sensor_device

# Terminal 2
./build/examples/linux/sensor-hub/sensor_dashboard
```

## Expected Output
Client:

```text
Connected to SensorHub.
Commands: accel | gyro | env | rate <1|10|50|100> | calibrate | quit
> accel
Accel: X=0.02 Y=-0.05 Z=9.83
> calibrate
Calibration successful
```

Server:

```text
SensorHub device running (10 Hz default). Press Ctrl-C to stop.
[device] sample rate set to 50 Hz
```

## What Just Happened
This example packs several related sensor interfaces into one typed service.
Methods handle query and control flows, while notifications carry unsolicited
events such as motion or fresh samples. It is a good bridge from the simple
examples into more device-like service design.

## Testing
Run from the repository root (requires a build with `-e`):

```bash
ctest --test-dir build --output-on-failure -R sensor_tests
```

## What To Modify Next
- add another sensor reading such as magnetometer data
- change the motion threshold and watch how notification frequency changes

## Related Examples
- [`../gpio-controller/`](../gpio-controller/) for another device-style dashboard example
- [`../sysmon/`](../sysmon/) for a monitoring-oriented service with alerts
