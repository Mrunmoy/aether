# C Echo Example: the raw C API path

This example shows the smallest host-side Aether integration: pure C code that
uses `aether_ipc.h` directly, with no IDL file and no generated code.

## What You'll Learn

- how to create a service and client with the C API
- how raw service and message IDs replace typed generated methods
- what work code generation normally saves you from doing

## Prerequisites

- Linux, macOS, or Windows
- C compiler, CMake, and Python 3 if you build from the source tree
- an Aether source checkout

All commands below run from the repository root.

## Files That Matter

| File | Why it matters |
|------|----------------|
| [`echo_server.c`](echo_server.c) | User-written raw C server callback and lifecycle |
| [`echo_client.c`](echo_client.c) | User-written raw C client calls and output |
| [`CMakeLists.txt`](CMakeLists.txt) | Builds the example against the in-tree `aether` target |

## Step 1: No IDL In This Example

Unlike [`../echo/`](../echo/), this example does not start from an IDL file.
The contract is implicit in the code:

- service name: `c_echo`
- service ID: `0`
- message ID: `1`
- payload: raw bytes echoed back unchanged

That makes the example small, but it also means the client and server must stay
manually aligned.

## Step 2: No Code Generation In This Example

There is no `ipcgen` step here. The client and server call the C API directly:

- the server registers `onRequest(...)`
- the client calls `aether_client_call(...)`

This is the contrast case that shows what generated code normally removes.

## Step 3: Review the User Code

[`echo_server.c`](echo_server.c):

- creates a service named `c_echo`
- starts the service
- handles each request in `onRequest(...)`
- echoes the incoming payload back to the caller

[`echo_client.c`](echo_client.c):

- creates a client for `c_echo`
- connects
- sends three raw string payloads
- prints each echoed response

The runtime still handles connection setup, framing, shared memory, and thread
management. The difference is that you manage the logical contract yourself.

## Build

```bash
python3 build.py -e
```

## Run

```bash
# Terminal 1
./build/examples/c-echo/c_echo_server

# Terminal 2
./build/examples/c-echo/c_echo_client
```

## Expected Output

Server output:

```text
[server] listening on 'c_echo'...
[server] request msg=1  5 bytes
[server] request msg=1  6 bytes
[server] request msg=1  3 bytes
[server] shutting down
```

Client output:

```text
[client] connected to 'c_echo'
[client] sent 'Hello' → got 'Hello'
[client] sent 'Aether' → got 'Aether'
[client] sent 'IPC' → got 'IPC'
```

## What Just Happened

This example uses the same runtime as the typed C++ path, but it leaves the
logical contract in user code instead of an IDL file. That is why the code is
short but more fragile: the service ID, message ID, and payload agreement live
only in the client and server source files.

## What To Modify Next

- Change the payload format and update both sides manually to see how quickly a
  raw API can drift.
- Then compare the same change in [`../echo/`](../echo/) to see why the IDL and
  generated path are the recommended default.

## Related Examples

- [`../echo/`](../echo/) for the recommended typed workflow
- [`../sdk-usage/`](../sdk-usage/) for the SDK consumer version of generated wrappers
