# Examples

## Which example should I use?

Aether supports two integration paths (see the main
[README](../README.md#integrating-aether-into-your-project) for details):

**Way 1 — SDK release** (you downloaded a pre-built `aether-sdk-*.tar.gz`):

→ Start with [`sdk-usage/`](sdk-usage/) — write an IDL, generate code with
`ipcgen --backend c_api`, implement your server/client, build against the
tarball. No source tree needed.

**Way 2 — Git submodule** (you added aether as a submodule and build from source):

→ Start with [`echo/`](echo/) — write an IDL, generate code with `ipcgen`
(default backend), link against the `aether` CMake target.

If you are extending transports instead of using the shared-memory runtime:

- Start with [`serial-loopback/`](serial-loopback/) for a minimal
  `TransportClientBase` + `ITransport` example over PTY.
- Then look at [`serial-sensor/`](serial-sensor/) for host ↔ `aether-lite`
  interoperability over the same framing.

If you are targeting a microcontroller directly:

- Start with [`mcu-firmware/`](mcu-firmware/) for a bare-metal UART template
  using `aether-lite`.

## All examples

### Core examples

| Example | Integration path | What it shows |
|---------|-----------------|---------------|
| [`sdk-usage/`](sdk-usage/) | **Way 1 — SDK release** | IDL → `ipcgen --backend c_api` → C++ server/client built against the release tarball |
| [`echo/`](echo/) | **Way 2 — Submodule** | IDL → `ipcgen` (default backend) → server/client using `ServiceBase`/`ClientBase` |
| [`c-echo/`](c-echo/) | Either | Minimal C echo server + client using the raw C API directly, no code generation |
| [`exhaust-analyzer/`](exhaust-analyzer/) | **Way 2 — Submodule** | Advanced example with Qt5 GUI, structs, enums, arrays, and notifications |
| [`serial-loopback/`](serial-loopback/) | **Way 2 — Submodule** | Minimal custom `ITransport` over PTY, exercising `TransportClientBase` without hardware |
| [`serial-sensor/`](serial-sensor/) | **Way 2 — Submodule** | Host C++ client talking to a simulated `aether-lite` sensor device over PTY |
| [`mcu-firmware/`](mcu-firmware/) | Firmware template | Bare-metal UART integration pattern for `aether-lite` on MCU targets |

### Platform-specific examples

Each platform directory contains OS-level and simulated-embedded examples.
Every example ships with its own IDL, generated code, server/device implementation,
client/dashboard, Google Test suite, CMakeLists.txt, and README.

#### Linux (`linux/`)

| Example | Type | What it shows |
|---------|------|---------------|
| [`linux/sysmon/`](linux/sysmon/) | OS-level | Real-time system monitor reading `/proc` (CPU, memory, disk, load average) with periodic notifications |
| [`linux/file-watcher/`](linux/file-watcher/) | OS-level | `inotify`-based file system watcher with create/modify/delete notifications |
| [`linux/gpio-controller/`](linux/gpio-controller/) | Simulated embedded | GPIO pin controller with digital read/write, interrupt simulation, and pin-change notifications |
| [`linux/sensor-hub/`](linux/sensor-hub/) | Simulated embedded | Multi-sensor hub (temperature, humidity, pressure) with threshold-based alarm notifications |

> **Note:** `sysmon` and `file-watcher` use Linux-specific APIs and only build on Linux.
> `gpio-controller` and `sensor-hub` are cross-platform (simulated devices).

#### macOS (`macos/`)

| Example | Type | What it shows |
|---------|------|---------------|
| [`macos/battery-monitor/`](macos/battery-monitor/) | Simulated | Battery monitor with charge/discharge simulation, threshold alerts, and state notifications |
| [`macos/disk-usage/`](macos/disk-usage/) | Simulated | Disk usage monitor with per-volume stats, threshold warnings, and periodic scan notifications |
| [`macos/ble-peripheral/`](macos/ble-peripheral/) | Simulated embedded | BLE peripheral simulator with GATT services, characteristic read/write, and connection notifications |
| [`macos/audio-dsp/`](macos/audio-dsp/) | Simulated embedded | Audio DSP pipeline with configurable effects (EQ, compressor, reverb), level metering, and clipping detection |

#### Windows (`windows/`)

| Example | Type | What it shows |
|---------|------|---------------|
| [`windows/service-status/`](windows/service-status/) | Simulated | Windows service status monitor with start/stop control, state-change notifications, and health checks |
| [`windows/process-monitor/`](windows/process-monitor/) | Simulated | Process monitor with spawn/exit tracking, resource usage queries, and process-lifecycle notifications |
| [`windows/can-bus-ecu/`](windows/can-bus-ecu/) | Simulated embedded | CAN bus ECU simulator with frame send/receive, DTC management, and bus-error notifications |
| [`windows/motor-controller/`](windows/motor-controller/) | Simulated embedded | Stepper motor controller with homing, position moves, jog, limit switches, and stall detection |

> **Note:** All platform examples use simulated devices and build on all platforms
> (Linux, macOS, Windows) — the directory names indicate the *theme*, not a build restriction
> (except `sysmon` and `file-watcher` which require Linux).
