# Examples

These examples show how to use aether at two levels and demonstrate how
the code generator eliminates boilerplate while keeping things type-safe.

## Building

```bash
python3 build.py -e           # build examples
python3 build.py -t           # build + run all tests (includes examples)
```

---

# Low-level API (Echo)

The low-level API gives you direct control. You subclass `ServiceBase` and
`ClientBase`, work with raw `std::vector<uint8_t>` payloads, and manage
message IDs yourself.

**Server** (`echo_server.cpp`):

```cpp
class EchoService : public aether::ipc::ServiceBase
{
protected:
    int onRequest(uint32_t messageId, const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override
    {
        *response = request;    // echo back
        return IPC_SUCCESS;
    }
};

EchoService service("echo");
service.start();
```

**Client** (`echo_client.cpp`):

```cpp
aether::ipc::ClientBase client("echo");
client.connect();

std::vector<uint8_t> request(msg, msg + strlen(msg));
std::vector<uint8_t> response;
int rc = client.call(1, 1, request, &response);    // serviceId=1, messageId=1

client.disconnect();
```

This works, but notice the pain points: you manually pack bytes into vectors,
track numeric message IDs, and both sides must agree on the exact byte layout.
For anything beyond a simple echo, this gets tedious and error-prone fast.

### RunLoop mode

Both `ServiceBase` and `ClientBase` accept an optional `ms::RunLoop*`.
When provided, they register fds on the RunLoop instead of spawning
internal threads — useful for single-threaded event-driven architectures:

```cpp
ms::RunLoop loop;
loop.init("app");

EchoService service("echo", &loop);
service.start();                    // no threads — fd registered on RunLoop

std::thread loopThread([&loop] { loop.run(); });
// ... clients can connect and call from other threads ...

service.stop();
loop.stop();
loopThread.join();
```

---

# Code Generation with ipcgen

The code generator eliminates all the boilerplate above. You describe your
service once in an IDL file, and `ipcgen` generates typed C++ classes where:

- **Server**: you implement pure virtual handlers — no byte packing
- **Client**: you call typed methods — no message IDs, no raw vectors
- **Notifications**: the server calls `notifyXxx()`, the client overrides `onXxx()`
- **Serialization**: fully automatic, compile-time checked

## Workflow

### 1. Write the IDL

```idl
service DeviceMonitor
{
    [method=1]
    int GetDeviceCount([out] uint32 count);

    [method=2]
    int GetDeviceInfo([in] uint32 deviceId, [out] DeviceInfo info);
};

notifications DeviceMonitor
{
    [notify=1]
    void DeviceConnected([in] DeviceInfo info);

    [notify=2]
    void DeviceDisconnected([in] uint32 deviceId);
};
```

### 2. Generate code

```bash
cd tools && python3 -m ipcgen ../example/DeviceMonitor.idl --outdir ../example/gen
```

This produces five files:

| File | What it gives you |
|------|-------------------|
| `DeviceMonitorTypes.h` | Shared enums and structs |
| `server/DeviceMonitor.h` | Server class with pure virtual handlers |
| `server/DeviceMonitor.cpp` | Request dispatch and notification senders |
| `client/DeviceMonitor.h` | Client class with typed RPC methods |
| `client/DeviceMonitor.cpp` | Request marshaling and notification dispatch |

### 3. Implement the server

Subclass the generated class. All you write is your business logic — the
generated code handles serialization, dispatch, and error routing:

```cpp
#include "DeviceMonitor.h"      // from gen/server/

class MyDeviceService : public aether::ipc::DeviceMonitor
{
public:
    using DeviceMonitor::DeviceMonitor;

protected:
    int handleGetDeviceCount(uint32_t *count) override
    {
        *count = m_deviceCount;
        return IPC_SUCCESS;
    }

    int handleGetDeviceInfo(uint32_t deviceId, DeviceInfo *info) override
    {
        if (deviceId >= m_deviceCount)
            return 1;                    // user-defined error
        *info = m_devices[deviceId];
        return IPC_SUCCESS;
    }

    void onDevicePlugged(uint32_t id)
    {
        notifyDeviceConnected(m_devices[id]);   // broadcasts to all clients
    }
};

MyDeviceService service("device_monitor");
service.start();
```

