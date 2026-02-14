# ms-ipc — Implementation Progress

## Architecture

```
User code (generated FooSkeleton / FooClient)
  ↓
Service / Client layer (dispatch, sync RPC, notifications)  ← Phase 3b+3c (done)
  ↓
Frame I/O (read/write framed messages through ring buffers) ← Phase 3a (done)
  ↓
Connection (handshake, shared memory, ring buffers)         ← Phase 2 (done)
  ↓
Platform (UDS, shared memory, FD passing)                   ← Phase 1 (done)
```

Users never see Platform, Connection, or ring buffers directly.
Generated code provides virtual handler methods (server) and RPC call
methods + notification callbacks (client).

Dependencies: `ms-ringbuffer` (submodule), `ms-runloop` (to be added).

---

## Phase 1: Platform Layer — DONE

**Files:** `inc/Platform.h`, `src/PlatformLinux.cpp`, `test/PlatformTest.cpp`
**Namespace:** `ms::ipc::platform`
**Tests:** 11 passing

Platform API (embedded-friendly, no `std::string`, no heap allocations):
- `serverSocket(name)` — create listening UDS (abstract namespace)
- `clientSocket(name)` — connect to UDS server
- `acceptClient(listenFd)` — accept connection
- `sendFd / recvFd` — FD passing via SCM_RIGHTS + ancillary data
- `sendSignal / recvSignal` — lightweight wakeup byte
- `shmCreate(size)` — anonymous shared memory (memfd_create)
- `closeFd(fd)` — safe close (-1 is no-op)

---

## Phase 2: Types + Connection Handshake — DONE

**Files:** `inc/Types.h`, `inc/Connection.h`, `src/Connection.cpp`, `test/ConnectionTest.cpp`
**Namespace:** `ms::ipc`
**Tests:** 5 passing (16 total)
**Dependency added:** `deps/ms-ringbuffer` (git submodule)

### Types.h
- `kProtocolVersion = 1`, `kRingSize = 256KB`
- `IpcRing` — typedef for `ms::spsc::ByteRingBuffer<kRingSize>`
- `IpcError` — error codes (negative = framework, 0 = success, positive = user)
- `FrameHeader` — 24-byte native-endian format, `FrameFlags`

### Connection (internal, not user-facing)
- `Connection` struct — holds socketFd, shmFd, shmBase, txRing, rxRing
- `connectToServer(name)` — client handshake: create shm, send FD + version, wait ACK
- `acceptConnection(listenFd)` — server handshake: receive FD + version, validate, mmap, ACK

### Shared memory layout
```
[ IpcRing: client→server ]  [ IpcRing: server→client ]
|-------- Ring 0 --------|  |-------- Ring 1 --------|

Client: tx = Ring 0, rx = Ring 1
Server: rx = Ring 0, tx = Ring 1
```

### Connection tests
1. BasicHandshake — both sides get valid connections
2. RingBufferDataFlow — bidirectional data through shared ring buffers
3. VersionMismatch — wrong version rejected
4. MultipleClients — 3 independent connections
5. ConnectionClose — cleanup verified

---

## Phase 3a: Frame I/O — DONE

**Files:** `inc/FrameIO.h`, `src/FrameIO.cpp`, `test/FrameIOTest.cpp`
**Namespace:** `ms::ipc`
**Tests:** 9 passing (25 total)

Read and write framed messages (FrameHeader + payload) through ring buffers.
No endian conversion — same-machine IPC via shared memory.

### API
- `writeFrame(ring, header, payload, len)` — atomic write, returns `IPC_ERR_RING_FULL` if no space
- `peekFrameHeader(ring, header)` — non-consuming header peek
- `readFrame(ring, header, payload, bufSize)` — fixed-buffer read
- `readFrameAlloc(ring, header, payload)` — vector-based convenience read

### Frame I/O tests
1. WriteAndReadFrame — round-trip with all header fields verified
2. PeekDoesNotConsume — peek leaves ring state unchanged
3. RingFullReturnsError — insufficient space detected, ring unchanged
4. EmptyRingReturnsError — peek/read on empty ring
5. MultipleFrames — 3 frames written and read in order
6. ZeroPayload — frame with no payload body
7. ReadFrameAlloc — vector-based read convenience API
8. LargePayload — ~200KB payload round-trip
9. PayloadTooLargeForBuffer — undersized buffer rejected, frame recoverable

