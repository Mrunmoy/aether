# Examples and Tutorial

This guide walks through using ms-ipc at two levels:

1. **Low-level** — subclass ServiceBase/ClientBase directly, handle raw bytes
2. **Code generation** — write an IDL file, generate typed C++ classes with ipcgen

## Building

```bash
# From the project root:
python3 build.py -e           # build examples
python3 build.py -t           # build + run all tests (includes examples)

# Or with CMake directly:
cmake -B build -DMS_IPC_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

---

# Part 1: Low-level API (Echo example)

The echo example shows the raw ServiceBase/ClientBase API. You work directly
with `std::vector<uint8_t>` payloads and numeric message IDs.

## echo_server

**File:** `echo_server.cpp`

### Step 1: Subclass ServiceBase

Override `onRequest()` to handle incoming requests. The base class handles
everything else: listening, accepting clients, threading, and signaling.

```cpp
#include "ServiceBase.h"

class EchoService : public ms::ipc::ServiceBase
{
protected:
    int onRequest(uint32_t messageId, const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override
    {
        // Echo: copy request payload into response.
        *response = request;
        return IPC_SUCCESS;
    }
};
```

### Step 2: Start the service

```cpp
EchoService service("echo");       // creates UDS socket at \0ipc_echo
service.start();                    // spawns accept thread
// ... wait for Ctrl-C ...
service.stop();                     // joins all threads, closes connections
```

## echo_client

**File:** `echo_client.cpp`

### Step 1: Connect

```cpp
ms::ipc::ClientBase client("echo");
client.connect();                   // handshake: creates shared memory, spawns receiver thread
```

### Step 2: Make RPC calls

```cpp
std::vector<uint8_t> request(msg, msg + strlen(msg));
std::vector<uint8_t> response;
int rc = client.call(1, 1, request, &response);    // serviceId=1, messageId=1
```

### Step 3: Disconnect

```cpp
client.disconnect();                // joins receiver, fails pending calls, closes connection
```

## Running the echo example

```bash
# Terminal 1:
./build/example/echo_server

# Terminal 2:
./build/example/echo_client
```

## RunLoop mode

Both ServiceBase and ClientBase accept an optional `ms::RunLoop*`. When
provided, they register fds on the RunLoop instead of spawning internal threads.

```cpp
ms::RunLoop loop;
loop.init("app");

EchoService service("echo", &loop);
service.start();                    // no threads — listenFd registered on RunLoop

std::thread loopThread([&loop] { loop.run(); });

// ... clients can connect and call from other threads ...

service.stop();
loop.stop();
loopThread.join();
```

---

# Part 2: Code Generation Tutorial (ipcgen)

For production services, ipcgen generates typed C++ classes from an IDL file.
You get compile-time type safety, automatic serialization, and no raw byte
manipulation. This tutorial walks through the full workflow step by step.

## Step 1: Write the IDL file

Create `DeviceMonitor.idl` describing your service interface:

```idl
service DeviceMonitor
{
    [method=1]
    int GetDeviceCount([out] uint32* count);

    [method=2]
    int GetDeviceStatus([in] uint32 deviceId, [out] uint32* status);
};

notifications DeviceMonitor
{
    [notify=1]
    void DeviceConnected([in] uint32 deviceId);

    [notify=2]
    void DeviceDisconnected([in] uint32 deviceId);
};
```

### IDL syntax reference

**Service block** — defines RPC methods that clients can call:

```idl
service <Name>
{
    [method=<id>]
    int <MethodName>([in] <type> <param>, ..., [out] <type>* <param>, ...);
};
```

- Methods always return `int` (error code: `IPC_SUCCESS` on success)
- `[in]` params are sent from client to server (pass by value)
- `[out]` params are returned from server to client (pass by pointer)
- Each method has a unique numeric ID (`[method=N]`)

**Notifications block** — defines server-to-client broadcasts:

```idl
notifications <Name>
{
    [notify=<id>]
    void <NotificationName>([in] <type> <param>, ...);
};
```

- Notifications return `void` (fire-and-forget, broadcast to all clients)
- Only `[in]` params allowed (data flows server to client)
- Each notification has a unique numeric ID (`[notify=N]`)

**Supported types:**

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

## Step 2: Run the code generator

```bash
cd tools && python3 -m ipcgen ../example/DeviceMonitor.idl --outdir ../example/gen
```

Output:

```
  wrote ../example/gen/server/DeviceMonitor.h
  wrote ../example/gen/server/DeviceMonitor.cpp
  wrote ../example/gen/client/DeviceMonitor.h
  wrote ../example/gen/client/DeviceMonitor.cpp

Generated 4 files for service 'DeviceMonitor' (serviceId=0x00fefaf3)
```

The `serviceId` is automatically derived from the service name using FNV-1a
hash — no manual ID assignment needed.

## Step 3: Understand the generated server code

### gen/server/DeviceMonitor.h

```cpp
// Auto-generated by ipcgen — do not edit.
#pragma once
#include "ServiceBase.h"
#include <cstdint>
#include <cstring>
#include <vector>

namespace ms::ipc
{

class DeviceMonitor : public ServiceBase
{
public:
    using ServiceBase::ServiceBase;

