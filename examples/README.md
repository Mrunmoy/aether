# Examples

Use this directory as a learning path, not a flat catalog.

If you are new to Aether, do not browse randomly. Start with one example,
finish it, then move to the next layer of complexity.

## Start Here

### 1. Canonical source-build path

Start with [`getting-started/echo/`](getting-started/echo/).

It is the default newcomer example because it shows the full Aether source-build
workflow:

- define a service in IDL
- generate typed C++ code with `ipcgen`
- implement a server and a client
- run both processes and observe RPC plus notification flow

### 2. Raw C API

Move to [`getting-started/c-echo/`](getting-started/c-echo/) next if you want to see the runtime without code
generation.

It shows:

- no IDL
- no generated files
- direct use of `aether_ipc.h`
- explicit service and message IDs

### 3. SDK consumer path

Read [`getting-started/sdk-usage/`](getting-started/sdk-usage/) if your real project will consume a packaged
SDK tarball instead of building Aether from source.

It shows:

- `ipcgen --backend c_api`
- generated wrappers that only depend on `aether_ipc.h`
- building against an extracted SDK tarball

## Learning Path

### Getting started

| Example | Use it when | What it teaches |
|---------|-------------|-----------------|
| [`getting-started/echo/`](getting-started/echo/) | You want the recommended first success | IDL, code generation, typed C++ client/server, runtime mental model |
| [`getting-started/c-echo/`](getting-started/c-echo/) | You want the lowest-level host API | Raw C API, manual service/message IDs, no codegen |
| [`getting-started/sdk-usage/`](getting-started/sdk-usage/) | You ship or consume a prebuilt SDK | `c_api` backend, SDK-only build, generated wrappers without the source tree |

### Transports and embedded flow

| Example | Use it when | What it teaches |
|---------|-------------|-----------------|
| [`transports/serial-loopback/`](transports/serial-loopback/) | You want to understand `ITransport` without hardware | PTY-backed transport, framing over a byte stream, `TransportClientBase` |
| [`transports/serial-sensor/`](transports/serial-sensor/) | You want host-to-device communication | Host C++ client talking to a simulated `aether-lite` device over serial framing |
| [`transports/mcu-firmware/`](transports/mcu-firmware/) | You need a firmware starting point | Bare-metal `aether-lite` integration pattern for UART-based devices |

### Application-style examples

| Example | Use it when | What it teaches |
|---------|-------------|-----------------|
| [`apps/exhaust-analyzer/`](apps/exhaust-analyzer/) | You want a richer desktop app | Qt GUI, generated code in a larger application, structs/enums/notifications |

### Platform and domain examples

These examples are best after you already understand `getting-started/echo/`. They are useful
for domain patterns, not for the first successful run.

#### Linux

| Example | What it shows |
|---------|---------------|
| [`platform/linux/sysmon/`](platform/linux/sysmon/) | Linux `/proc` monitoring with notifications |
| [`platform/linux/file-watcher/`](platform/linux/file-watcher/) | `inotify`-based notifications for file changes |
| [`platform/linux/gpio-controller/`](platform/linux/gpio-controller/) | Simulated GPIO control and pin-change notifications |
| [`platform/linux/sensor-hub/`](platform/linux/sensor-hub/) | Simulated multi-sensor aggregation and alarm notifications |

#### macOS

| Example | What it shows |
|---------|---------------|
| [`platform/macos/battery-monitor/`](platform/macos/battery-monitor/) | Simulated battery monitoring and threshold notifications |
| [`platform/macos/disk-usage/`](platform/macos/disk-usage/) | Simulated disk-usage reporting and scan notifications |
| [`platform/macos/ble-peripheral/`](platform/macos/ble-peripheral/) | Simulated BLE service/characteristic interactions |
| [`platform/macos/audio-dsp/`](platform/macos/audio-dsp/) | Simulated DSP control, metering, and fault notifications |

#### Windows

| Example | What it shows |
|---------|---------------|
| [`platform/windows/service-status/`](platform/windows/service-status/) | Simulated service control and state-change notifications |
| [`platform/windows/process-monitor/`](platform/windows/process-monitor/) | Simulated process lifecycle and resource queries |
| [`platform/windows/can-bus-ecu/`](platform/windows/can-bus-ecu/) | Simulated ECU RPC and bus-status notifications |
| [`platform/windows/motor-controller/`](platform/windows/motor-controller/) | Simulated motor control, motion commands, and fault notifications |

## README Contract

Every example README should help a user answer the same questions in the same
order:

1. What is this example for?
2. What will I learn?
3. What files matter?
4. What IDL exists, if any?
5. What code is generated?
6. What code do I write?
7. How do I build it?
8. How do I run it?
9. What output should I expect?
10. What should I try next?

The standard format for new or rewritten examples lives in
[`README_TEMPLATE.md`](README_TEMPLATE.md).

## Picking The Right Example

Use this rule of thumb:

- Want the default path: [`getting-started/echo/`](getting-started/echo/)
- Want the raw API: [`getting-started/c-echo/`](getting-started/c-echo/)
- Want the SDK path: [`getting-started/sdk-usage/`](getting-started/sdk-usage/)
- Want transport internals: [`transports/serial-loopback/`](transports/serial-loopback/)
- Want host plus embedded flow: [`transports/serial-sensor/`](transports/serial-sensor/)
- Want firmware: [`transports/mcu-firmware/`](transports/mcu-firmware/)
- Want a richer application: [`apps/exhaust-analyzer/`](apps/exhaust-analyzer/)

Everything else is best treated as domain-specific follow-on material.
