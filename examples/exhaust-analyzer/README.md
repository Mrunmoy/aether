# Exhaust Gas Analyzer Example

A simulated car exhaust gas analyzer — the kind of device found in emissions
testing stations and performance tuning shops. Demonstrates aether's
code-generated RPC and live notification broadcast with a Qt5 GUI client.

## Screenshots

**Disconnected** — gauges idle, waiting to connect to the analyzer service:

![Client offline](img/client-offline.png)

**Measuring** — live sensor data streaming at 10 Hz via IPC notifications:

![Client measuring](img/client-measuring.png)

## What it shows

- **IDL-defined service** (`ExhaustAnalyzer.idl`) with RPC methods and push notifications
- **Server** simulates 7 gas sensors (O₂, CO, CO₂, NOx, HC, λ, exhaust temp) with a state machine (Offline → Warming → Ready → Measuring)
- **Qt5 client** receives live `ReadingsUpdated` notifications over shared-memory IPC and displays color-coded gauges (green/amber/red thresholds)
- Thread-safe IPC-to-Qt signal crossing via `Qt::QueuedConnection`

## Building

Requires the nix dev environment for Qt5:

```bash
nix develop
python3 build.py -e
```

## Running

Start the server in one terminal, client in another (both inside `nix develop`):

```bash
# Terminal 1
./build/examples/exhaust-analyzer/exhaust_server

# Terminal 2
./build/examples/exhaust-analyzer/exhaust_client
```

Click **CONNECT**, then **START** to begin live measurement.

## IDL interface

```idl
service ExhaustAnalyzer
{
    [method=1] int GetStatus([out] AnalyzerStatus status);
    [method=2] int GetCurrentReadings([out] GasReadings readings);
    [method=3] int StartMeasurement([out] bool started);
    [method=4] int StopMeasurement([out] bool stopped);
};

notifications ExhaustAnalyzer
{
    [notify=1] void ReadingsUpdated([in] GasReadings readings);
    [notify=2] void StatusChanged([in] AnalyzerStatus status);
};
```

## Sensor channels

| Channel | Unit | Typical range |
|---------|------|---------------|
| O₂ | % | 0.5 – 2.0 |
| CO | % | 0.5 – 3.0 |
| CO₂ | % | 12 – 14 |
| NOx | ppm | 500 – 2000 |
| HC | ppm | 50 – 300 |
| λ (Lambda) | — | 0.97 – 1.03 |
| EGT | °C | 350 – 500 |
