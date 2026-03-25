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

| Example | Integration path | What it shows |
|---------|-----------------|---------------|
| [`sdk-usage/`](sdk-usage/) | **Way 1 — SDK release** | IDL → `ipcgen --backend c_api` → C++ server/client built against the release tarball |
| [`echo/`](echo/) | **Way 2 — Submodule** | IDL → `ipcgen` (default backend) → server/client using `ServiceBase`/`ClientBase` |
| [`c-echo/`](c-echo/) | Either | Minimal C echo server + client using the raw C API directly, no code generation |
| [`exhaust-analyzer/`](exhaust-analyzer/) | **Way 2 — Submodule** | Advanced example with Qt5 GUI, structs, enums, arrays, and notifications |
| [`serial-loopback/`](serial-loopback/) | **Way 2 — Submodule** | Minimal custom `ITransport` over PTY, exercising `TransportClientBase` without hardware |
| [`serial-sensor/`](serial-sensor/) | **Way 2 — Submodule** | Host C++ client talking to a simulated `aether-lite` sensor device over PTY |
| [`mcu-firmware/`](mcu-firmware/) | Firmware template | Bare-metal UART integration pattern for `aether-lite` on MCU targets |