### 4. Use the client

Call typed methods — no message IDs, no byte vectors, no memcpy:

```cpp
#include "DeviceMonitor.h"      // from gen/client/

aether::ipc::DeviceMonitor client("device_monitor");
client.connect();

uint32_t count = 0;
client.GetDeviceCount(&count);

DeviceInfo info{};
client.GetDeviceInfo(0, &info);

client.disconnect();
```

To receive notifications, subclass and override callbacks:

```cpp
class MyDeviceClient : public aether::ipc::DeviceMonitor
{
protected:
    void onDeviceConnected(DeviceInfo info) override
    {
        printf("Device %u connected: %s\n", info.id, info.name);
    }

    void onDeviceDisconnected(uint32_t deviceId) override
    {
        printf("Device %u disconnected\n", deviceId);
    }
};
```

### 5. Add to CMakeLists.txt

```cmake
# Server
add_executable(device_server device_server.cpp gen/server/DeviceMonitor.cpp)
target_include_directories(device_server PRIVATE gen/server/)
target_link_libraries(device_server aether)

# Client
add_executable(device_client device_client.cpp gen/client/DeviceMonitor.cpp)
target_include_directories(device_client PRIVATE gen/client/)
target_link_libraries(device_client aether)
```

**Note:** Server and client generated classes share the same name but have
different implementations. Compile them into separate executables.

## What you avoid with codegen

Compare the low-level echo approach to the generated approach:

| Without codegen | With codegen |
|---|---|
| Pack params into `std::vector<uint8_t>` with memcpy | Call `client.GetDeviceInfo(42, &info)` |
| Track numeric message IDs manually | Method names in generated enums |
| Parse raw bytes in `onRequest()` switch | Implement `handleGetDeviceInfo(deviceId, info)` |
| Manual `sendNotify()` + `onNotification()` byte parsing | Call `notifyXxx()`, override `onXxx()` callbacks |
| Both sides must agree on byte layout | Single IDL source of truth |

---

# IDL syntax reference

**Service block** — RPC methods:

```idl
service <Name>
{
    [method=<id>]
    int <MethodName>([in] <type> <param>, ..., [out] <type> <param>, ...);
};
```

- Methods return `int` (error code: `IPC_SUCCESS` on success)
- `[in]` params are sent client-to-server
- `[out]` params are returned server-to-client
- Each method has a unique numeric ID

**Notifications block** — server-to-client broadcasts:

```idl
notifications <Name>
{
    [notify=<id>]
    void <NotificationName>([in] <type> <param>, ...);
};
```

- Notifications return `void` (fire-and-forget, broadcast to all clients)
- Only `[in]` params (data flows server to client)

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
| `T[N]` | `std::array<T, N>` | N x sizeof(T) |
| `string[N]` | `char[N+1]` | N+1 bytes |

**Arrays** (`T[N]`) are fixed-length and work with any scalar, enum, or struct.
**Strings** (`string[N]`) are null-terminated, fixed-capacity char buffers.
`N` is max characters; wire size is `N+1` bytes. `string` without `[N]`
is a syntax error.

You can also define **enums** and **structs**:

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

---

# Feature Examples

Self-contained examples showing each IDL feature. Each one is a
copy-paste starting point for a real embedded service.

---

## Scalars & Enums — Temperature Sensor

A minimal service using only scalar types and enums.

### IDL

```idl
enum SensorStatus
{
    Offline = 0,
    Online = 1,
    Error = 2,
};

service TemperatureSensor
{
    [method=1]
    int GetTemperature([out] float32 celsius);

    [method=2]
    int GetStatus([out] SensorStatus status);

    [method=3]
    int SetThreshold([in] float32 high, [in] float32 low);
};

notifications TemperatureSensor
{
    [notify=1]
    void OverTemperature([in] float32 celsius);

    [notify=2]
    void StatusChanged([in] SensorStatus status);
};
```

