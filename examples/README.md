# Examples

Use this directory as a learning path, not a flat catalog.

If you are new to Aether, do not browse randomly. Start with one example,
finish it, then move to the next layer of complexity.

## Start Here

### 1. Canonical source-build path

Start with [`echo/`](echo/).

It is the default newcomer example because it shows the full Aether source-build
workflow:

- define a service in IDL
- generate typed C++ code with `ipcgen`
- implement a server and a client
- run both processes and observe RPC plus notification flow

### 2. Raw C API

Move to [`c-echo/`](c-echo/) next if you want to see the runtime without code
generation.

It shows:

- no IDL
- no generated files
- direct use of `aether_ipc.h`
- explicit service and message IDs

### 3. SDK consumer path

Read [`sdk-usage/`](sdk-usage/) if your real project will consume a packaged
SDK tarball instead of building Aether from source.

It shows:

- `ipcgen --backend c_api`
- generated wrappers that only depend on `aether_ipc.h`
- building against an extracted SDK tarball

## Learning Path

### Getting started

| Example | Use it when | What it teaches |
|---------|-------------|-----------------|
| [`echo/`](echo/) | You want the recommended first success | IDL, code generation, typed C++ client/server, runtime mental model |
| [`c-echo/`](c-echo/) | You want the lowest-level host API | Raw C API, manual service/message IDs, no codegen |
| [`sdk-usage/`](sdk-usage/) | You ship or consume a prebuilt SDK | `c_api` backend, SDK-only build, generated wrappers without the source tree |

### Transports and embedded flow

| Example | Use it when | What it teaches |
|---------|-------------|-----------------|
| [`serial-loopback/`](serial-loopback/) | You want to understand `ITransport` without hardware | PTY-backed transport, framing over a byte stream, `TransportClientBase` |
| [`serial-sensor/`](serial-sensor/) | You want host-to-device communication | Host C++ client talking to a simulated `aether-lite` device over serial framing |
| [`mcu-firmware/`](mcu-firmware/) | You need a firmware starting point | Bare-metal `aether-lite` integration pattern for UART-based devices |

### Application-style examples

| Example | Use it when | What it teaches |
|---------|-------------|-----------------|
| [`exhaust-analyzer/`](exhaust-analyzer/) | You want a richer desktop app | Qt GUI, generated code in a larger application, structs/enums/notifications |

### Platform and domain examples

These examples are best after you already understand `echo/`. They are useful
for domain patterns, not for the first successful run.

#### Linux

| Example | What it shows |
|---------|---------------|
| [`linux/sysmon/`](linux/sysmon/) | Linux `/proc` monitoring with notifications |
| [`linux/file-watcher/`](linux/file-watcher/) | `inotify`-based notifications for file changes |
| [`linux/gpio-controller/`](linux/gpio-controller/) | Simulated GPIO control and pin-change notifications |
| [`linux/sensor-hub/`](linux/sensor-hub/) | Simulated multi-sensor aggregation and alarm notifications |

#### macOS

| Example | What it shows |
|---------|---------------|
| [`macos/battery-monitor/`](macos/battery-monitor/) | Simulated battery monitoring and threshold notifications |
| [`macos/disk-usage/`](macos/disk-usage/) | Simulated disk-usage reporting and scan notifications |
| [`macos/ble-peripheral/`](macos/ble-peripheral/) | Simulated BLE service/characteristic interactions |
| [`macos/audio-dsp/`](macos/audio-dsp/) | Simulated DSP control, metering, and fault notifications |

#### Windows

| Example | What it shows |
|---------|---------------|
| [`windows/service-status/`](windows/service-status/) | Simulated service control and state-change notifications |
| [`windows/process-monitor/`](windows/process-monitor/) | Simulated process lifecycle and resource queries |
| [`windows/can-bus-ecu/`](windows/can-bus-ecu/) | Simulated ECU RPC and bus-status notifications |
| [`windows/motor-controller/`](windows/motor-controller/) | Simulated motor control, motion commands, and fault notifications |

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

- Want the default path: [`echo/`](echo/)
- Want the raw API: [`c-echo/`](c-echo/)
- Want the SDK path: [`sdk-usage/`](sdk-usage/)
- Want transport internals: [`serial-loopback/`](serial-loopback/)
- Want host plus embedded flow: [`serial-sensor/`](serial-sensor/)
- Want firmware: [`mcu-firmware/`](mcu-firmware/)
- Want a richer application: [`exhaust-analyzer/`](exhaust-analyzer/)

Everything else is best treated as domain-specific follow-on material.