---

## Phase 3b: ServiceBase — DONE

**Files:** `inc/ServiceBase.h`, `src/ServiceBase.cpp`, `test/ServiceBaseTest.cpp`
**Namespace:** `ms::ipc`
**Tests:** 8 passing (33 total)

Server-side base class for IPC services. Generated FooSkeleton inherits from
ServiceBase and implements `onRequest()` as a switch on messageId. Users never
instantiate ServiceBase directly.

### Threading model
- **Accept thread** — blocks on `acceptConnection(m_listenFd)`, spawns a
  receiver thread per client
- **Receiver thread (per client)** — blocks on `recvSignal()`, drains frames,
  dispatches `FRAME_REQUEST` via virtual `onRequest()`, sends `FRAME_RESPONSE`

### API
- `start()` — creates listening socket, spawns accept thread
- `stop()` — two-phase shutdown: (1) close listen socket, join accept thread;
  (2) shutdown all client sockets, join receiver threads, cleanup
- `isRunning()` — atomic running flag
- `onRequest(messageId, request, response)` — pure virtual dispatch point
- `sendNotify(serviceId, messageId, payload, len)` — broadcast FRAME_NOTIFY
  to all connected clients

### ServiceBase tests
1. StartAndStop — lifecycle works without connections
2. SingleRequestResponse — echo request, verify response matches
3. InvalidMethodReturnsError — unknown messageId returns error in aux
4. MultipleRequestsOnSameConnection — 3 sequential requests, correct seq
5. MultipleClients — 2 clients get independent correct responses
6. NotifyBroadcast — 2 clients both receive notification
7. StopCleansUpConnections — client detects disconnect after stop
8. ClientDisconnectDoesNotCrash — service continues after client disconnect

## Phase 3c: ClientBase — DONE

**Files:** `inc/ClientBase.h`, `src/ClientBase.cpp`, `test/ClientBaseTest.cpp`
**Namespace:** `ms::ipc`
**Tests:** 9 passing (42 total)

Client-side base class for IPC services. Generated FooClient inherits from
ClientBase and provides typed call wrappers and virtual notification callbacks.

### Threading model
- **Receiver thread** — blocks on `recvSignal()`, drains frames, dispatches
  `FRAME_RESPONSE` to matching PendingCall, `FRAME_NOTIFY` to virtual callback

### API
- `connect()` — handshake via `connectToServer()`, spawns receiver thread
- `disconnect()` — shutdown socket, join receiver, fail pending calls, cleanup
- `isConnected()` — atomic running flag
- `call(serviceId, messageId, request, response, timeoutMs)` — synchronous RPC
  with condition_variable wait and timeout
- `onNotification(serviceId, messageId, payload)` — virtual callback (default no-op)

### Sync RPC mechanism
- Atomic sequence counter correlates requests to responses
- `PendingCall` struct with condition_variable, done flag, status, response
- `unordered_map<seq, shared_ptr<PendingCall>>` tracks in-flight calls
- Receiver thread sets `done=true` and notifies when response arrives
- Caller blocks on `cv.wait_for()` with configurable timeout

### ClientBase tests
1. ConnectAndDisconnect — lifecycle works
2. SingleCall — echo RPC round-trip, response matches
3. InvalidMethodReturnsError — unknown messageId returns error
4. MultipleCalls — 3 sequential calls, correct responses
5. CallTimeout — no response within timeout returns IPC_ERR_TIMEOUT
6. DisconnectFailsPendingCalls — in-flight call fails on disconnect
7. ReceiveNotification — service notification arrives via onNotification
8. CallAfterDisconnect — returns IPC_ERR_DISCONNECTED
9. ServerStopDisconnectsClient — client detects server shutdown

---

## Build & Verify

```bash
cd /home/litu/sandbox/cpp/ms-ipc
git submodule update --init --recursive
python3 build.py -c -t
```

42 tests passing (11 platform + 5 connection + 9 frame I/O + 8 service base + 9 client base).