### Server

```cpp
class MyTempSensor : public aether::ipc::TemperatureSensor
{
protected:
    int handleGetTemperature(float *celsius) override
    {
        *celsius = readHardwareSensor();
        return IPC_SUCCESS;
    }

    int handleGetStatus(SensorStatus *status) override
    {
        *status = m_status;
        return IPC_SUCCESS;
    }

    int handleSetThreshold(float high, float low) override
    {
        m_highThreshold = high;
        m_lowThreshold = low;
        return IPC_SUCCESS;
    }

    // Call when hardware detects over-temperature:
    void onHwOverTemp(float temp)
    {
        notifyOverTemperature(temp);    // broadcasts to all clients
    }
};
```

### Client

```cpp
aether::ipc::TemperatureSensor client("temp_sensor");
client.connect();

float temp = 0;
if (client.GetTemperature(&temp) == IPC_SUCCESS)
    printf("Temperature: %.1f°C\n", temp);

SensorStatus status;
client.GetStatus(&status);

client.SetThreshold(85.0f, -10.0f);
client.disconnect();
```

---

## Fixed-length Arrays — Multi-channel ADC

Service reading fixed-length sample buffers from an ADC. Uses arrays in
both struct fields and method parameters.

### IDL

```idl
struct AdcSample
{
    uint32 timestamp;
    uint16[8] channels;
};

service AdcReader
{
    [method=1]
    int GetLatestSample([out] AdcSample sample);

    [method=2]
    int GetChannelHistory([in] uint32 channelId, [out] uint16[64] buffer);
};

notifications AdcReader
{
    [notify=1]
    void SampleReady([in] AdcSample sample);
};
```

### Generated struct

```cpp
struct AdcSample
{
    uint32_t timestamp;
    std::array<uint16_t, 8> channels;   // T[N] -> std::array<T, N>
};
```

### Server

```cpp
class MyAdcReader : public aether::ipc::AdcReader
{
protected:
    int handleGetLatestSample(AdcSample *sample) override
    {
        sample->timestamp = now();
        for (int i = 0; i < 8; i++)
            sample->channels[i] = readAdc(i);
        return IPC_SUCCESS;
    }

    int handleGetChannelHistory(uint32_t channelId, uint16_t *buffer) override
    {
        // buffer points to a uint16_t[64] on the wire
        for (int i = 0; i < 64; i++)
            buffer[i] = m_history[channelId][i];
        return IPC_SUCCESS;
    }
};
```

### Client

```cpp
aether::ipc::AdcReader client("adc");
client.connect();

AdcSample sample{};
if (client.GetLatestSample(&sample) == IPC_SUCCESS)
{
    printf("Channel 0: %u\n", sample.channels[0]);
    printf("Channel 7: %u\n", sample.channels[7]);
}

uint16_t history[64];
client.GetChannelHistory(0, history);
client.disconnect();
```

---

## Bounded Strings — Alarm System

Alarm service using `string[N]` for human-readable names and messages.
Strings are fixed-size on the wire (N+1 bytes, null-terminated).

### IDL

```idl
struct AlarmEvent
{
    uint32 id;
    uint32 severity;
    string[32] source;
    string[128] message;
};

service AlarmSystem
{
    [method=1]
    int GetAlarmCount([out] uint32 count);

    [method=2]
    int GetAlarm([in] uint32 alarmId, [out] AlarmEvent event);

    [method=3]
    int AcknowledgeAlarm([in] uint32 alarmId, [in] string[32] operatorName);
};

notifications AlarmSystem
{
    [notify=1]
    void AlarmTriggered([in] AlarmEvent event);

    [notify=2]
    void AlarmCleared([in] uint32 alarmId);
};
```

### Generated struct

