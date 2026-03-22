# aether

A lightweight IPC framework for Linux that lets you define service interfaces
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

Communicating between processes on Linux usually means choosing between
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
- Linux
- Python 3 (for build script and code generator)

### Clone and build

```bash
git clone --recursive https://github.com/Mrunmoy/aether
cd aether
python3 build.py -t    # build + run all tests
```

### Generate code from IDL

```bash
cd tools && python3 -m ipcgen path/to/YourService.idl --outdir path/to/gen
```

This generates five files:

| File | Purpose |
|------|---------|
| `YourServiceTypes.h` | Enums and structs (shared between server and client) |
| `server/YourService.h` | Server class with pure virtual handlers to implement |
| `server/YourService.cpp` | Request dispatch and notification senders |
| `client/YourService.h` | Client class with typed RPC methods |
| `client/YourService.cpp` | Request marshaling and notification dispatch |

### Link with CMake

```cmake
# Server
add_executable(my_server my_server.cpp gen/server/YourService.cpp)
target_include_directories(my_server PRIVATE gen/server/)
target_link_libraries(my_server aether)

# Client
add_executable(my_client my_client.cpp gen/client/YourService.cpp)
target_include_directories(my_client PRIVATE gen/client/)
target_link_libraries(my_client aether)
```

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

## Two ways to use

| | Low-level API | Code generation |
|---|---|---|
| **How** | Subclass `ServiceBase` / `ClientBase` directly | Write IDL, run `ipcgen` |
| **Payloads** | Raw `std::vector<uint8_t>` | Typed C++ parameters |
| **Serialization** | Manual | Automatic |
| **Best for** | Custom protocols, prototyping | Production services |

Both modes support optional [vortex](https://github.com/Mrunmoy/Vortex)
integration for single-threaded event-driven operation.

## Key properties

- **Shared memory transport** — data flows through lock-free ring buffers, not sockets
- **Embedded-friendly** — no `std::string`, no heap allocations in the transport layer
- **Fixed-size wire format** — all types are POD; structs use `memcpy` serialization
- **RunLoop support** — optionally drive server and client from a shared event loop with zero internal threads
- **Notifications** — server can broadcast to all connected clients; clients receive typed callbacks

## Examples

| Example | Description |
|---------|-------------|
| [`examples/sdk-usage/`](examples/sdk-usage/) | **Start here** — C++ project using the SDK release artifact (RAII wrappers, RPC, notifications) |
| [`examples/c-echo/`](examples/c-echo/) | Minimal C echo server + client using the C API |
| [`examples/echo/`](examples/echo/) | Low-level C++ (`ServiceBase`/`ClientBase`) + ipcgen codegen (source build) |
| [`examples/exhaust-analyzer/`](examples/exhaust-analyzer/) | Qt-based codegen example with structs and notifications |

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

268 tests total: 101 C++ (Google Test) + 167 Python (pytest).

```bash
python3 build.py -t    # runs everything
```

## C API

Aether ships a stable C API (`aether_ipc.h`) with opaque handles for
ABI-safe distribution. This is the public interface for SDK consumers.
See [`examples/c-echo/`](examples/c-echo/) for a minimal C example, or
[`examples/sdk-usage/`](examples/sdk-usage/) for a full C++ project
using the SDK.

## Using the SDK release

Pre-built SDK tarballs are published as
[GitHub Release](https://github.com/Mrunmoy/ms-ipc/releases) assets.
The SDK exposes a stable **C API** (`aether_ipc.h`) so your C++ project
isn't tied to a specific compiler or standard library version.

### Download and extract

```bash
gh release download v1.1.0 -R Mrunmoy/ms-ipc -p '*.tar.gz'
tar xzf aether-sdk-*-linux-x86_64.tar.gz
```

The tarball contains:

```
aether-sdk-1.1.0-linux-x86_64/
├── include/
│   └── aether_ipc.h          # C API header (only public header)
├── lib/
│   ├── libaether.a            # Fat static archive (all deps bundled)
│   ├── libaether.so           # Shared library
│   ├── pkgconfig/aether.pc
│   └── cmake/aether/          # find_package() support
└── example/                   # C echo server + client
```

### Write C++ code using the C API

The recommended pattern is to wrap the C handles in RAII classes. Define your
protocol in a shared header, then write typed C++ wrappers:

```cpp
#include "aether_ipc.h"
#include <cstdio>
#include <cstring>

// RAII wrapper around aether_client_t
class MyClient {
public:
    MyClient(const char *name) {
        aether_client_create(name, nullptr, nullptr, &m_cli);
    }
    ~MyClient() { aether_client_destroy(m_cli); }

    bool connect() { return aether_client_connect(m_cli) == AETHER_SUCCESS; }

    // Typed RPC — hides raw byte marshaling
    int getTemperature(float *celsius) {
        uint8_t resp[64]{};
        uint32_t resp_len = 0;
        int rc = aether_client_call(m_cli, SVC_ID, METHOD_GET_TEMP,
                                    nullptr, 0, resp, sizeof(resp),
                                    &resp_len, 2000);
        if (rc == AETHER_SUCCESS)
            std::memcpy(celsius, resp, sizeof(float));
        return rc;
    }

private:
    aether_client_t m_cli = nullptr;
};
```

> See [`examples/sdk-usage/`](examples/sdk-usage/) for a complete working
> project with server, client, notifications, and a CMakeLists.txt you can
> copy into your own project.

### Link with CMake

```cmake
set(AETHER_SDK "/path/to/aether-sdk-1.1.0-linux-x86_64" CACHE PATH "Aether SDK")

add_executable(my_app main.cpp)
target_include_directories(my_app PRIVATE ${AETHER_SDK}/include)
target_link_libraries(my_app ${AETHER_SDK}/lib/libaether.a stdc++ pthread)
target_compile_features(my_app PRIVATE cxx_std_17)
```

### Link with pkg-config

```bash
export PKG_CONFIG_PATH=/path/to/aether-sdk-1.1.0-linux-x86_64/lib/pkgconfig

g++ -std=c++17 -o my_app main.cpp $(pkg-config --cflags --libs aether) -lpthread
```

### Link manually

```bash
SDK=/path/to/aether-sdk-1.1.0-linux-x86_64

g++ -std=c++17 -o my_app main.cpp \
    -I$SDK/include $SDK/lib/libaether.a -lpthread
```

> **Note:** The fat static archive (`libaether.a`) bundles Ouroboros and
> Vortex, so no additional library files are needed.

### Why a C API for a C++ library?

C++ libraries bake in a specific ABI — name mangling, vtable layout,
`std::string`/`std::vector` internals — which change between compiler
versions and standard library implementations. The C API with opaque handles
gives you a stable binary interface: build the SDK once, use it from any
C++17 toolchain. The trade-off is writing a thin RAII wrapper on the consumer
side (as shown above), but you never have to rebuild the SDK when you upgrade
your compiler.

## Dependencies

| Dependency | Purpose |
|-----------|---------|
| [ouroboros](https://github.com/Mrunmoy/Ouroboros) | Lock-free SPSC ring buffers (submodule) |
| [vortex](https://github.com/Mrunmoy/Vortex) | Event loop for fd watching (submodule) |
| [Google Test](https://github.com/google/googletest) v1.14.0 | C++ testing (submodule, tests only) |

## License

MIT — see [LICENSE](LICENSE).
