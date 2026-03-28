# SensorHub Example

A simulated multi-sensor fusion device (IMU + environment sensor), demonstrating Aether IPC service methods and notifications.

## Overview

The **SensorHub** service simulates accelerometer, gyroscope, and environment sensor data. The server generates readings with configurable noise and broadcasts them at a configurable sample rate.

### IDL

```
enum SampleRate { Rate_1Hz=1, Rate_10Hz=10, Rate_50Hz=50, Rate_100Hz=100 };

service SensorHub {
    int GetAccelerometer([out] AccelData accel);
    int GetGyroscope([out] GyroData gyro);
    int GetEnvironment([out] EnvironmentData env);
    int SetSampleRate([in] SampleRate rate);
    int Calibrate([out] uint8 success);
};

notifications SensorHub {
    void DataReady([in] AccelData accel, [in] GyroData gyro);
    void MotionDetected([in] float32 magnitude);
};
```

## Building

From the repository root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DMS_IPC_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

## Running

Start the server in one terminal:

```bash
./build/examples/linux/sensor-hub/sensor_device
```

Start the dashboard client in another terminal:

```bash
./build/examples/linux/sensor-hub/sensor_dashboard
```

### Dashboard Commands

| Command | Description |
|---------|-------------|
| `accel` | Query current accelerometer reading |
| `gyro` | Query current gyroscope reading |
| `env` | Query current environment reading |
| `rate <1\|10\|50\|100>` | Set sample rate in Hz |
| `calibrate` | Run sensor calibration |
| `quit` | Exit dashboard |

### Sample Output

**Server:**
```
╔══════════════════════════════════╗
║   Aether SensorHub Device        ║
║   IMU + Environment Simulator    ║
╚══════════════════════════════════╝

SensorHub device running (10 Hz default). Press Ctrl-C to stop.
[device] sample rate set to 50 Hz
[device] calibrated
```

**Client:**
```
Connected to SensorHub.
Accel: X=0.02  Y=-0.05  Z=9.83  (m/s²)
Gyro:  X=0.12  Y=-0.34  Z=0.08  (deg/s)
Env:   T=22.3°C  P=1013.1hPa  H=44.8%

Commands: accel | gyro | env | rate <1|10|50|100> | calibrate | quit
> calibrate
Calibration successful
```

## Tests

```bash
ctest --test-dir build --output-on-failure -R SensorHub
```