```cpp
struct AlarmEvent
{
    uint32_t id;
    uint32_t severity;
    char source[33];      // string[32] -> char[33] (32 chars + null)
    char message[129];    // string[128] -> char[129]
};
```

### Server

```cpp
class MyAlarmSystem : public aether::ipc::AlarmSystem
{
protected:
    int handleGetAlarmCount(uint32_t *count) override
    {
        *count = m_alarms.size();
        return IPC_SUCCESS;
    }

    int handleGetAlarm(uint32_t alarmId, AlarmEvent *event) override
    {
        if (alarmId >= m_alarms.size())
            return 1;
        *event = m_alarms[alarmId];
        return IPC_SUCCESS;
    }

    // operatorName is const char* (read-only [in] param)
    int handleAcknowledgeAlarm(uint32_t alarmId,
                               const char *operatorName) override
    {
        printf("Alarm %u acknowledged by %s\n", alarmId, operatorName);
        notifyAlarmCleared(alarmId);
        return IPC_SUCCESS;
    }

    // Raise an alarm from hardware event:
    void raiseAlarm(const char *source, const char *msg)
    {
        AlarmEvent event{};
        event.id = m_nextId++;
        event.severity = 3;
        std::strncpy(event.source, source, 32);
        std::strncpy(event.message, msg, 128);
        m_alarms.push_back(event);
        notifyAlarmTriggered(event);
    }
};
```

### Client

```cpp
aether::ipc::AlarmSystem client("alarms");
client.connect();

uint32_t count = 0;
client.GetAlarmCount(&count);

AlarmEvent event{};
for (uint32_t i = 0; i < count; i++)
{
    client.GetAlarm(i, &event);
    printf("[%s] %s (severity=%u)\n", event.source, event.message, event.severity);
}

client.AcknowledgeAlarm(0, "operator_jdoe");
client.disconnect();
```

---

## Convenience Wrapper — Sensor Inventory

When a service has `GetCount` + `GetItem` methods, you can write a manual
wrapper that chains them into a single `std::vector`-returning call.
This is **not generated** — it's a pattern you write on top of the
generated client.

### IDL

```idl
struct SensorInfo
{
    uint32 id;
    uint32 type;
    string[64] name;
    float32 lastReading;
};

service SensorInventory
{
    [method=1]
    int GetSensorCount([out] uint32 count);

    [method=2]
    int GetSensorInfo([in] uint32 sensorId, [out] SensorInfo info);
};
```

### Manual convenience wrapper

```cpp
#include "SensorInventory.h"   // generated client

class SensorClient : public aether::ipc::SensorInventory
{
public:
    using SensorInventory::SensorInventory;

    // Convenience: chain count + list into a single call.
    int GetAllSensors(std::vector<SensorInfo> &sensors)
    {
        uint32_t count = 0;
        int rc = GetSensorCount(&count);
        if (rc != IPC_SUCCESS)
            return rc;

        sensors.resize(count);
        for (uint32_t i = 0; i < count; i++)
        {
            rc = GetSensorInfo(i, &sensors[i]);
            if (rc != IPC_SUCCESS)
                return rc;
        }
        return IPC_SUCCESS;
    }
};
```

### Usage

```cpp
SensorClient client("sensors");
client.connect();

std::vector<SensorInfo> sensors;
if (client.GetAllSensors(sensors) == IPC_SUCCESS)
{
    for (auto &s : sensors)
        printf("Sensor %u: %s (reading=%.2f)\n", s.id, s.name, s.lastReading);
}

client.disconnect();
```

This pattern works for any count+item pair. The wrapper class inherits
all generated methods and adds the convenience method on top.

---

# Tests

| File | Tests |
|------|-------|
| `CodeGenServerTest.cpp` | 6 tests — server dispatch, notifications, invalid method, RunLoop mode |
| `CodeGenClientTest.cpp` | 5 tests — client typed RPC, notification callbacks, RunLoop mode |

```bash
python3 build.py -t
```
