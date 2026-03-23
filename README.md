# aether

A lightweight IPC framework for Linux and macOS that lets you define service interfaces
in a simple IDL and generates type-safe C++ server and client code. Built on
shared memory for high performance, with zero manual serialization.

## Background

This project grew out of experience building multi-process systems for
professional media and embedded hardware, where multiple applications need
to communicate reliably over IPC — exchanging device status, sensor data,
configuration, and real-time events. Existing solutions were either too
heavyweight (D-Bus, gRPC) or too manual (custom socket protocols with
hand-rolled serialization).

aether is an attempt to hit the sweet spot: a minimal framework that handles
the boring parts (connection setup, shared memory, framing, marshaling) while
staying small, fast, and predictable enough for embedded and latency-sensitive
use cases. The IDL code generator was added to eliminate the most error-prone
part of IPC development — manually keeping server and client message formats
in sync.

## Why aether

Communicating between processes on the desktop usually means choosing between
heavyweight frameworks (D-Bus, gRPC) or hand-rolling sockets with custom
serialization. Both paths lead to boilerplate, fragile byte packing, and
wasted time debugging wire format mismatches.

aether solves this by letting you describe your service interface once in a
simple IDL file, then generating all the marshaling and dispatch code for
you. On the server side, you implement virtual handler methods. On the client
side, you call typed C++ methods. The framework handles connection management,
shared memory setup, serialization, and notification broadcast — so you can
focus on your application logic instead of plumbing.

It is designed for embedded and systems-level use cases where you need:
- Low-latency, high-throughput data transfer between processes
- Compile-time type safety without runtime overhead
- No external runtime dependencies (no D-Bus daemon, no protobuf library)
- A small, predictable footprint (no heap allocations in the transport layer)

## How it works

1. You define your service interface in an IDL file
2. `ipcgen` generates typed C++ classes for both server and client
3. You implement the server handlers and use the typed client API — no raw bytes

```idl
service TemperatureSensor
{
    [method=1]
    int GetTemperature([out] float32 celsius);

    [method=2]
    int SetThreshold([in] float32 high, [in] float32 low);
};

notifications TemperatureSensor
{
    [notify=1]
    void OverTemperature([in] float32 celsius);
};
```

**Server** — implement pure virtual handlers:

```cpp
class MyTempSensor : public aether::ipc::TemperatureSensor
{
protected:
    int handleGetTemperature(float *celsius) override
    {
        *celsius = readSensor();
        return IPC_SUCCESS;
    }

    int handleSetThreshold(float high, float low) override
    {
        m_high = high;
        m_low = low;
        return IPC_SUCCESS;
    }
};

MyTempSensor service("temp_sensor");
service.start();
```

**Client** — call typed methods:

```cpp
aether::ipc::TemperatureSensor client("temp_sensor");
client.connect();

float temp = 0;
int rc = client.GetTemperature(&temp);

client.SetThreshold(85.0f, -10.0f);
client.disconnect();
```

No message IDs, no serialization, no raw byte buffers.

## Getting started

### Prerequisites

- C++17 compiler (GCC 7+, Clang 5+)
- CMake 3.14+
- Linux or macOS
- Python 3 (for build script and code generator)

### Clone and build

```bash
git clone --recursive https://github.com/Mrunmoy/ms-ipc.git aether
cd aether
python3 build.py -t    # build + run all tests
```

### Generate code from IDL

```bash
cd tools && python3 -m ipcgen path/to/YourService.idl --outdir path/to/gen
```

