# Serial Loopback Example

Learn the lowest-friction `ITransport` path by running Aether over a PTY-backed
serial loopback instead of shared memory and sockets.

## What You'll Learn
- how `SerialTransport` maps a byte stream onto Aether frames
- how `TransportClientBase` talks to a custom transport
- how to test a serial transport without physical hardware

## Prerequisites
- Linux or macOS
- build from the repository root
- `python3 build.py -e` or an equivalent CMake build with examples enabled

## Files That Matter
| File | Why it matters |
|------|----------------|
| `serial_loopback.cpp` | creates the PTY pair, runs the echo server thread, and issues client RPC calls |
| `../SerialTransport.h` | shared serial transport implementation used by the transport examples |
| `CMakeLists.txt` | defines the `serial_loopback` example target |

## Step 1: Read the IDL
This example does not use IDL or generated bindings.

It is intentionally lower level: the client sends raw service and message IDs,
and the transport layer just moves framed payload bytes.

## Step 2: Generate Code
No code generation step is required.

## Step 3: Review the User Code
- `serial_loopback.cpp` creates a PTY pair with `openpty()` so both sides can
  exchange binary data locally.
- `echoServer()` reads frames from the slave side and flips each request into a
  response by changing the frame flags.
- `TransportClientBase` handles sequence numbers, timeouts, and waiting for the
  response. `SerialTransport` handles handshake, framing, and CRC validation.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
./build/examples/transports/serial-loopback/serial_loopback
```

## Expected Output
You should see:

```text
=== Serial Loopback Example ===
Handshake complete
Client connected.
TX [svc=1 msg=1]: "Hello, serial world!"
RX [seq ok]:      "Hello, serial world!"
Done.
```

## What Just Happened
The program created a serial link in memory, negotiated the transport
handshake, then used Aether's transport-oriented client runtime to send three
RPC-style messages over that link. This is the smallest example that shows how
to plug a non-default transport into the runtime.

## What To Modify Next
- change the payload strings or service/message IDs and rerun the demo
- add a new frame type or server-side validation inside `echoServer()`

## Related Examples
- [`../serial-sensor/`](../serial-sensor/) for a host-to-device transport with an `aether-lite` endpoint
- [`../c-echo/`](../c-echo/) for a raw API example that uses the default host transport instead
