# Using the Aether SDK in a C++ Project

The Aether SDK ships a **C API** (`aether_ipc.h`) for ABI stability — it works
with any C++17 compiler regardless of standard library version. This example
shows how to wrap that C API in idiomatic C++ classes for a clean developer
experience.

## What's in the SDK tarball

```
aether-sdk-1.1.0-linux-x86_64/
├── include/
│   └── aether_ipc.h       # The only public header (stable C API)
├── lib/
│   ├── libaether.a         # Fat static archive (all deps bundled)
│   ├── libaether.so        # Shared library
│   ├── pkgconfig/aether.pc
│   └── cmake/aether/       # find_package() support
└── example/                # C echo example (shipped in tarball)
```

## Step by step

### 1. Download and extract the SDK

```bash
# From GitHub Releases
gh release download v1.1.0 -R Mrunmoy/ms-ipc -p '*.tar.gz'
tar xzf aether-sdk-*-linux-x86_64.tar.gz
```

### 2. Define your protocol

Agree on service/method/notification IDs and payload layouts between server
and client. Use a shared header:

```cpp
// protocol.h
#pragma once
#include <cstdint>

namespace temp {
    constexpr uint32_t kServiceId         = 0x0001;
    constexpr uint32_t kMethodGetTemp     = 1;
    constexpr uint32_t kMethodSetThreshold = 2;
    constexpr uint32_t kNotifyOverTemp    = 1;

    struct Thresholds {
        float high;
        float low;
    };
}
```

### 3. Implement the server

See [`temp_server.cpp`](temp_server.cpp) — wraps `aether_service_t` in an
RAII class, dispatches requests by method ID, broadcasts notifications.

### 4. Implement the client

See [`temp_client.cpp`](temp_client.cpp) — wraps `aether_client_t` in an
RAII class, provides typed RPC methods, receives notifications via callback.

### 5. Build

```bash
cmake -B build -DAETHER_SDK=/path/to/aether-sdk-1.1.0-linux-x86_64
cmake --build build
```

### 6. Run

```bash
# Terminal 1
./build/temp_server

# Terminal 2
./build/temp_client
```

Expected output:

```
# Server:
[server] Temperature sensor running. Ctrl+C to stop.
[server] GetTemperature → 72.5°C
[server] SetThreshold(high=50.0, low=-10.0)
[server] 72.5°C exceeds threshold — broadcasting notification

# Client:
[client] Connected to temp_sensor
[client] Current temperature: 72.5°C
[client] Threshold set to [50.0, -10.0]°C
[client] ⚠ Over-temperature: 72.5°C!
[client] Done.
```

## Why a C API?

C++ libraries bake in a specific **ABI** — name mangling, vtable layout,
`std::string`/`std::vector` internals — all of which change between compiler
versions and standard library implementations. A C API with opaque handles
avoids all of that:

- Build the SDK once, use it from GCC 9 or GCC 14, libstdc++ or libc++
- No header-only template leakage across the boundary
- The `extern "C"` functions have a stable calling convention

The trade-off is that you write a thin wrapper on the consumer side (as shown
in these examples), but you never have to rebuild the SDK when you upgrade
your toolchain.