    static constexpr uint32_t kServiceId = 0x00fefaf3u;

protected:
    virtual int handleGetDeviceCount(uint32_t *count) = 0;
    virtual int handleGetDeviceStatus(uint32_t deviceId, uint32_t *status) = 0;

    int notifyDeviceConnected(uint32_t deviceId);
    int notifyDeviceDisconnected(uint32_t deviceId);

    int onRequest(uint32_t messageId, const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override;
};

} // namespace ms::ipc
```

What ipcgen generates for the server:

| Member | Purpose |
|--------|---------|
| `kServiceId` | Auto-derived hash of `"DeviceMonitor"` — used in all frames |
| `handleGetDeviceCount()` | **Pure virtual** — you implement this |
| `handleGetDeviceStatus()` | **Pure virtual** — you implement this |
| `notifyDeviceConnected()` | **Concrete** — call this to broadcast to all clients |
| `notifyDeviceDisconnected()` | **Concrete** — call this to broadcast to all clients |
| `onRequest()` | **Generated override** — dispatches by messageId, handles marshal/unmarshal |

### gen/server/DeviceMonitor.cpp

The generated `onRequest()` dispatches incoming requests by messageId:

```cpp
int DeviceMonitor::onRequest(uint32_t messageId,
                             const std::vector<uint8_t> &request,
                             std::vector<uint8_t> *response)
{
    switch (messageId)
    {
    case 1: // GetDeviceCount
    {
        uint32_t count;
        int _rc = handleGetDeviceCount(&count);
        response->resize(sizeof(count));
        std::memcpy(response->data(), &count, sizeof(count));
        return _rc;
    }
    case 2: // GetDeviceStatus
    {
        uint32_t deviceId;
        std::memcpy(&deviceId, request.data() + 0, sizeof(deviceId));
        uint32_t status;
        int _rc = handleGetDeviceStatus(deviceId, &status);
        response->resize(sizeof(status));
        std::memcpy(response->data(), &status, sizeof(status));
        return _rc;
    }
    default:
        return IPC_ERR_INVALID_METHOD;
    }
}
```

For each method it:
1. Unmarshals `[in]` params from the request buffer using `memcpy`
2. Calls your `handleXxx()` virtual method
3. Marshals `[out]` params into the response buffer
4. Returns your error code

Notification senders serialize params and call `sendNotify()`:

```cpp
int DeviceMonitor::notifyDeviceConnected(uint32_t deviceId)
{
    std::vector<uint8_t> payload(sizeof(deviceId));
    std::memcpy(payload.data(), &deviceId, sizeof(deviceId));
    return sendNotify(kServiceId, 1, payload.data(),
                      static_cast<uint32_t>(payload.size()));
}
```

## Step 4: Implement your server

Subclass the generated class and implement the pure virtual handlers:

```cpp
#include "DeviceMonitor.h"      // from gen/server/

class MyDeviceService : public ms::ipc::DeviceMonitor
{
public:
    using DeviceMonitor::DeviceMonitor;      // inherit constructor

protected:
    int handleGetDeviceCount(uint32_t *count) override
    {
        *count = m_deviceCount;
        return IPC_SUCCESS;
    }

    int handleGetDeviceStatus(uint32_t deviceId, uint32_t *status) override
    {
        if (deviceId >= m_deviceCount)
            return 1;                        // user-defined error (positive)
        *status = m_devices[deviceId].online ? 1 : 0;
        return IPC_SUCCESS;
    }

    // You can also call the generated notification senders:
    void onDevicePlugged(uint32_t id)
    {
        notifyDeviceConnected(id);           // broadcasts to all connected clients
    }

private:
    uint32_t m_deviceCount = 4;
    // ... your device tracking data ...
};
```

Start it the same way as any ServiceBase:

```cpp
MyDeviceService service("device_monitor");
service.start();
// ... run until shutdown ...
service.stop();
```

## Step 5: Understand the generated client code

### gen/client/DeviceMonitor.h

```cpp
// Auto-generated by ipcgen — do not edit.
#pragma once
#include "ClientBase.h"
#include <cstdint>
#include <cstring>
#include <vector>

namespace ms::ipc
{

class DeviceMonitor : public ClientBase
{
public:
    using ClientBase::ClientBase;

    static constexpr uint32_t kServiceId = 0x00fefaf3u;

