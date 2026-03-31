# Aether in 5 Minutes

This guide explains the mental model behind Aether: what the code generator
produces, what the runtime handles, and what you write. Read this before
diving into the API reference or design docs.

## What Aether Does

Aether is an IPC framework. Two processes on the same machine communicate
through shared memory ring buffers. You describe your service interface in
an IDL file, and the code generator produces typed C++ classes for both
sides. The server implements virtual handler methods. The client calls
typed RPC methods. Serialization, dispatch, and sequence correlation happen
automatically.

The same wire protocol also works over serial and USB for desktop-to-device
communication with microcontrollers.

## The Three Pieces

### 1. What ipcgen generates

From a single `.idl` file, the code generator (`ipcgen`) produces five C++
files:

| File | Purpose |
|------|---------|
| `FooTypes.h` | Shared enums and structs used by both sides |
| `server/Foo.h` | Server class with pure virtual `handleXxx()` methods |
| `server/Foo.cpp` | Request dispatch, payload validation, notification senders |
| `client/Foo.h` | Client class with typed `Xxx()` RPC methods |
| `client/Foo.cpp` | Request marshaling, response unpacking, notification dispatch |

The generated server class inherits from `ServiceBase`. The generated client
class inherits from `ClientBase`. You never touch these base classes directly
when using codegen.

### 2. What the runtime handles

The Aether runtime (`libaether.a`) manages everything below the generated
code:

- **Connection setup** — UDS/named-pipe handshake, shared memory allocation,
  ring buffer placement
- **Frame I/O** — packing and unpacking the 24-byte wire header and payload
- **Threading** — accept loop, per-client receiver threads (or RunLoop
  dispatch)
- **RPC correlation** — matching responses to pending calls via sequence
  numbers
- **Notification broadcast** — fanning out server notifications to all
  connected clients
- **Platform abstraction** — Linux, macOS, and Windows under the same API

### 3. What you write

**Server side:** subclass the generated service, implement the virtual
handlers, call `start()`.

```cpp
class MyService : public aether::ipc::Foo
{
protected:
    int handleGetData(uint32_t id, DataInfo *info) override
    {
        *info = m_data[id];
        return IPC_SUCCESS;
    }
};

MyService svc("my_service");
svc.start();
```

**Client side:** create the generated client, connect, call methods.

```cpp
aether::ipc::Foo client("my_service");
client.connect();

DataInfo info{};
int rc = client.GetData(42, &info);

client.disconnect();
```

That is the complete pattern. Everything else — shared memory, ring buffers,
frame headers, sequence numbers — is handled by the runtime.

## Threading Model

Understanding the threading model prevents the most common mistakes.

### Server threads

In the default mode, `ServiceBase::start()` creates:

- **One accept thread** that listens for new client connections. When a
  client connects, the handshake runs on this thread, then a new receiver
  thread is spawned for that client.
- **One receiver thread per client** that blocks on `recvSignal()`, reads
  frames from the client's ring buffer, and calls `onRequest()` which
  dispatches to your `handleXxx()` virtual methods.

Your handlers run on the client's receiver thread. If you access shared
state from multiple handlers, you need your own synchronization.

### Client threads

`ClientBase::connect()` creates one receiver thread that reads responses
and notifications from the server's ring buffer.

`call()` is blocking. The calling thread writes the request to the ring
buffer, sends a wakeup signal, then sleeps on a condition variable until
the receiver thread delivers the matching response (matched by sequence
number) or the timeout expires.

`call()` is thread-safe — multiple threads can call it concurrently. Each
gets its own sequence number and condition variable.

### RunLoop mode

Both `ServiceBase` and `ClientBase` accept an optional `RunLoop*`. When
provided, all dispatch happens on the RunLoop thread instead of spawning
per-client threads. This is the preferred mode for applications that already
run an event loop.

In RunLoop mode, `call()` still blocks the calling thread while waiting for
a response. If you call it from the RunLoop thread itself, it deadlocks —
the response can only be delivered by the RunLoop, which is blocked. The
workaround is to post the call to a worker thread and have it deliver the
result back via `loop.executeOnRunLoop()`.

