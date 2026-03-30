# Echo Example: the default source-build path

If you cloned the Aether repo and want the fastest way to understand the
framework, start here.

This is the recommended newcomer stack:

- C++
- default `ipcgen` backend
- source build against the in-tree `aether` target
- typed generated classes on top of `ServiceBase` and `ClientBase`

## Run it

From the repo root:

```bash
python3 build.py -e
./build/examples/echo/device_monitor_server
./build/examples/echo/device_monitor_client
```

Expected client output looks like:

```text
[client] device count: 2
[client] device 0: USB Audio Interface (vendor=0x1234 product=0x0001)
[client] device 1: BLE Sensor Tag (vendor=0x4321 product=0x0002)
[client] waiting briefly for notifications...
[client] notification: connected -> USB Audio Interface
[client] notification: disconnected -> id=1
```

## What this example teaches

The path is intentionally small:

1. [`DeviceMonitor.idl`](DeviceMonitor.idl) defines the RPC contract.
2. `ipcgen` turns that IDL into typed C++ glue in [`gen/`](gen/).
3. [`device_monitor_server.cpp`](device_monitor_server.cpp) subclasses the generated server and implements business logic.
4. [`device_monitor_client.cpp`](device_monitor_client.cpp) subclasses the generated client and overrides notification callbacks.
5. The Aether runtime handles connection setup, shared-memory transport, framing, dispatch, and notification delivery.

## What you write vs. what Aether writes

| You write | Aether generates or provides |
|-----------|------------------------------|
| [`DeviceMonitor.idl`](DeviceMonitor.idl) | [`gen/DeviceMonitorTypes.h`](gen/DeviceMonitorTypes.h) |
| [`device_monitor_server.cpp`](device_monitor_server.cpp) | [`gen/server/DeviceMonitor.h`](gen/server/DeviceMonitor.h) |
| [`device_monitor_client.cpp`](device_monitor_client.cpp) | [`gen/server/DeviceMonitor.cpp`](gen/server/DeviceMonitor.cpp) |
| `CMakeLists.txt` target wiring | [`gen/client/DeviceMonitor.h`](gen/client/DeviceMonitor.h) |
| | [`gen/client/DeviceMonitor.cpp`](gen/client/DeviceMonitor.cpp) |
| | core runtime in `ServiceBase`, `ClientBase`, `Connection`, and `FrameIO` |

## The learning narrative

### Step 1: read the IDL

[`DeviceMonitor.idl`](DeviceMonitor.idl) is the source of truth. It declares:

- two methods: `GetDeviceCount` and `GetDeviceInfo`
- two notifications: `DeviceConnected` and `DeviceDisconnected`
- shared types: `DeviceType` and `DeviceInfo`

### Step 2: inspect what `ipcgen` produced

The generated files in [`gen/`](gen/) show the split clearly:

- `gen/server/` is the typed server skeleton
- `gen/client/` is the typed client stub
- `gen/DeviceMonitorTypes.h` is shared by both sides

To regenerate them from the repo root:

```bash
python3 -m tools.ipcgen examples/echo/DeviceMonitor.idl --outdir examples/echo/gen
```

### Step 3: inspect the user code

[`device_monitor_server.cpp`](device_monitor_server.cpp) only implements domain logic:

- return a device count
- return a device record
- trigger demo notifications

[`device_monitor_client.cpp`](device_monitor_client.cpp) only:

- connects
- calls typed methods
- overrides notification callbacks

### Step 4: connect that to the runtime

Under the hood:

- `ServiceBase` accepts clients and dispatches typed handlers
- `ClientBase` manages the connection and waits for typed responses
- `Connection` and `FrameIO` move framed messages through shared memory

That is the mental model for the default Aether path:

`IDL -> generated types -> your handlers/callbacks -> runtime transport`

## What codegen removes

The generated classes remove the repetitive wire work:

- the server side gives you pure virtual handlers such as `handleGetDeviceInfo(...)`
- the client side gives you typed RPC methods such as `GetDeviceInfo(...)`
- notifications become typed calls and callbacks instead of raw message IDs

You never pack bytes by hand in this path. The generated code and runtime do that for you.

## Low-level contrast

The same directory also includes [`echo_server.cpp`](echo_server.cpp) and
[`echo_client.cpp`](echo_client.cpp). Those use the low-level API directly:

- raw `std::vector<uint8_t>` payloads
- explicit numeric `serviceId` and `messageId`
- manual payload layout agreement

They are useful to understand the runtime, but they are not the recommended
starting point for new users.

## Why this is the recommended default

This path has the smallest cognitive load for a first-time contributor:

- one checked-in IDL
- one generated type set
- one typed server
- one typed client
- no SDK packaging setup
- no custom transport setup
- no manual serialization