    int GetDeviceCount(uint32_t *count, uint32_t timeoutMs = 2000);
    int GetDeviceStatus(uint32_t deviceId, uint32_t *status, uint32_t timeoutMs = 2000);

protected:
    virtual void onDeviceConnected(uint32_t deviceId) {}
    virtual void onDeviceDisconnected(uint32_t deviceId) {}

    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override;
};

} // namespace ms::ipc
```

What ipcgen generates for the client:

| Member | Purpose |
|--------|---------|
| `GetDeviceCount()` | **Typed RPC method** — marshals params, calls server, unmarshals response |
| `GetDeviceStatus()` | **Typed RPC method** — same |
| `onDeviceConnected()` | **Virtual callback** — override to handle notifications (default: no-op) |
| `onDeviceDisconnected()` | **Virtual callback** — override to handle notifications |
| `onNotification()` | **Generated override** — dispatches by messageId, calls your callbacks |

### gen/client/DeviceMonitor.cpp

Each RPC method serializes `[in]` params, calls the base `call()`, and
deserializes `[out]` params:

```cpp
int DeviceMonitor::GetDeviceStatus(uint32_t deviceId, uint32_t *status, uint32_t timeoutMs)
{
    std::vector<uint8_t> request(sizeof(deviceId));
    std::memcpy(request.data(), &deviceId, sizeof(deviceId));

    std::vector<uint8_t> response;
    int _rc = call(kServiceId, 2, request, &response, timeoutMs);

    if (_rc == IPC_SUCCESS && response.size() >= sizeof(*status))
    {
        if (status)
            std::memcpy(status, response.data(), sizeof(*status));
    }
    return _rc;
}
```

The notification dispatcher routes incoming notifications to your callbacks:

```cpp
void DeviceMonitor::onNotification(uint32_t serviceId, uint32_t messageId,
                                   const std::vector<uint8_t> &payload)
{
    if (serviceId != kServiceId) return;

    switch (messageId)
    {
    case 1: // DeviceConnected
    {
        uint32_t deviceId;
        std::memcpy(&deviceId, payload.data(), sizeof(deviceId));
        onDeviceConnected(deviceId);
        break;
    }
    case 2: // DeviceDisconnected
    {
        uint32_t deviceId;
        std::memcpy(&deviceId, payload.data(), sizeof(deviceId));
        onDeviceDisconnected(deviceId);
        break;
    }
    default:
        break;
    }
}
```

## Step 6: Use the generated client

For basic usage, use the generated client directly:

```cpp
#include "DeviceMonitor.h"      // from gen/client/

// Connect
ms::ipc::DeviceMonitor client("device_monitor");
client.connect();

// Typed RPC calls — no raw bytes, no message IDs
uint32_t count = 0;
int rc = client.GetDeviceCount(&count);
if (rc == IPC_SUCCESS)
    printf("Device count: %u\n", count);

uint32_t status = 0;
rc = client.GetDeviceStatus(0, &status);
if (rc == IPC_SUCCESS)
    printf("Device 0 status: %u\n", status);

// Custom timeout
rc = client.GetDeviceStatus(1, &status, 5000);  // 5 second timeout

client.disconnect();
```

To receive notifications, subclass and override the callbacks:

```cpp
class MyDeviceClient : public ms::ipc::DeviceMonitor
{
public:
    using DeviceMonitor::DeviceMonitor;

protected:
    void onDeviceConnected(uint32_t deviceId) override
    {
        printf("Device %u connected!\n", deviceId);
    }

    void onDeviceDisconnected(uint32_t deviceId) override
    {
        printf("Device %u disconnected!\n", deviceId);
    }
};

// Use it:
MyDeviceClient client("device_monitor");
client.connect();
// ... notifications arrive automatically on the receiver thread ...
```

## Step 7: Add to your CMakeLists.txt

```cmake
# Server executable
add_executable(device_server
    device_server.cpp
    gen/server/DeviceMonitor.cpp
)
target_include_directories(device_server PRIVATE gen/server/)
target_link_libraries(device_server ms-ipc)

# Client executable
add_executable(device_client
    device_client.cpp
    gen/client/DeviceMonitor.cpp
)
target_include_directories(device_client PRIVATE gen/client/)
target_link_libraries(device_client ms-ipc)
```

**Important:** The server and client generated classes have the same name
(`DeviceMonitor`) but different implementations. They must be compiled into
separate executables — do not link both into the same binary.

---

# Summary: Low-level vs Generated

| Aspect | Low-level (ServiceBase/ClientBase) | Generated (ipcgen) |
|--------|-----------------------------------|---------------------|
| Payload format | Raw `std::vector<uint8_t>` | Typed C++ parameters |
| Message IDs | Manual numeric constants | Auto-assigned in IDL |
| Serialization | You handle it | Auto-generated memcpy |
| Type safety | None (raw bytes) | Compile-time checked |
| Notifications | Manual `sendNotify()` + `onNotification()` | `notifyXxx()` + `onXxx()` virtual callbacks |
| Best for | Prototyping, dynamic formats | Production services |

---

# Tests

| File | Tests |
|------|-------|
| `CodeGenServerTest.cpp` | 6 tests — server dispatch, marshal/unmarshal, notifications, invalid method, RunLoop mode |
| `CodeGenClientTest.cpp` | 5 tests — client typed RPC, notification callbacks, RunLoop mode |

Run with:

```bash
python3 build.py -t
```