### The sendMutex rule

Each client connection has one `txRing` (server-to-client). This ring is
SPSC — single producer, single consumer. But the server writes to it from
two paths: response frames from the receiver thread and notification frames
from `sendNotify()`. The `sendMutex` serializes these writes so the SPSC
contract is maintained. This is handled internally.

## Ownership and Lifetime

- **Shared memory** is created by the client during `connect()` and shared
  with the server. On Linux and macOS, the file descriptor is passed via
  `SCM_RIGHTS`. On Windows, the client creates a named file mapping and
  sends the mapping name in the handshake; the server opens it with
  `OpenFileMappingA()`. On POSIX, the memory is reclaimed when both file
  descriptors are closed. On Windows, the kernel reference-counts the
  mapping object and releases it when both handles are closed.

- **Ring buffers** are placement-new'd into the shared memory region. They
  do not allocate heap memory. Their lifetime is tied to the shared memory
  mapping.

- **`ServiceBase`** owns its client connections. When `stop()` is called,
  it joins all threads, closes all connections, and releases all resources.
  Calling `stop()` is safe from any thread.

- **`ClientBase`** owns its connection and receiver thread. `disconnect()`
  joins the receiver thread and cleans up. Destroying the object calls
  `disconnect()` automatically.

## Error Handling

All RPC methods return `int`. The convention:

| Value | Meaning |
|-------|---------|
| `0` (`IPC_SUCCESS`) | Success |
| Negative | Framework error (see table below) |
| Positive | Application-defined error returned by your handler |

### Framework error codes

| Code | Name | When it happens |
|------|------|-----------------|
| -1 | `IPC_ERR_DISCONNECTED` | Peer closed the connection |
| -2 | `IPC_ERR_TIMEOUT` | `call()` timed out waiting for response |
| -3 | `IPC_ERR_INVALID_SERVICE` | Service ID in frame doesn't match |
| -4 | `IPC_ERR_INVALID_METHOD` | Unknown method ID in request |
| -5 | `IPC_ERR_VERSION_MISMATCH` | Protocol version mismatch |
| -6 | `IPC_ERR_RING_FULL` | Ring buffer has no space for the frame |
| -7 | `IPC_ERR_STOPPED` | Service or client is shutting down |
| -8 | `IPC_ERR_INVALID_ARGUMENT` | Bad parameter passed to a platform or API function |
| -9 | `IPC_ERR_TRANSPORT` | Serial or custom transport-level failure |
| -10 | `IPC_ERR_CRC` | CRC mismatch on a received frame (transport mode) |
| -11 | `IPC_ERR_NOT_SUPPORTED` | Feature disabled at compile time |
| -12 | `IPC_ERR_NO_SPACE` | Registration tables full |
| -13 | `IPC_ERR_OVERFLOW` | Payload exceeds buffer capacity |

Always check the return value. A disconnected peer is the most common
runtime error and can happen at any time.

## Common Failures

These are the situations new users hit most often.

**"Connection refused" or `connect()` returns false.**
The server is not running, or the service name doesn't match. Service names
are case-sensitive. On Linux, the abstract socket namespace uses
`\0aether_<name>`. On macOS, a pathname socket is created under `$TMPDIR`
(or `/tmp/aether_<uid>/` as fallback) with the filename derived from an
FNV-1a hash of the service name. On Windows, a named pipe at
`\\.\pipe\aether_<hash>` is used (the pipe name is derived from an FNV-1a
hash of the service name, not the name itself). Make sure the server calls `start()`
before the client calls `connect()`.

**`IPC_ERR_TIMEOUT` on every call.**
The server handler is blocking or deadlocked. Check that your `handleXxx()`
implementation returns promptly. In RunLoop mode, a slow handler blocks all
other clients on the same loop.

**Client disconnected during notification broadcast.**
If a client's ring is full when `sendNotify()` tries to write, the runtime
treats that client as unresponsive: it marks the client dead, shuts down
its socket, and continues to the next client. The return value is
`IPC_ERR_DISCONNECTED` (not `IPC_ERR_RING_FULL`) because the client is
forcibly disconnected. Other connected clients still receive the
notification normally.

