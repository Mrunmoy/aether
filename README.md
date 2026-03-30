# Aether

![Build](https://github.com/Mrunmoy/aether/actions/workflows/ci.yml/badge.svg?branch=main&event=push)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-informational)

**Shared-memory IPC framework with IDL code generation for C++, Python, and C99.**

Aether moves data between processes through lock-free ring buffers in shared
memory. You describe your service interface in an IDL file, and the code
generator produces typed C++ server skeletons and client stubs. The same
24-byte wire protocol also works over serial and USB for desktop-to-device
communication with microcontrollers.

## Quick Start

**Prerequisites:** C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+), CMake 3.14+,
Python 3, Linux or macOS or Windows.

```bash
git clone --recursive https://github.com/Mrunmoy/aether.git
cd aether
python3 build.py -e        # build library + examples
```

**Try it — run the echo example in two terminals:**

Terminal 1 (server):
```bash
build/examples/echo/echo_server
```
```
Echo server running (Ctrl-C to stop)...
```

Terminal 2 (client):
```bash
build/examples/echo/echo_client
```
```
Connected to echo service.
[client] sent: "Hello"  ->  received: "Hello"
[client] sent: "World"  ->  received: "World"
[client] sent: "aether works!"  ->  received: "aether works!"
Disconnected.
```

You just made two processes communicate through shared memory. The server
echoed back everything the client sent, with data flowing through lock-free
ring buffers mapped into both processes.

## How It Works

The echo example above uses the raw `ServiceBase`/`ClientBase` API. For real
services, you describe the interface in IDL and let the code generator handle
serialization and dispatch.

### 1. Write the IDL

```idl
service DeviceMonitor
{
    [method=1] int GetDeviceCount([out] uint32 count);
    [method=2] int GetDeviceInfo([in] uint32 deviceId, [out] DeviceInfo info);
};

notifications DeviceMonitor
{
    [notify=1] void DeviceConnected([in] DeviceInfo info);
};
```

### 2. Generate code

```bash
python3 -m ipcgen examples/echo/DeviceMonitor.idl --outdir gen/
```

This produces five files: `DeviceMonitorTypes.h`, `server/DeviceMonitor.h`,
`server/DeviceMonitor.cpp`, `client/DeviceMonitor.h`, `client/DeviceMonitor.cpp`.

### 3. Implement your service

**Server** — subclass the generated skeleton, implement the handlers:

```cpp
class MyDeviceService : public aether::ipc::DeviceMonitor
{
protected:
    int handleGetDeviceCount(uint32_t *count) override
    {
        *count = m_devices.size();
        return IPC_SUCCESS;
    }
};

MyDeviceService service("device_monitor");
service.start();
```

**Client** — call typed methods:

```cpp
aether::ipc::DeviceMonitor client("device_monitor");
client.connect();

uint32_t count = 0;
client.GetDeviceCount(&count);
client.disconnect();
```

The generated code handles serialization, dispatch, and RPC correlation.
The [`examples/echo/`](examples/echo/) directory shows this workflow in full
with build commands and tests.

## Learn More

| Document | What it covers |
|----------|----------------|
| [Aether in 5 Minutes](doc/AetherIn5Minutes.md) | Mental model: what's generated, what's runtime, what you write. Threading, errors, common pitfalls. |
| [Examples guide](examples/README.md) | Which example to start with and how the examples are organized. |
| [Architecture guide](doc/architecture-guide.md) | Visual walkthrough of the five runtime layers with diagrams. |

## Other Integration Paths

The quickstart above uses the default C++ codegen path. Other paths exist for
specific scenarios:

| Path | When to use | Start here |
|------|-------------|------------|
| **C API** | SDK distribution or non-C++ consumers | [`examples/sdk-usage/`](examples/sdk-usage/) |
| **Python client** | Python GUI or script talking to a C++ server | `python3 -m ipcgen --backend python` |
| **aether-lite** | Bare-metal MCU (Cortex-M, AVR) with UART/USB | [`examples/mcu-firmware/`](examples/mcu-firmware/) |
| **Serial transport** | Desktop-to-device over a byte stream | [`examples/serial-sensor/`](examples/serial-sensor/) |

## Building

```bash
python3 build.py              # build library
python3 build.py -t           # build + run all tests
python3 build.py -c -t        # clean rebuild + full test pass
python3 build.py -e           # build + examples

# CMake directly
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DMS_IPC_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure

# Sanitizers
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAETHER_SANITIZERS=address,undefined
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAETHER_SANITIZERS=thread
```

## Documentation

| Document | Contents |
|----------|----------|
| [AetherIn5Minutes.md](doc/AetherIn5Minutes.md) | Mental model, threading, ownership, common failures, anti-patterns |
| [aether-hld.md](doc/aether-hld.md) | High-level design — architecture and components |
| [aether-lld.md](doc/aether-lld.md) | Low-level design — APIs, wire protocol, threading |
| [ipcgen-hld.md](doc/ipcgen-hld.md) | Code generator — IDL grammar, backend architecture |
| [ipcgen-lld.md](doc/ipcgen-lld.md) | Code generator — module APIs and code generation |
| [architecture-guide.md](doc/architecture-guide.md) | Visual architecture walkthrough with diagrams |
| [aether-vision.md](doc/aether-vision.md) | Project vision, platform requirements, language roles |

## License

MIT — see [LICENSE](LICENSE).
