# ms-ipc Architecture

## Layer stack

```mermaid
graph TD
    subgraph "User code"
        US["FooService : FooSkeleton"]
        UC["FooClient (generated)"]
    end

    subgraph "Generated code (future)"
        SK["FooSkeleton : ServiceBase"]
        CL["FooClient : ClientBase"]
    end

    subgraph "Service layer"
        SB["ServiceBase"]
        CB["ClientBase (planned)"]
    end

    subgraph "Frame I/O"
        FIO["writeFrame / readFrame / readFrameAlloc"]
    end

    subgraph "Connection"
        CON["connectToServer / acceptConnection"]
        SHM["Shared Memory (2x IpcRing)"]
    end

    subgraph "Platform"
        UDS["UDS Sockets"]
        FD["FD Passing (SCM_RIGHTS)"]
        MEM["memfd_create / mmap"]
        SIG["sendSignal / recvSignal"]
    end

    US --> SK
    UC --> CL
    SK --> SB
    CL --> CB
    SB --> FIO
    CB --> FIO
    FIO --> SHM
    CON --> UDS
    CON --> FD
    CON --> MEM
    SB --> SIG
    CB --> SIG
    SB --> CON
    CB --> CON
```

## Data plane vs control plane

The key architectural insight: **data never touches the socket**. The UDS
socket is only used for lightweight signaling and the initial handshake.
All payload data flows through shared memory ring buffers.

```mermaid
graph LR
    subgraph "Client Process"
        CA["Application"]
        CTX["tx ring"]
        CRX["rx ring"]
    end

    subgraph "Shared Memory"
        R0["Ring 0\n(client → server)"]
        R1["Ring 1\n(server → client)"]
    end

    subgraph "Server Process"
        SA["ServiceBase"]
        STX["tx ring"]
        SRX["rx ring"]
    end

    CA -- "write request" --> CTX
    CTX -. "zero-copy" .-> R0
    R0 -. "zero-copy" .-> SRX
    SRX -- "read request" --> SA

    SA -- "write response" --> STX
    STX -. "zero-copy" .-> R1
    R1 -. "zero-copy" .-> CRX
    CRX -- "read response" --> CA

    CA -- "sendSignal() · UDS 1 byte" --> SA
    SA -- "sendSignal() · UDS 1 byte" --> CA
```

## Connection establishment

```mermaid
sequenceDiagram
    participant C as Client
    participant UDS as UDS Socket
    participant S as Server (accept thread)

    C->>UDS: clientSocket(name)
    S->>UDS: acceptClient(listenFd)
    Note over C: memfd_create + mmap
    Note over C: placement-new 2x IpcRing
    C->>S: sendFd(shmFd, version)
    Note over S: Validate version
    Note over S: fstat + mmap shared memory
    Note over S: Assign rx=Ring0, tx=Ring1
    S->>C: sendSignal() (ACK)
    Note over C: Assign tx=Ring0, rx=Ring1
    Note over C,S: Connection established — both sides share memory
```

## Frame format

```mermaid
packet-beta
    0-15: "version (u16)"
    16-31: "flags (u16)"
    32-63: "serviceId (u32)"
    64-95: "messageId (u32)"
    96-127: "seq (u32)"
    128-159: "payloadBytes (u32)"
    160-191: "aux (u32)"
```

Frames are written contiguously into the ring buffer:

```mermaid
block-beta
    columns 5
    H["FrameHeader (24 bytes)"]:2
    P["Payload (N bytes)"]:3
```

Flag values:
- `FRAME_REQUEST` (0x01) — client → server RPC call
- `FRAME_RESPONSE` (0x02) — server → client reply (status in `aux`)
- `FRAME_NOTIFY` (0x04) — server → client broadcast

## Request/response flow

```mermaid
sequenceDiagram
    participant C as Client
    participant TR as tx ring (shm)
    participant RR as rx ring (shm)
    participant R as Receiver Thread
    participant H as onRequest()

    C->>TR: writeFrame(FRAME_REQUEST)
    C->>R: sendSignal()
    Note over R: recvSignal() unblocks
    R->>TR: readFrameAlloc()
    R->>H: onRequest(messageId, payload)
    H-->>R: return status + response
    R->>RR: writeFrame(FRAME_RESPONSE)
    R->>C: sendSignal()
    Note over C: recvSignal() unblocks
    C->>RR: readFrameAlloc()
```

## Notification broadcast

```mermaid
sequenceDiagram
    participant S as Service code
    participant SB as ServiceBase
    participant C1 as Client 1 (shm + socket)
    participant C2 as Client 2 (shm + socket)

    S->>SB: sendNotify(serviceId, messageId, payload)
    Note over SB: Lock m_clientsMutex
    SB->>C1: writeFrame(FRAME_NOTIFY) + sendSignal()
    SB->>C2: writeFrame(FRAME_NOTIFY) + sendSignal()
    Note over SB: Unlock
    Note over C1: recvSignal() → readFrameAlloc()
    Note over C2: recvSignal() → readFrameAlloc()
```

## ServiceBase threading model

