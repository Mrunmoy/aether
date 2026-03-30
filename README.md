# Aether

![Build](https://github.com/Mrunmoy/aether/actions/workflows/ci.yml/badge.svg?branch=main&event=push)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-informational)

**Shared-memory IPC with IDL code generation for C++, Python, C API, and C99 firmware targets.**

Aether gives you a typed contract between processes on the same machine, and it
reuses that same wire protocol for desktop-to-device links over serial or USB.
The default path is simple: write IDL, generate C++ bindings, implement your
handlers, and let the runtime take care of shared memory, framing, dispatch,
and notifications.

## Start Here

If this is your first time in the repo, use the source-build C++ path and the
typed `echo` example. That is the canonical newcomer flow.

**Prerequisites:** C++17 compiler, CMake 3.14+, Python 3, and Linux or macOS
or Windows.

```bash
git clone --recursive https://github.com/Mrunmoy/aether.git
cd aether
python3 build.py -e
```

Run the example in two terminals from the repository root:

Terminal 1:
```bash
./build/examples/echo/device_monitor_server
```

Terminal 2:
```bash
./build/examples/echo/device_monitor_client
```

## Verify

You should see output similar to:

```text
[client] device count: 2
[client] device 0: USB Audio Interface (vendor=0x1234 product=0x0001)
[client] device 1: BLE Sensor Tag (vendor=0x4321 product=0x0002)
[client] waiting briefly for notifications...
[client] notification: connected -> USB Audio Interface
[client] notification: disconnected -> id=1
[client] disconnected.
```

At that point you have already exercised:
- IDL-defined types and methods
- generated client and server glue
- request/response RPC
- notification delivery
- the shared-memory runtime under the hood

## Understand

The canonical example lives in [`examples/echo/`](examples/echo/). It walks
through the full flow:

1. read [`DeviceMonitor.idl`](examples/echo/DeviceMonitor.idl)
2. generate code with:

   ```bash
   python3 -m tools.ipcgen examples/echo/DeviceMonitor.idl --outdir examples/echo/gen
   ```

3. inspect the generated files in [`examples/echo/gen/`](examples/echo/gen/)
4. compare them with the user-written files:
   - [`examples/echo/device_monitor_server.cpp`](examples/echo/device_monitor_server.cpp)
   - [`examples/echo/device_monitor_client.cpp`](examples/echo/device_monitor_client.cpp)

If you want the short mental model before reading more code, start with
[Aether in 5 Minutes](doc/AetherIn5Minutes.md).

## Branch Out

Once the canonical path makes sense, use one of these next:

| Goal | Start here |
|------|------------|
| See the raw runtime without code generation | [`examples/c-echo/`](examples/c-echo/) |
| Consume a packaged SDK instead of building from source | [`examples/sdk-usage/`](examples/sdk-usage/) |
| Understand custom transports | [`examples/serial-loopback/`](examples/serial-loopback/) |
| Talk to an `aether-lite` device over serial | [`examples/serial-sensor/`](examples/serial-sensor/) |
| Start from a firmware template | [`examples/mcu-firmware/`](examples/mcu-firmware/) |
| Explore the full learning path | [Examples guide](examples/README.md) |

Other integration paths are available, but they are intentionally not the first
thing a newcomer has to choose between.

## Build And Test

```bash
python3 build.py              # build library
python3 build.py -e           # build library + examples
python3 build.py -t           # build + run all tests
python3 build.py -c -t        # clean rebuild + full test pass
```

Direct CMake:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

Sanitizers:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAETHER_SANITIZERS=address,undefined
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAETHER_SANITIZERS=thread
```

## Documentation

Use the docs in this order:

- [Documentation guide](doc/README.md)
- [Aether in 5 Minutes](doc/AetherIn5Minutes.md)
- [Examples guide](examples/README.md)
- [Architecture guide](doc/architecture-guide.md)

Deep references:

- [High-level design](doc/aether-hld.md)
- [Low-level design](doc/aether-lld.md)
- [ipcgen high-level design](doc/ipcgen-hld.md)
- [ipcgen low-level design](doc/ipcgen-lld.md)
- [Project vision](doc/aether-vision.md)

## License

MIT — see [LICENSE](LICENSE).
