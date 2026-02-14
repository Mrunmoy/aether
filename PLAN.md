# ms-ipc — Implementation Progress

## Architecture

```
User code (generated FooSkeleton / FooClient)
  ↓
Service layer (manages connections, dispatches messages)    ← Phase 3 (next)
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
- `FrameHeader` — 24-byte wire format, `FrameFlags`

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

## Phase 3: Service Layer — NEXT

- Add `ms-runloop` as submodule dependency
- `Service` class with `start()` / `stop()` — manages listening socket,
  accepts connections, multiplexes via epoll, dispatches incoming frames
- Frame encoding/decoding over ring buffers
- `call()` (request/response) and `notify()` (fire-and-forget)
- Base classes that generated code (FooSkeleton, FooClient) will inherit from

---

## Build & Verify

```bash
cd /home/litu/sandbox/cpp/ms-ipc
git submodule update --init --recursive
python3 build.py -c -t
```

16 tests passing (11 platform + 5 connection).