**Deadlock in RunLoop mode.**
You called `client.call()` from the RunLoop thread. The response can only
be delivered by the RunLoop, but the RunLoop is blocked waiting for the
response. Move `call()` to a worker thread, or use the threaded mode
instead.

**Server crashes with SIGPIPE.**
On POSIX systems, writing to a socket after the peer disconnects raises
SIGPIPE. Aether suppresses this on Linux with `MSG_NOSIGNAL` on every
send, and on macOS with `SO_NOSIGPIPE` set on each socket at creation
time. If you still see SIGPIPE, check that you are not writing to a
raw socket outside of Aether's API.

**Generated code won't compile after IDL change.**
Regenerate all five files. Partial regeneration (e.g., regenerating the
server but not the client) causes mismatched types or method signatures.
Always regenerate the full set with one `ipcgen` invocation.

## Anti-Patterns

**Calling `call()` from a handler.**
Your `handleXxx()` runs on the receiver thread. If you create a client
inside the handler and call another service synchronously, you block the
receiver thread. Other clients waiting for responses from this server will
time out. Keep handlers fast.

**Sharing a `ClientBase` across threads without understanding the model.**
`call()` is thread-safe — multiple threads can call it concurrently.
`connect()` and `disconnect()` must not be called concurrently with each
other or with `connect()` from another thread. Calling `disconnect()` while
another thread is blocked in `call()` is safe: the pending call wakes up
with `IPC_ERR_DISCONNECTED`. The typical pattern is: call `connect()` once
from one thread, then `call()` from as many threads as needed.

**Using raw `ServiceBase`/`ClientBase` instead of generated code.**
The raw API requires manual byte packing, numeric message IDs, and
careful payload size management. Use the generated code unless you have
a specific reason not to (e.g., the C API for SDK distribution).

**Forgetting to check error codes.**
Disconnection can happen between any two calls. If you ignore the return
value of `call()`, you will process stale data or crash on an empty
response buffer.

**Placing large data in the IDL.**
Each ring buffer is 256 KB. A single frame must fit in the ring. If your
struct is tens of kilobytes, you will run into `IPC_ERR_RING_FULL` under
load. Keep wire-format structs small and use multiple calls for bulk data
transfer.

## When to Use Each Backend

| Backend | Use when... |
|---------|-------------|
| **C++ (default)** | You are writing both server and client in C++. This is the recommended path. |
| **C API** | You need a stable ABI for SDK distribution, or your consumer is not C++. |
| **Python** | You want a Python client (e.g., PySide6 GUI) talking to a C++ server. |
| **aether-lite** | Your server runs on a bare-metal MCU (Cortex-M, AVR) with no OS. |

If you are unsure, use the default C++ backend. The other backends exist
for specific integration scenarios.

## Performance Characteristics

The shared-memory transport avoids syscalls on the data path. A `call()`
writes the request frame into a lock-free SPSC ring buffer (`memcpy`), sends
a one-byte wakeup signal, and blocks on a condition variable. The server
reads from the ring, dispatches to your handler, writes the response into the
reverse ring, and signals back. There is no serialization step — the payload
is a flat `memcpy` of your struct.

Rough order-of-magnitude numbers (single-core, Linux, Release build):

| Metric | Typical range |
|--------|---------------|
| Round-trip latency (64 B payload) | Low single-digit µs |
| Round-trip latency (4 KB payload) | ~10 µs |
| Ring buffer capacity | 256 KB per direction |

These numbers depend on hardware, OS, and system load. Run the benchmarks
under [`bench/`](../bench/) to measure on your own machine.

## Next Steps

- [README.md](../README.md) — quick start, build commands, examples
- [aether-hld.md](aether-hld.md) — full architecture and component design
- [aether-lld.md](aether-lld.md) — wire protocol, APIs, threading details
- [ipcgen-hld.md](ipcgen-hld.md) — IDL grammar and code generator design
- [architecture-guide.md](architecture-guide.md) — visual walkthrough with diagrams