Use `--backend c_api` when building against the SDK release (see
[Way 1](#way-1-pre-built-sdk-recommended-for-most-users) below).

## IDL types

| IDL type | C++ type | Size |
|----------|----------|------|
| `uint8` | `uint8_t` | 1 byte |
| `uint16` | `uint16_t` | 2 bytes |
| `uint32` | `uint32_t` | 4 bytes |
| `uint64` | `uint64_t` | 8 bytes |
| `int8` | `int8_t` | 1 byte |
| `int16` | `int16_t` | 2 bytes |
| `int32` | `int32_t` | 4 bytes |
| `int64` | `int64_t` | 8 bytes |
| `float32` | `float` | 4 bytes |
| `float64` | `double` | 8 bytes |
| `bool` | `bool` | 1 byte |
| `T[N]` | `std::array<T, N>` | N x sizeof(T) |
| `string[N]` | `char[N+1]` | N+1 bytes |

You can also define **enums** and **structs** in the IDL:

```idl
enum DeviceType { Unknown = 0, USB = 1, Bluetooth = 2 };

struct DeviceInfo
{
    uint32 id;
    DeviceType type;
    uint8[6] serial;
    string[64] name;
};
```

## Integrating aether into your project

There are two ways to use aether. Both give you the same developer
experience — write an IDL, generate code, subclass, implement handlers.

---

### Way 1: Pre-built SDK (recommended for most users)

Download a pre-built release tarball. Your project only needs `aether_ipc.h`
and `libaether.a` — no source tree, no submodules, no building aether yourself.
The C API boundary means it works with **any C++17 toolchain** regardless of
which compiler the SDK was built with.

**Step by step:**

```bash
# 1. Download the SDK from GitHub Releases
gh release download v1.1.0 -R Mrunmoy/ms-ipc -p '*.tar.gz'
tar xzf aether-sdk-*-linux-x86_64.tar.gz

# 2. Clone the repo just to get ipcgen (the code generator)
git clone https://github.com/Mrunmoy/ms-ipc.git aether-tools
# (You only need the tools/ipcgen/ directory)

# 3. Write your service IDL
cat > MyService.idl << 'EOF'
service MyService
{
    [method=1]
    int DoSomething([in] uint32 input, [out] uint32 result);
};
EOF

# 4. Generate C++ code using the c_api backend
cd aether-tools/tools && python3 -m ipcgen /path/to/MyService.idl \
    --outdir /path/to/your-project/gen --backend c_api

# 5. Implement your server/client (subclass the generated classes)
# 6. Build against the SDK
```

**CMakeLists.txt:**
```cmake
set(AETHER_SDK "/path/to/aether-sdk" CACHE PATH "Aether SDK")

add_executable(my_server my_server.cpp gen/server/MyService.cpp)
target_include_directories(my_server PRIVATE gen gen/server ${AETHER_SDK}/include)
target_link_libraries(my_server ${AETHER_SDK}/lib/libaether.a stdc++ pthread)
target_compile_features(my_server PRIVATE cxx_std_17)
```

**SDK tarball contents:**
```
aether-sdk-1.1.0-linux-x86_64/
├── include/aether_ipc.h       # Stable C API (only public header)
├── lib/
│   ├── libaether.a            # Fat static archive (all deps bundled)
│   ├── libaether.so           # Shared library
│   ├── pkgconfig/aether.pc    # pkg-config support
│   └── cmake/aether/          # find_package() support
└── example/                   # C echo example
```

> See [`examples/sdk-usage/`](examples/sdk-usage/) for a complete working
> project with IDL, generated code, server, client, and CMakeLists.txt.

---

### Way 2: Git submodule (full source access)

Add aether as a git submodule in your project. You build everything from
source and link directly against the C++ CMake target. This gives you access
to all internals — `ServiceBase`, `ClientBase`, RunLoop integration, etc.

**Step by step:**

```bash
# 1. Add aether as a submodule
git submodule add --recursive https://github.com/Mrunmoy/ms-ipc.git deps/aether

# 2. Write your service IDL (same as Way 1)

# 3. Generate C++ code using the default backend
cd deps/aether/tools && python3 -m ipcgen /path/to/MyService.idl \
    --outdir /path/to/your-project/gen
```

**CMakeLists.txt:**
```cmake
add_subdirectory(deps/aether)

add_executable(my_server my_server.cpp gen/server/MyService.cpp)
target_include_directories(my_server PRIVATE gen/server)
target_link_libraries(my_server aether)
```

This backend generates classes that inherit from `ServiceBase`/`ClientBase`
directly, with optional [Vortex](https://github.com/Mrunmoy/Vortex) RunLoop
integration for single-threaded event-driven operation.

> See [`examples/echo/`](examples/echo/) for a source-build codegen example.

---

### Which way should I choose?

| | Way 1: SDK release | Way 2: Git submodule |
|---|---|---|
| **Setup** | Download tarball + clone ipcgen | `git submodule add` |
| **Build aether?** | No — pre-built binary | Yes — built as part of your project |
| **ABI safety** | ✅ Works with any C++17 toolchain | ❌ Must match compiler versions |
| **RunLoop support** | Not available | ✅ Full Vortex integration |
| **Generated code uses** | `aether_ipc.h` (C API handles) | `ServiceBase`/`ClientBase` (C++ internals) |
| **User-facing API** | Virtual handlers + typed methods | Virtual handlers + typed methods |
| **Best for** | Shipping products, cross-team use | Single-team projects built from one tree |

## Key properties

- **Shared memory transport** — data flows through lock-free ring buffers, not sockets
- **Transport abstraction** — `ITransport` interface enables serial, USB, and network transports alongside SHM
- **Embedded-friendly** — no `std::string`, no heap allocations in the transport layer
- **aether-lite** — standalone C99 bare-metal runtime for ARM Cortex-M / AVR targets (same wire protocol)
- **Fixed-size wire format** — all types are POD; structs use `memcpy` serialization
- **RunLoop support** — optionally drive server and client from a shared event loop with zero internal threads
- **Notifications** — server can broadcast to all connected clients; clients receive typed callbacks

## Examples

> **Not sure where to start?** See [`examples/README.md`](examples/README.md)
> for guidance on which example fits your use case.

| Example | Audience | Description |
|---------|----------|-------------|
| [`examples/sdk-usage/`](examples/sdk-usage/) | SDK consumers | IDL → `ipcgen --backend c_api` → C++ server/client using the SDK release |
| [`examples/echo/`](examples/echo/) | Source builders | IDL → `ipcgen` (default backend) → `ServiceBase`/`ClientBase` |
| [`examples/c-echo/`](examples/c-echo/) | C developers | Raw C API echo server + client (no codegen) |
| [`examples/exhaust-analyzer/`](examples/exhaust-analyzer/) | Source builders | Qt5 GUI with structs, enums, and notifications |

## Documentation

| Document | Contents |
|----------|----------|
| [example/README.md](example/README.md) | Usage examples and tutorial |
| [doc/aether-hld.md](doc/aether-hld.md) | High-level design — architecture and components |
| [doc/aether-lld.md](doc/aether-lld.md) | Low-level design — APIs, wire protocol, threading |
| [doc/ipcgen-hld.md](doc/ipcgen-hld.md) | High-level design — code generator |
| [doc/ipcgen-lld.md](doc/ipcgen-lld.md) | Low-level design — module APIs, code generation |

## Building

```bash
python3 build.py              # build only
python3 build.py -c           # clean build
python3 build.py -t           # build + run all tests (C++ + Python)
python3 build.py -e           # build + examples
python3 build.py -c -t        # clean build + tests
```

Or with CMake directly:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

## Tests

268 tests total: 120 C++ (Google Test) + 167 Python (pytest).

```bash
python3 build.py -t    # runs everything
```

## Dependencies

| Dependency | Purpose |
|-----------|---------|
| [ouroboros](https://github.com/Mrunmoy/Ouroboros) | Lock-free SPSC ring buffers (submodule) |
| [vortex](https://github.com/Mrunmoy/Vortex) | Event loop for fd watching (submodule) |
| [Google Test](https://github.com/google/googletest) v1.14.0 | C++ testing (submodule, tests only) |

## Architecture

```
┌──────────────────────────────────────────────────────────┐
│  Generated code (ipcgen)                                 │
│  ├── C++ server/client (ServiceBase / ClientBase)        │
│  ├── C API (aether_ipc.h)                                │
│  └── C bare-metal (planned: c_bare_metal backend)        │
├──────────────────────────────────────────────────────────┤
│  Runtime                                                 │
│  ├── ServiceBase / ClientBase (SHM transport)            │
│  ├── TransportClientBase (ITransport-based connections)  │
│  └── aether-lite (C99 bare-metal, same wire protocol)    │
├──────────────────────────────────────────────────────────┤
│  Transport layer                                         │
│  ├── ITransport interface                                │
│  ├── SHM: Connection + ring buffers + UDS signals        │
│  └── Serial: (planned) termios + CRC32 framing           │
├──────────────────────────────────────────────────────────┤
│  Platform: Linux abstractions (UDS, memfd, SCM_RIGHTS)   │
└──────────────────────────────────────────────────────────┘
```

## License

MIT — see [LICENSE](LICENSE).
