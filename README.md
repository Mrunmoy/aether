# ms-ipc

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

ms-ipc is an attempt to hit the sweet spot: a minimal framework that handles
the boring parts (connection setup, shared memory, framing, marshaling) while
staying small, fast, and predictable enough for embedded and latency-sensitive
use cases. The IDL code generator was added to eliminate the most error-prone
part of IPC development — manually keeping server and client message formats
in sync.

## Why ms-ipc

Communicating between processes on Linux usually means choosing between
heavyweight frameworks (D-Bus, gRPC) or hand-rolling sockets with custom
serialization. Both paths lead to boilerplate, fragile byte packing, and
wasted time debugging wire format mismatches.

ms-ipc solves this by letting you describe your service interface once in a
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
class MyTempSensor : public ms::ipc::TemperatureSensor
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
ms::ipc::TemperatureSensor client("temp_sensor");
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
git clone --recursive https://github.com/Mrunmoy/ms-ipc
cd ms-ipc
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
target_link_libraries(my_server ms-ipc)

# Client
add_executable(my_client my_client.cpp gen/client/YourService.cpp)
target_include_directories(my_client PRIVATE gen/client/)
target_link_libraries(my_client ms-ipc)
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

Both modes support optional [ms-runloop](https://github.com/Mrunmoy/ms-runloop)
integration for single-threaded event-driven operation.

## Key properties

- **Shared memory transport** — data flows through lock-free ring buffers, not sockets
- **Embedded-friendly** — no `std::string`, no heap allocations in the transport layer
- **Fixed-size wire format** — all types are POD; structs use `memcpy` serialization
- **RunLoop support** — optionally drive server and client from a shared event loop with zero internal threads
- **Notifications** — server can broadcast to all connected clients; clients receive typed callbacks

## Examples

See [example/README.md](example/README.md) for self-contained, copy-paste examples:

- **Low-level echo** — raw `ServiceBase` / `ClientBase` usage
- **Code generation tutorial** — step-by-step `ipcgen` walkthrough
- **Scalars & enums** — temperature sensor
- **Fixed-length arrays** — multi-channel ADC
- **Bounded strings** — alarm system
- **Convenience wrappers** — chaining count + list RPCs into `std::vector`

## Documentation

| Document | Contents |
|----------|----------|
| [example/README.md](example/README.md) | Usage examples and tutorial |
| [doc/ms-ipc-hld.md](doc/ms-ipc-hld.md) | High-level design — architecture and components |
| [doc/ms-ipc-lld.md](doc/ms-ipc-lld.md) | Low-level design — APIs, wire protocol, threading |
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

217 tests total: 61 C++ (Google Test) + 156 Python (pytest).

```bash
python3 build.py -t    # runs everything
```

## Dependencies

| Dependency | Purpose |
|-----------|---------|
| [ms-ringbuffer](https://github.com/Mrunmoy/ms-ringbuffer) | Lock-free SPSC ring buffers (submodule) |
| [ms-runloop](https://github.com/Mrunmoy/ms-runloop) | Event loop for fd watching (submodule) |
| [Google Test](https://github.com/google/googletest) v1.14.0 | C++ testing (submodule, tests only) |

## License

MIT — see [LICENSE](LICENSE).
