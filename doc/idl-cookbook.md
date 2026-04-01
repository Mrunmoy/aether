# IDL Cookbook

This document preserves the broader example-oriented material that used to live
in `examples/getting-started/echo/README.md`.

Use [examples/getting-started/echo/README.md](../examples/getting-started/echo/README.md) for the canonical
source-build path. Use this cookbook when you want additional IDL patterns,
comparison material, and implementation sketches after the first successful run.

## Low-level vs generated API

Generated code is the recommended default because it removes wire-format
boilerplate.

| Without codegen | With codegen |
|---|---|
| Pack params into `std::vector<uint8_t>` with `memcpy` | Call typed methods such as `GetDeviceInfo(...)` |
| Track numeric IDs manually | Use generated method and notification enums |
| Parse raw bytes in `onRequest()` | Implement typed handlers such as `handleGetDeviceInfo(...)` |
| Manually encode notifications | Call `notifyXxx()` and override `onXxx()` |
| Keep client/server layouts in sync by hand | Keep one IDL as the source of truth |

## RunLoop mode

`ServiceBase` and `ClientBase` accept an optional `ms::RunLoop *`.

- On Linux and macOS, they can register on the RunLoop instead of starting internal threads.
- On Windows, the current backend rejects RunLoop mode because named pipes are not waitable RunLoop sources.

Typical shape:

```cpp
ms::RunLoop loop;
loop.init("app");

EchoService service("echo", &loop);
service.start();

std::thread loopThread([&loop] { loop.run(); });
// ... clients connect and call from other threads ...

service.stop();
loop.stop();
loopThread.join();
```

## IDL syntax reference

### Services

```idl
service <Name>
{
    [method=<id>]
    int <MethodName>([in] <type> <param>, ..., [out] <type> <param>, ...);
};
```

- Methods return `int`
- `[in]` params flow client to server
- `[out]` params flow server to client
- each method ID must be unique

### Notifications

```idl
notifications <Name>
{
    [notify=<id>]
    void <NotificationName>([in] <type> <param>, ...);
};
```

- notifications are fire-and-forget broadcasts
- only `[in]` params are allowed
- each notification ID must be unique

### Type map

| IDL type | C++ type | Size |
|----------|----------|------|
| `uint8` .. `uint64` | `uint8_t` .. `uint64_t` | 1-8 bytes |
| `int8` .. `int64` | `int8_t` .. `int64_t` | 1-8 bytes |
| `float32` / `float64` | `float` / `double` | 4 / 8 bytes |
| `bool` | `bool` | 1 byte |
| `T[N]` | `std::array<T, N>` | `N * sizeof(T)` |
| `string[N]` | `char[N+1]` | `N+1` bytes |

Arrays are fixed-length. Strings are fixed-capacity, null-terminated buffers.

### Enums and structs

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

## Pattern: Scalars and enums

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
};
```

Server sketch:

```cpp
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
```

## Pattern: Fixed-length arrays

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
```

Generated C++ maps `uint16[8]` to `std::array<uint16_t, 8>`, while `[out] uint16[64]`
becomes a pointer-style output buffer in the generated handler signature.

## Pattern: Bounded strings

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
    int GetAlarm([in] uint32 alarmId, [out] AlarmEvent event);

    [method=2]
    int AcknowledgeAlarm([in] uint32 alarmId, [in] string[32] operatorName);
};
```

Generated C++ maps:

- `string[32]` -> `char[33]`
- `[in] string[32]` -> `const char *`
- `[out] string[32]` -> `char *`

## Pattern: Convenience wrappers

When a service exposes `GetCount` plus `GetItem`, it is often worth writing a
manual convenience wrapper on top of the generated client.

```cpp
class SensorClient : public aether::ipc::SensorInventory
{
public:
    using SensorInventory::SensorInventory;

    int GetAllSensors(std::vector<SensorInfo> &sensors)
    {
        uint32_t count = 0;
        int rc = GetSensorCount(&count);
        if (rc != IPC_SUCCESS)
            return rc;

        sensors.resize(count);
        for (uint32_t i = 0; i < count; ++i)
        {
            rc = GetSensorInfo(i, &sensors[i]);
            if (rc != IPC_SUCCESS)
                return rc;
        }
        return IPC_SUCCESS;
    }
};
```

## When to use this cookbook

Read this after the canonical echo example if you want to:

- model richer IDLs with arrays, strings, structs, and enums
- compare low-level and generated integration styles
- understand RunLoop integration
- add convenience layers on top of generated clients
