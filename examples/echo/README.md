# Echo Example: the default source-build path

This is the recommended first example if you cloned the Aether source tree and
want to understand the normal C++ workflow.

## What You'll Learn

- how an IDL file defines an RPC contract
- how `ipcgen` generates typed C++ client and server glue
- what code you write versus what Aether generates
- how a typed client and server talk through the shared-memory runtime

## Prerequisites

- Linux, macOS, or Windows
- C++17 compiler, CMake, and Python 3
- an Aether source checkout

All commands below run from the repository root.

## Files That Matter

| File | Why it matters |
|------|----------------|
| [`DeviceMonitor.idl`](DeviceMonitor.idl) | Defines the RPC contract and shared types |
| [`gen/DeviceMonitorTypes.h`](gen/DeviceMonitorTypes.h) | Shared generated enums and structs |
| [`gen/server/DeviceMonitor.h`](gen/server/DeviceMonitor.h) | Generated typed server skeleton |
| [`gen/client/DeviceMonitor.h`](gen/client/DeviceMonitor.h) | Generated typed client stub |
| [`device_monitor_server.cpp`](device_monitor_server.cpp) | User-written server implementation |
| [`device_monitor_client.cpp`](device_monitor_client.cpp) | User-written client implementation |
| [`echo_server.cpp`](echo_server.cpp) | Low-level contrast example without code generation |
| [`echo_client.cpp`](echo_client.cpp) | Low-level contrast example without code generation |

## Step 1: Read the IDL

[`DeviceMonitor.idl`](DeviceMonitor.idl) is the source of truth for the typed
example. It declares:

- two methods: `GetDeviceCount` and `GetDeviceInfo`
- two notifications: `DeviceConnected` and `DeviceDisconnected`
- shared types: `DeviceType` and `DeviceInfo`

That single IDL definition drives both the client and the server side.

## Step 2: Generate Code

Regenerate the typed glue with:

```bash
python3 tools/ipcgen/__main__.py examples/echo/DeviceMonitor.idl --outdir examples/echo/gen
```

This creates three kinds of files:

- `gen/DeviceMonitorTypes.h`: shared structs and enums
- `gen/server/*`: the typed server skeleton and dispatch logic
- `gen/client/*`: the typed client stub and notification dispatch

## Step 3: Review the User Code

[`device_monitor_server.cpp`](device_monitor_server.cpp) only implements domain
logic:

- return a device count
- return a specific device record
- emit demo notifications

[`device_monitor_client.cpp`](device_monitor_client.cpp) only:

- connects to the service
- calls typed methods
- overrides typed notification callbacks

Aether handles:

- connection setup
- shared-memory transport
- request and response framing
- request dispatch
- notification delivery

## Build

```bash
python3 build.py -e
```

## Run

```bash
# Terminal 1
./build/examples/echo/device_monitor_server

# Terminal 2
./build/examples/echo/device_monitor_client
```

## Expected Output

Client output:

```text
[client] device count: 2
[client] device 0: USB Audio Interface (vendor=0x1234 product=0x0001)
[client] device 1: BLE Sensor Tag (vendor=0x4321 product=0x0002)
[client] waiting briefly for notifications...
[client] disconnected.
```

While the client waits, you should also see one or more notification lines:

```text
[client] notification: connected -> USB Audio Interface
[client] notification: disconnected -> id=1
```

The notification order depends on when the client joins the server's demo cycle.

## What Just Happened

You wrote one IDL file, Aether generated typed C++ wrappers from it, and your
client/server code only filled in the business logic. The server implementation
never packs bytes manually, and the client never uses numeric message IDs. The
runtime underneath `ServiceBase`, `ClientBase`, `Connection`, and `FrameIO`
moves the framed messages through shared memory and dispatches them to the
typed handlers.

## What To Modify Next

- Add a new method to [`DeviceMonitor.idl`](DeviceMonitor.idl), regenerate, and
  implement the new handler in [`device_monitor_server.cpp`](device_monitor_server.cpp).
- Change the client to call that new method and print the result.

## Related Examples

- [`../c-echo/`](../c-echo/) to see the same runtime used without code generation
- [`../sdk-usage/`](../sdk-usage/) to see the packaged SDK workflow
- [`../../doc/idl-cookbook.md`](../../doc/idl-cookbook.md) for additional IDL patterns
