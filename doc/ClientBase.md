# ClientBase Walkthrough

ClientBase is the client-side base class for IPC services. Generated
FooClient classes inherit from it and provide typed call wrappers
(e.g., `fooMethod(req) -> response`) and virtual notification callbacks.

```mermaid
classDiagram
    class ClientBase {
        +connect() bool
        +disconnect() void
        +isConnected() bool
        +call(serviceId, messageId, request, response, timeout) int
        #onNotification(serviceId, messageId, payload) void
        -receiverLoop() void
        -m_conn : Connection
        -m_running : atomic~bool~
        -m_nextSeq : atomic~uint32_t~
        -m_pending : map~seq, PendingCall~
    }
    class FooClient {
        <<generated, future>>
        +method1(req) Response
        +method2(req) Response
        #onNotification() void
    }
    ClientBase <|-- FooClient
```

## Files

| File | Purpose |
|------|---------|
| `inc/ClientBase.h` | Class declaration |
| `src/ClientBase.cpp` | Connect, sync RPC, receiver thread, notifications |

## Lifecycle

### Connecting

```cpp
ClientBase client("my-service");
client.connect();  // handshake + spawn receiver thread
```

```mermaid
sequenceDiagram
    participant App as Application
    participant CB as ClientBase
    participant S as Server

    App->>CB: connect()
    CB->>S: connectToServer(name) — full handshake
    Note over CB,S: Shared memory established
    Note over CB: Spawn receiver thread
    CB-->>App: return true
```

`connect()` calls `connectToServer()` from Connection.h — creates shared
memory, sends the FD to the server, waits for ACK. On success, spawns
the receiver thread.

### Disconnecting

```cpp
client.disconnect();
```

```mermaid
sequenceDiagram
    participant App as Application
    participant CB as disconnect()
    participant RT as Receiver Thread
    participant PC as Pending Calls

    App->>CB: disconnect()
    CB->>RT: shutdown(socketFd, SHUT_RDWR)
    Note over RT: recvSignal() returns -1, exits loop
    CB->>RT: join()
    CB->>PC: Set all status = IPC_ERR_DISCONNECTED
    CB->>PC: notify_one() on each, clear map
    Note over CB: conn.close()
```

The destructor calls `disconnect()` automatically.

## Synchronous RPC — call()

```cpp
int call(uint32_t serviceId, uint32_t messageId,
         const std::vector<uint8_t> &request,
         std::vector<uint8_t> *response,
         uint32_t timeoutMs = 2000);
```

### How it works

```mermaid
sequenceDiagram
    participant App as Caller thread
    participant CB as call()
    participant Ring as tx ring (shm)
    participant RT as Receiver thread
    participant SRV as Server

    App->>CB: call(serviceId, messageId, request)
    Note over CB: seq = m_nextSeq++
    CB->>Ring: writeFrame(FRAME_REQUEST, seq)
    Note over CB: m_pending[seq] = PendingCall
    CB->>SRV: sendSignal()

    Note over App: blocked on cv.wait_for(timeout)

    SRV-->>RT: sendSignal() (response ready)
    RT->>RT: readFrameAlloc(FRAME_RESPONSE)
    RT->>RT: m_pending[seq].done = true
    RT->>App: cv.notify_one()

    Note over App: wakes up
    App->>CB: return status, move response
```

### Sequence number correlation

```mermaid
graph LR
    subgraph "Caller thread"
        C1["call() seq=1"]
        C2["call() seq=2"]
        C3["call() seq=3"]
    end

    subgraph "m_pending map"
        P1["PendingCall 1"]
        P2["PendingCall 2"]
        P3["PendingCall 3"]
    end

    subgraph "Receiver thread"
        R["response.seq → lookup → notify"]
    end

    C1 --> P1
    C2 --> P2
    C3 --> P3
    R --> P1
    R --> P2
    R --> P3
```

Each call generates a unique sequence number via `m_nextSeq.fetch_add(1)`.
The server echoes the sequence number in the response frame. The receiver
thread looks up the matching PendingCall by seq and notifies the waiting
caller.

### Error returns

| Return value | Meaning |
|-------------|---------|
| `IPC_SUCCESS` (0) | Response received successfully |
| `IPC_ERR_DISCONNECTED` (-1) | Not connected or connection lost |
| `IPC_ERR_TIMEOUT` (-2) | No response within timeoutMs |
| `IPC_ERR_RING_FULL` (-3) | Ring buffer full (request too large) |
| Server status (from aux) | Server-returned error code |

## Receiver thread

```mermaid
graph TD
    START["receiverLoop()"] --> WAIT["recvSignal()"]
    WAIT -->|"signal received"| DRAIN["Drain rx ring"]
    DRAIN --> CHECK{"Frame type?"}
    CHECK -->|"FRAME_RESPONSE"| RESP["Find PendingCall by seq\nSet status + response\ndone = true\ncv.notify_one()"]
    CHECK -->|"FRAME_NOTIFY"| NOTIFY["onNotification(\nserviceId, messageId, payload)"]
    RESP --> DRAIN
    NOTIFY --> DRAIN
    DRAIN -->|"no more frames"| WAIT
    WAIT -->|"error / shutdown"| EXIT["Fail remaining pending calls\nwith IPC_ERR_DISCONNECTED"]
```

The receiver thread handles two frame types:
- **FRAME_RESPONSE** — matches to a pending `call()` by sequence number
- **FRAME_NOTIFY** — dispatched to virtual `onNotification()`

## Notification callbacks

```cpp
// Default implementation does nothing.
virtual void onNotification(uint32_t serviceId, uint32_t messageId,
                            const std::vector<uint8_t> &payload);
```

Not pure virtual — clients that don't need notifications don't have to
override it. Generated FooClient overrides this with a switch on messageId,
calling typed virtual methods like `onTemperatureChanged(payload)`.

## Typical usage

```cpp
// Direct usage (without generated code):
ClientBase client("echo-service");
client.connect();

std::vector<uint8_t> request = {1, 2, 3};
std::vector<uint8_t> response;
int rc = client.call(1, 1, request, &response);
if (rc == IPC_SUCCESS) {
    // response contains the server's reply
}

client.disconnect();
```

## Design decisions

**Virtual `onNotification()` instead of `std::function`** — matches the
ServiceBase pattern. No handler mutex needed. Generated code overrides it
with a switch, calling typed virtual methods that users implement.

**`Connection` struct** — reuses the existing `connectToServer()` handshake
rather than reimplementing shared memory setup. Single member variable
instead of separate fd/shm/region fields.

**`shared_ptr<PendingCall>`** — the PendingCall is shared between the caller
(which waits on the cv) and the receiver thread (which sets done and notifies).
Using shared_ptr ensures the PendingCall lives long enough even if the map
entry is erased during cleanup.

**Atomic sequence counter** — `m_nextSeq.fetch_add(1)` is lock-free and
generates unique sequence numbers without contention. Safe for concurrent
calls from multiple threads.

**Timeout per call** — each call has its own timeout (default 2000ms). The
condition_variable `wait_for` handles this efficiently without a separate
timer thread.