```mermaid
graph TD
    START["svc.start()"] --> AT["Accept Thread"]
    AT -->|"acceptConnection()"| AT
    AT -->|"new client"| RT1["Receiver Thread 1"]
    AT -->|"new client"| RT2["Receiver Thread 2"]
    AT -->|"new client"| RTN["Receiver Thread N"]

    RT1 -->|"recvSignal()"| DRAIN1["Drain rx ring"]
    DRAIN1 -->|"FRAME_REQUEST"| DISPATCH1["onRequest()"]
    DISPATCH1 --> RESPOND1["writeFrame() + sendSignal()"]
    RESPOND1 --> RT1

    RT2 -->|"recvSignal()"| DRAIN2["Drain rx ring"]
    DRAIN2 --> DISPATCH2["onRequest()"]
    DISPATCH2 --> RESPOND2["writeFrame() + sendSignal()"]
    RESPOND2 --> RT2

    STOP["svc.stop()"] -->|"Phase 1"| SHUTDOWN_LISTEN["shutdown(listenFd)"]
    SHUTDOWN_LISTEN --> JOIN_AT["Join accept thread"]
    JOIN_AT -->|"Phase 2"| SHUTDOWN_CLIENTS["shutdown() all client sockets"]
    SHUTDOWN_CLIENTS --> JOIN_RT["Join all receiver threads"]
    JOIN_RT --> CLEANUP["close() + clear"]
```

## Two-phase shutdown

```mermaid
sequenceDiagram
    participant Caller as svc.stop()
    participant AT as Accept Thread
    participant RT1 as Receiver Thread 1
    participant RT2 as Receiver Thread 2

    Note over Caller: Phase 1 — stop accepting
    Caller->>AT: shutdown(listenFd, SHUT_RDWR)
    Note over AT: acceptConnection() returns invalid
    Note over AT: loop exits (m_running == false)
    Caller->>AT: join()

    Note over Caller: Phase 2 — stop receivers
    Caller->>RT1: shutdown(clientFd, SHUT_RDWR)
    Caller->>RT2: shutdown(clientFd, SHUT_RDWR)
    Note over RT1: recvSignal() returns -1, loop exits
    Note over RT2: recvSignal() returns -1, loop exits
    Caller->>RT1: join()
    Caller->>RT2: join()
    Note over Caller: close() all connections, clear vector
```

## Class hierarchy (current + planned)

```mermaid
classDiagram
    class ServiceBase {
        <<abstract>>
        +start() bool
        +stop() void
        +isRunning() bool
        #onRequest(messageId, request, response)* int
        #sendNotify(serviceId, messageId, payload, len) int
        -acceptLoop() void
        -receiverLoop(client) void
        -m_serviceName : string
        -m_listenFd : int
        -m_running : atomic~bool~
        -m_acceptThread : thread
        -m_clientsMutex : mutex
        -m_clients : vector~ClientConn~
    }

    class FooSkeleton {
        <<generated>>
        #onRequest() int
        #notifyEvent(payload) int
    }

    class FooService {
        <<user code>>
        +handleMethod1(req) Response
        +handleMethod2(req) Response
    }

    ServiceBase <|-- FooSkeleton : inherits
    FooSkeleton <|-- FooService : inherits

    class ClientBase {
        <<planned>>
        +connect(name) bool
        +disconnect() void
        +call(messageId, request, response, timeout) int
        #onNotification(messageId, payload)* void
    }

    class FooClient {
        <<generated>>
        +method1(req) Response
        +method2(req) Response
        #onNotification() void
    }

    ClientBase <|-- FooClient : inherits

    class Connection {
        +socketFd : int
        +shmFd : int
        +shmBase : void*
        +txRing : IpcRing*
        +rxRing : IpcRing*
        +valid() bool
        +close() void
    }

    ServiceBase *-- Connection : manages N
    ClientBase *-- Connection : owns 1
```

## Shared memory layout (per connection)

```mermaid
graph LR
    subgraph "memfd (mmap'd in both processes)"
        subgraph "Ring 0 — sizeof(IpcRing) ≈ 256KB"
            R0H["Control block\n(head, tail atomics)"]
            R0D["Data buffer\n(256 KB)"]
        end
        subgraph "Ring 1 — sizeof(IpcRing) ≈ 256KB"
            R1H["Control block\n(head, tail atomics)"]
            R1D["Data buffer\n(256 KB)"]
        end
    end

    C["Client"] -- "tx (write)" --> R0D
    R0D -- "rx (read)" --> S["Server"]
    S -- "tx (write)" --> R1D
    R1D -- "rx (read)" --> C
```

## Error code space

```mermaid
graph LR
    subgraph "Negative — framework errors"
        E1["-1 IPC_ERR_DISCONNECTED"]
        E2["-2 IPC_ERR_TIMEOUT"]
        E3["-3 IPC_ERR_RING_FULL"]
        E4["-4 IPC_ERR_INVALID_METHOD"]
        E5["-5 IPC_ERR_VERSION"]
    end

    subgraph "Zero"
        E0["0 IPC_SUCCESS"]
    end

    subgraph "Positive — user-defined"
        EU["1+ application errors"]
    end

    E1 --- E0
    E0 --- EU
```

## What's built vs planned

```mermaid
timeline
    title ms-ipc Implementation Progress
    Phase 1 (done) : Platform layer
                   : UDS sockets, shared memory
                   : FD passing, signaling
                   : 11 tests
    Phase 2 (done) : Connection handshake
                   : Shared memory setup
                   : Bidirectional ring buffers
                   : 5 tests
    Phase 3a (done) : Frame I/O
                    : writeFrame, readFrame
                    : peekFrameHeader
                    : readFrameAlloc
                    : 9 tests
    Phase 3b (done) : ServiceBase
                    : Accept + receiver threads
                    : Virtual dispatch
                    : Notification broadcast
                    : 8 tests
    Phase 3c (planned) : ClientBase
                       : connect / disconnect
                       : Sync call with timeout
                       : Notification callbacks
    Future : Code generation
           : IDL → FooSkeleton + FooClient
           : End-to-end RPC
```
