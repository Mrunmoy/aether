# MS-IPC Complete C++ API Reference for Stress Testing

## Overview
This document contains EXACT type definitions, method signatures, constants, and usage patterns for writing stress tests in the ms-ipc codebase.

---

## 1. TYPE DEFINITIONS & CONSTANTS

### From: `inc/Types.h`

```cpp
namespace ms::ipc {

    // === Protocol Constants ===
    constexpr uint16_t kProtocolVersion = 1;
    constexpr uint32_t kRingSize = 256 * 1024;          // 256KB per direction
    
    // Ring buffer type for IPC data plane
    using IpcRing = ms::spsc::ByteRingBuffer<kRingSize>;

    // === Error Codes (negative=framework, 0=success, positive=user-defined) ===
    enum IpcError : int {
        IPC_SUCCESS              = 0,
        IPC_ERR_DISCONNECTED     = -1,   // Not connected or connection lost
        IPC_ERR_TIMEOUT          = -2,   // No response within timeoutMs
        IPC_ERR_INVALID_SERVICE  = -3,
        IPC_ERR_INVALID_METHOD   = -4,
        IPC_ERR_VERSION_MISMATCH = -5,
        IPC_ERR_RING_FULL        = -6,   // Ring buffer full
        IPC_ERR_STOPPED          = -7,
        IPC_ERR_INVALID_ARGUMENT = -8,
    };

    // === Frame Header (24 bytes, native endian, same-machine IPC only) ===
    struct FrameHeader {
        uint16_t version;        // Must be kProtocolVersion (1)
        uint16_t flags;          // FrameFlags (REQUEST | RESPONSE | NOTIFY)
        uint32_t serviceId;      // Service identifier
        uint32_t messageId;      // Message/method identifier
        uint32_t seq;            // Sequence number (correlates request to response)
        uint32_t payloadBytes;   // Length of payload data
        uint32_t aux;            // Auxiliary field (status code in responses)
    };
    
    static_assert(sizeof(FrameHeader) == 24, "FrameHeader must be 24 bytes");

    // === Frame Flags ===
    enum FrameFlags : uint16_t {
        FRAME_REQUEST  = 0x0001,        // Request message
        FRAME_RESPONSE = 0x0002,        // Response message
        FRAME_NOTIFY   = 0x0004,        // Notification broadcast
    };
}
```

---

## 2. FRAME I/O API

### From: `inc/FrameIO.h`

```cpp
namespace ms::ipc {

    // === Constants ===
    constexpr uint32_t kMaxPayload = kRingSize - sizeof(FrameHeader);
    // Effectively: 262,144 - 24 = 262,120 bytes

    // === Write Frame (Atomic) ===
    inline int writeFrame(IpcRing *ring, 
                          const FrameHeader &header,
                          const uint8_t *payload,
                          uint32_t payloadBytes)
    /**
     * Atomically writes complete frame (header + payload) to ring buffer.
     * Either entire frame writes or nothing writes (all-or-nothing).
     * 
     * Returns:
     *   - IPC_SUCCESS: Frame written successfully
     *   - IPC_ERR_RING_FULL: Ring full OR payloadBytes > kMaxPayload
     * 
     * SPSC Invariant: Only ONE producer can call this per IpcRing
     * (enforced by sendMutex in framework)
     */

    // === Peek Frame Header (Non-Consuming) ===
    inline bool peekFrameHeader(const IpcRing *ring, 
                                FrameHeader *header)
    /**
     * Peeks at next frame header without consuming it.
     * Safe to call multiple times on same data.
     * 
     * Returns:
     *   - true: Complete header is available
     *   - false: Incomplete header or ring empty
     * 
     * Does NOT consume data from ring.
     * SPSC Invariant: Only ONE consumer can call this per IpcRing
     */

    // === Read Frame (Consuming, Fixed Buffer) ===
    inline int readFrame(IpcRing *ring,
                         FrameHeader *header,
                         uint8_t *payload,
                         uint32_t payloadBufSize)
    /**
     * Reads complete frame (header + payload) from ring buffer.
     * 
     * Returns:
     *   - IPC_SUCCESS: Frame read successfully
     *   - IPC_ERR_DISCONNECTED: Not enough data OR oversized payload (> kMaxPayload)
     *   - IPC_ERR_RING_FULL: Payload buffer too small for frame's payload
     * 
     * Usage pattern:
     *   1. Call peekFrameHeader() to learn frame.payloadBytes
     *   2. Allocate/select buffer with size >= frame.payloadBytes
     *   3. Call readFrame() to consume header + payload
     * 
     * SPSC Invariant: Only ONE consumer can call this per IpcRing
     */

    // === Read Frame (Consuming, Vector Convenience) ===
    int readFrameAlloc(IpcRing *ring,
                       FrameHeader *header,
                       std::vector<uint8_t> *payload)
    /**
     * Convenience wrapper that auto-resizes vector to fit payload.
     * Implementation location: src/FrameIO.cpp
     * 
     * Returns:
     *   - IPC_SUCCESS: Frame read successfully
     *   - IPC_ERR_DISCONNECTED: Not enough data OR oversized payload
     * 
     * SPSC Invariant: Only ONE consumer can call this per IpcRing
     */
}
```

---

## 3. RING BUFFER API

### From: `deps/ms-ringbuffer/inc/spsc/RingBuffer.h`

```cpp
namespace ms::spsc {

    // === Specialization for ms-ipc ===
    // IpcRing = RingBuffer<uint8_t, 262144, 64>
    
    template <uint32_t Capacity, uint32_t CacheLineSize = 64>
    class RingBuffer<uint8_t, Capacity, CacheLineSize> {
    public:
        using value_type = uint8_t;
        static constexpr uint32_t Mask = Capacity - 1;

        // === Producer API ===
        
        uint32_t writeAvailable() const
        /**
         * Returns number of bytes available for writing.
         * PRODUCER ONLY
         */

        bool write(const uint8_t *data, uint32_t count)
        /**
         * Writes count bytes from data into ring buffer.
         * All-or-nothing: Either all count bytes write or nothing writes.
         * 
         * Returns:
         *   - true: All count bytes written successfully
         *   - false: Insufficient space in ring (buffer unchanged)
         * 
         * PRODUCER ONLY
         */

        bool push(const uint8_t &item)
        /**
         * Shorthand for write(&item, 1).
         * PRODUCER ONLY
         */

        // === Consumer API ===
        
        uint32_t readAvailable() const
        /**
         * Returns number of bytes available for reading.
         * CONSUMER ONLY
         */

        bool read(uint8_t *dest, uint32_t count)
        /**
         * Reads and consumes count bytes from ring buffer into dest.
         * 
         * Returns:
         *   - true: All count bytes read successfully
         *   - false: Insufficient data in ring (buffer unchanged)
         * 
         * CONSUMER ONLY
         */

        bool peek(uint8_t *dest, uint32_t count) const
        /**
         * Reads count bytes from ring without consuming.
         * 
         * Returns:
         *   - true: All count bytes available and peeked
         *   - false: Insufficient data in ring
         * 
         * Does NOT consume data. Safe to call multiple times.
         * CONSUMER ONLY
         */

        bool pop(uint8_t &item)
        /**
         * Shorthand for read(&item, 1).
         * CONSUMER ONLY
         */

        bool skip(uint32_t count)
        /**
         * Consumes count bytes without reading.
         * 
         * Returns:
         *   - true: count bytes skipped successfully
         *   - false: Insufficient data in ring
         * 
         * CONSUMER ONLY
         */

        // === Info Methods ===
        
        static constexpr uint32_t capacity()
        /**
         * For IpcRing: returns 262,144 (256*1024)
         */

        static constexpr uint32_t cacheLineSize()
        /**
         * Default 64 bytes (tunable for M1/ARM: 128)
         */

        bool isEmpty() const
        /**
         * Returns true if readAvailable() == 0
         */

        bool isFull() const
        /**
         * Returns true if writeAvailable() == 0
         */

        void reset()
        /**
         * Resets buffer to empty state.
         * Sets head and tail to 0.
         */
    };

    // === Convenience Type ===
    template <uint32_t Size>
    using ByteRingBuffer = RingBuffer<uint8_t, Size>;
}
```

**Memory Layout in Shared Memory:**
```
[ControlBlock]      (128 bytes: 2x64-byte cache lines, head and tail atomics)
[Data Array]        (262,144 bytes: ring buffer data)
Total per ring: ~262KB
```

---

## 4. CONNECTION MANAGEMENT

### From: `inc/Connection.h` and `src/Connection.cpp`

```cpp
namespace ms::ipc {

    struct Connection {
        int socketFd = -1;              // UDS socket for signaling
        int shmFd = -1;                 // Shared memory file descriptor
        void *shmBase = nullptr;        // mmap'd base pointer
        uint32_t shmSize = 0;           // Total shared memory region size

        IpcRing *txRing = nullptr;      // Ring buffer for sending
        IpcRing *rxRing = nullptr;      // Ring buffer for receiving

        Connection() = default;
        
        // Non-copyable: owns fds and mmap'd memory
        Connection(const Connection &) = delete;
        Connection &operator=(const Connection &) = delete;

        // Move transfers ownership and resets source
        Connection(Connection &&other) noexcept
            : socketFd(other.socketFd), shmFd(other.shmFd), 
              shmBase(other.shmBase), shmSize(other.shmSize),
              txRing(other.txRing), rxRing(other.rxRing) {
            other.socketFd = -1;
            other.shmFd = -1;
            other.shmBase = nullptr;
            other.shmSize = 0;
            other.txRing = nullptr;
            other.rxRing = nullptr;
        }

        Connection &operator=(Connection &&other) noexcept;  // Move assignment

        bool valid() const
        /**
         * Returns true if socketFd >= 0 && shmBase != nullptr
         */

        void close()
        /**
         * Clean up: munmap, close fds, reset all fields.
         * Safe to call multiple times.
         */
    };

    // === Shared Memory Layout ===
    // Total SHM size: 2 * sizeof(IpcRing) = ~512KB
    //
    // [IpcRing: client→server (Ring 0)]  [IpcRing: server→client (Ring 1)]
    // |-------- Ring 0 --------|         |-------- Ring 1 --------|
    //
    // Client: txRing = Ring 0, rxRing = Ring 1
    // Server: rxRing = Ring 0, txRing = Ring 1

    // === Client Handshake ===
    Connection connectToServer(const char *name)
    /**
     * Establishes client connection to service (client side).
     * 
     * Parameters:
     *   name: Service name (e.g., "echo" → UDS \0ipc_echo)
     * 
     * Process:
     *   1. Connect to server via UDS socket
     *   2. Set socket send timeout (5 seconds)
     *   3. Create anonymous shared memory
     *   4. mmap the shared memory
     *   5. Placement-new two IpcRing buffers in shmBase
     *   6. Send protocol version + shmFd to server
     *   7. Wait for ACK from server
     * 
     * Returns:
     *   - Valid Connection (socketFd >= 0) on success
     *   - Invalid Connection (socketFd == -1) on failure
     */

    // === Server Handshake ===
    Connection acceptConnection(int listenFd)
    /**
     * Accepts one client connection and performs handshake (server side).
     * 
     * Parameters:
     *   listenFd: Listening socket from platform::serverSocket()
     * 
     * Process:
     *   1. Accept the client connection
     *   2. Set socket send timeout (5 seconds)
     *   3. Receive protocol version + shmFd from client
     *   4. Validate protocol version matches kProtocolVersion
     *   5. Determine shared memory size via fstat
     *   6. mmap the shared memory
     *   7. Set ring buffer pointers (opposite direction from client)
     *   8. Send ACK to client
     * 
     * Returns:
     *   - Valid Connection (socketFd >= 0) on success
     *   - Invalid Connection (socketFd == -1) on failure
     */
}
```

---

## 5. PLATFORM ABSTRACTION

### From: `inc/Platform.h` and `src/PlatformLinux.cpp`

```cpp
namespace ms::ipc::platform {

    // === Unix Domain Sockets ===

    int serverSocket(const char *name)
    /**
     * Create a listening UDS socket bound to abstract namespace.
     * Linux: SOCK_SEQPACKET, path = \0ipc_<name>
     * 
     * Returns:
     *   - fd >= 0 on success
     *   - -1 on failure
     */

    int clientSocket(const char *name)
    /**
     * Connect to a UDS server in abstract namespace.
     * 
     * Returns:
     *   - fd >= 0 on success
     *   - -1 on failure
     */

    int acceptClient(int listenFd)
    /**
     * Accept a connection on a listening socket.
     * 
     * Returns:
     *   - fd >= 0 on success
     *   - -1 on failure
     */

    int sendFd(int sockFd, int fdToSend, const void *data, uint32_t dataLen)
    /**
     * Send a file descriptor + ancillary data over UDS (SCM_RIGHTS).
     * 
     * Parameters:
     *   sockFd: UDS socket
     *   fdToSend: File descriptor to send
     *   data: Ancillary data (e.g., protocol version)
     *   dataLen: Length of ancillary data
     * 
     * Returns:
     *   - 0 on success
     *   - -1 on failure
     */

    int recvFd(int sockFd, int *receivedFd, void *data, uint32_t dataLen)
    /**
     * Receive a file descriptor + ancillary data over UDS (SCM_RIGHTS).
     * 
     * Parameters:
     *   sockFd: UDS socket
     *   receivedFd: Pointer to receive fd (set on success)
     *   data: Buffer for ancillary data
     *   dataLen: Max length of ancillary data to receive
     * 
     * Returns:
     *   - bytes received (> 0) on success
     *   - -1 on failure
     */

    int sendSignal(int sockFd)
    /**
     * Send a single wakeup byte (triggers epoll/poll on peer).
     * 
     * Returns:
     *   - 0 on success
     *   - -1 on failure
     */

    int recvSignal(int sockFd)
    /**
     * Receive a single wakeup byte (blocks until available).
     * 
     * Returns:
     *   - 0 on success
     *   - -1 on failure
     */

    int setSocketTimeouts(int sockFd, uint32_t timeoutMs)
    /**
     * Set a timeout on socket send operations (SO_SNDTIMEO only).
     * 
     * Parameters:
     *   sockFd: Socket to configure
     *   timeoutMs: Timeout in milliseconds
     * 
     * Returns:
     *   - 0 on success
     *   - -1 on failure
     */

    // === Shared Memory ===

    int shmCreate(uint32_t size)
    /**
     * Create anonymous shared memory region of size bytes.
     * Linux: memfd_create + ftruncate
     * 
     * Returns:
     *   - fd >= 0 on success
     *   - -1 on failure
     */

    // === File Descriptor Management ===

    void closeFd(int fd)
    /**
     * Close a file descriptor. Safe to call with -1 (no-op).
     */
}
```

---

## 6. SERVICE BASE CLASS

### From: `inc/ServiceBase.h` and `src/ServiceBase.cpp`

```cpp
namespace ms::ipc {

    class ServiceBase {
    public:
        explicit ServiceBase(const char *serviceName, 
                            ms::RunLoop *loop = nullptr);
        virtual ~ServiceBase();

        // Non-copyable
        ServiceBase(const ServiceBase &) = delete;
        ServiceBase &operator=(const ServiceBase &) = delete;

        // === Lifecycle ===

        bool start()
        /**
         * Start accepting client connections.
         * Spawns accept thread (if no RunLoop) or registers with RunLoop.
         * 
         * Returns:
         *   - true: Successfully started
         *   - false: Already running
         */

        void stop()
        /**
         * Stop service and close all client connections.
         * Joins all receiver threads and accept thread.
         * Safe to call multiple times.
         */

        bool isRunning() const
        /**
         * Returns true if service is currently active.
         */

    protected:
        // === Virtual Dispatch Point (Override in Subclass) ===

        virtual int onRequest(uint32_t messageId,
                             const std::vector<uint8_t> &request,
                             std::vector<uint8_t> *response) = 0
        /**
         * Called on receiver thread for each incoming FRAME_REQUEST.
         * Subclass implements this as a switch on messageId.
         * 
         * Parameters:
         *   messageId: From request frame header
         *   request: Full request payload from frame
         *   response: Vector to populate with response payload
         *             (pre-allocated by framework)
         * 
         * Returns:
         *   - 0 or positive: Success (stored in response aux field)
         *   - Negative: Error code (stored in response aux field)
         * 
         * THREAD SAFETY:
         *   - Called concurrently (one receiver thread per client)
         *   - MUST be thread-safe
         *   - Can safely call sendNotify() from here
         */

        // === Notification Broadcast ===

        int sendNotify(uint32_t serviceId, uint32_t messageId,
                       const uint8_t *payload, uint32_t payloadBytes)
        /**
         * Send a FRAME_NOTIFY to all connected clients.
         * Called by generated notifyXxx() methods or user code.
         * 
         * Parameters:
         *   serviceId: Service ID for notification frame
         *   messageId: Message ID for notification frame
         *   payload: Notification data (can be nullptr if payloadBytes == 0)
         *   payloadBytes: Length of payload data
         * 
         * Returns:
         *   - IPC_SUCCESS: All clients received (or no clients connected)
         *   - IPC_ERR_RING_FULL: Some client's ring full (client marked dead)
         *   - IPC_ERR_DISCONNECTED: sendSignal() failed on some client
         * 
         * THREAD SAFETY:
         *   - Thread-safe: Uses per-client sendMutex + global lock
         *   - Two-phase reaping: Dead clients reaped on next sendNotify() call
         *   - Can be called from any thread (including onRequest handlers)
         */

    private:
        struct ClientConn {
            Connection conn;
            std::thread thread;
            std::mutex handlerMutex;         // Guards RunLoop handler execution
            std::mutex sendMutex;            // Serializes txRing writes (SPSC)
            std::atomic<bool> dead{false};   // Set when client disconnects
        };

        void acceptLoop();
        void receiverLoop(ClientConn *client);
        void onAcceptReady();               // RunLoop handler
        void onClientReady(ClientConn *);   // RunLoop handler
        void removeClient(ClientConn *);

        std::string m_serviceName;
        ms::RunLoop *m_loop;
        int m_listenFd = -1;
        std::atomic<bool> m_running{false};
        std::thread m_acceptThread;
        std::mutex m_clientsMutex;
        std::vector<std::unique_ptr<ClientConn>> m_clients;
    };

    // === Threading Model ===
    // Standalone mode (no RunLoop):
    //   - 1 accept thread waits for client connections
    //   - 1 receiver thread per connected client
    //   - onRequest() called concurrently (one thread per client)
    //
    // RunLoop mode (with RunLoop parameter):
    //   - No dedicated threads
    //   - Handlers called on RunLoop's event thread
}
```

---

## 7. CLIENT BASE CLASS

### From: `inc/ClientBase.h` and `src/ClientBase.cpp`

```cpp
namespace ms::ipc {

    class ClientBase {
    public:
        explicit ClientBase(const char *serviceName, 
                           ms::RunLoop *loop = nullptr);
        virtual ~ClientBase();

        // Non-copyable
        ClientBase(const ClientBase &) = delete;
        ClientBase &operator=(const ClientBase &) = delete;

        // === Lifecycle ===

        bool connect()
        /**
         * Connect to service.
         * Starts receiver thread (if no RunLoop) or registers with RunLoop.
         * 
         * Returns:
         *   - true: Successfully connected
         *   - false: Connection failed
         */

        void disconnect()
        /**
         * Disconnect from service.
         * Joins receiver thread and fails pending calls with IPC_ERR_DISCONNECTED.
         * Safe to call multiple times.
         */

        bool isConnected() const
        /**
         * Returns true if currently connected.
         */

        // === Synchronous RPC ===

        int call(uint32_t serviceId, uint32_t messageId,
                 const std::vector<uint8_t> &request,
                 std::vector<uint8_t> *response,
                 uint32_t timeoutMs = 2000)
        /**
         * Send request and block until response arrives or timeout.
         * 
         * Parameters:
         *   serviceId: Service ID for request frame
         *   messageId: Message/method ID for request frame
         *   request: Request payload
         *   response: Vector to populate with response payload
         *             (can be nullptr if response not needed)
         *   timeoutMs: Max time to wait for response (default 2000ms)
         * 
         * Returns:
         *   - IPC_SUCCESS: Response received successfully
         *   - IPC_ERR_DISCONNECTED: Not connected or connection lost
         *   - IPC_ERR_TIMEOUT: No response within timeoutMs
         *   - IPC_ERR_RING_FULL: Ring buffer full (server busy)
         *   - Positive/negative: Server-returned status (from response aux field)
         * 
         * Implementation details:
         *   - Allocates sequence number (atomic increment)
         *   - Registers pending call
         *   - Writes request to txRing
         *   - Sends signal to server (triggers receiver to process)
         *   - Blocks on condition variable until response arrives
         *   - Receiver thread matches responses to requests via sequence number
         * 
         * THREAD SAFETY:
         *   - Thread-safe: Can call from multiple threads simultaneously
         *   - Each call allocates unique sequence number
         *   - Blocks caller until response or timeout
         */

    protected:
        // === Notification Callback (Override in Subclass) ===

        virtual void onNotification(uint32_t serviceId, uint32_t messageId,
                                   const std::vector<uint8_t> &payload)
        /**
         * Called on receiver thread for each incoming FRAME_NOTIFY.
         * Generated FooClient overrides this as a switch on messageId.
         * Users subclass FooClient to handle notifications.
         * 
         * Parameters:
         *   serviceId: Service ID from notification frame
         *   messageId: Message ID from notification frame
         *   payload: Notification data from frame
         * 
         * Default implementation: does nothing (no-op)
         * 
         * THREAD SAFETY:
         *   - Called on receiver thread
         *   - MUST be thread-safe if using from multiple threads
         */

    private:
        struct PendingCall {
            std::condition_variable cv;
            bool done{false};
            int status{IPC_SUCCESS};
            std::vector<uint8_t> response;
        };

        void receiverLoop();
        void onDataReady();              // RunLoop handler

        std::string m_serviceName;
        ms::RunLoop *m_loop;
        Connection m_conn;
        std::atomic<bool> m_running{false};
        std::atomic<uint32_t> m_nextSeq{1};     // For sequence number generation
        std::thread m_receiverThread;
        std::mutex m_handlerMutex;
        std::mutex m_sendMutex;                 // Serializes txRing writes (SPSC)
        std::mutex m_pendingMutex;
        std::unordered_map<uint32_t, std::shared_ptr<PendingCall>> m_pending;
    };

    // === Threading Model ===
    // Standalone mode (no RunLoop):
    //   - 1 receiver thread
    //   - call() blocks the caller
    //   - Sequence numbers correlate requests to responses
    //
    // RunLoop mode (with RunLoop parameter):
    //   - No dedicated threads
    //   - Handlers called on RunLoop's event thread
    //   - call() still blocks caller (waits on condition variable)
}
```

---

## 8. STRESS TEST FILE

### Location: `pr6_race_stress.cpp` (at repo root)

This file contains three targeted race condition and resource stress tests:

#### Test 1: Client Isolation
```cpp
void test1_client_isolation(int rounds)
```
- Scenario: One client fills its rxRing (never reads responses) while service broadcasts
- Expected outcome: Dying client gets disconnected; healthy clients unaffected
- Key APIs: `sendNotify()`, `connectToServer()`, `writeFrame()`, `sendSignal()`

#### Test 2: Service Recovery
```cpp
void test2_service_recovery(int iterations)
```
- Scenario: Repeated ring-full forced disconnects; verify fresh clients still work
- Expected outcome: No FD/thread leaks; service healthy after repeated disconnects
- Key APIs: `connectToServer()`, `sendNotify()` (to trigger reap), `call()`

#### Test 3: Concurrent Reap
```cpp
void test3_concurrent_reap(int rounds)
```
- Scenario: Hammer `sendNotify()` from multiple threads while stuck client killed
- Expected outcome: No deadlocks (all threads complete within 3 seconds)
- Key APIs: `sendNotify()` (concurrent), `fillRing()` (simultaneous)

#### Raw Connection Stress Pattern
```cpp
static void fillRing(Connection &conn, int maxFrames = 12000)
```
- Fills client's rxRing by sending requests without reading responses
- Server echoes responses into rxRing, causing it to fill
- Once full, server's writeFrame() fails, triggering disconnect

---

## 9. EXAMPLE USAGE PATTERNS

### Simple Echo Service

```cpp
class EchoService : public ms::ipc::ServiceBase {
public:
    using ServiceBase::ServiceBase;

protected:
    int onRequest(uint32_t messageId, 
                  const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override {
        *response = request;  // Echo back
        return ms::ipc::IPC_SUCCESS;
    }
};

// Usage
EchoService service("echo");
if (!service.start()) {
    std::fprintf(stderr, "Failed to start service\n");
    return 1;
}
std::printf("Echo server running...\n");
// ... service accepting connections ...
service.stop();
```

### Simple Echo Client

```cpp
ms::ipc::ClientBase client("echo");
if (!client.connect()) {
    std::fprintf(stderr, "Failed to connect\n");
    return 1;
}

const char *message = "Hello";
std::vector<uint8_t> request(message, message + strlen(message));
std::vector<uint8_t> response;

int rc = client.call(1, 1, request, &response, 2000);
if (rc == ms::ipc::IPC_SUCCESS) {
    std::string reply(response.begin(), response.end());
    std::printf("Received: %s\n", reply.c_str());
} else {
    std::fprintf(stderr, "Call failed: %d\n", rc);
}

client.disconnect();
```

### Broadcasting Notifications

```cpp
class NotifyService : public ms::ipc::ServiceBase {
public:
    using ServiceBase::ServiceBase;
    
    void broadcastUpdate(int version) {
        std::vector<uint8_t> payload;
        payload.push_back(static_cast<uint8_t>(version));
        sendNotify(1, 10, payload.data(), payload.size());
    }

protected:
    int onRequest(uint32_t messageId,
                  const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override {
        return ms::ipc::IPC_SUCCESS;
    }
};
```

### Receiving Notifications

```cpp
class NotifyClient : public ms::ipc::ClientBase {
public:
    using ClientBase::ClientBase;
    std::atomic<int> updateCount{0};

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                       const std::vector<uint8_t> &payload) override {
        updateCount.fetch_add(1, std::memory_order_relaxed);
        if (payload.size() > 0) {
            int version = payload[0];
            std::printf("Received update: version %d\n", version);
        }
    }
};
```

---

## 10. THREAD SAFETY GUARANTEES

### SPSC Invariant (CRITICAL)

```
✓ Only ONE producer can write to txRing
  Enforced by: sendMutex (ServiceBase/ClientBase)
  
✓ Only ONE consumer can read from rxRing
  Enforced by: Single receiver thread per connection

✗ Violating the SPSC invariant causes DATA CORRUPTION
```

### ServiceBase Thread Safety

```
✓ onRequest() called concurrently (one thread per client)
✓ sendNotify() serializes all writes via per-client sendMutex
✓ Dead client reaping is thread-safe (two-phase with lock protection)
✓ Can call sendNotify() from any thread (including onRequest handlers)
```

### ClientBase Thread Safety

```
✓ call() is thread-safe (can call from multiple threads)
✓ Receiver thread matches responses via sequence number
✓ Pending calls list protected by pendingMutex
✓ Send operations protected by sendMutex
✓ Multiple concurrent call() threads are safe
```

### Frame Operations Atomicity

```
✓ writeFrame() is atomic: entire frame writes or nothing writes
✓ readFrame() is atomic: header + payload consumed together
```

---

## 11. QUICK REFERENCE: ERROR CODES

| Code | Value | Meaning |
|------|-------|---------|
| `IPC_SUCCESS` | 0 | Operation succeeded |
| `IPC_ERR_DISCONNECTED` | -1 | Not connected or connection lost |
| `IPC_ERR_TIMEOUT` | -2 | No response within timeoutMs |
| `IPC_ERR_INVALID_SERVICE` | -3 | Service not found |
| `IPC_ERR_INVALID_METHOD` | -4 | Method not found |
| `IPC_ERR_VERSION_MISMATCH` | -5 | Protocol version mismatch |
| `IPC_ERR_RING_FULL` | -6 | Ring buffer full |
| `IPC_ERR_STOPPED` | -7 | Service stopped |
| `IPC_ERR_INVALID_ARGUMENT` | -8 | Invalid argument |

---

## 12. KEY CONSTANTS

```cpp
kProtocolVersion        = 1
kRingSize               = 262,144 bytes (256 KB)
kMaxPayload             = 262,120 bytes
sizeof(FrameHeader)     = 24 bytes
Socket Send Timeout     = 5 seconds
call() Default Timeout  = 2000 ms (2 seconds)
```

---

## 13. FILES & LOCATIONS

| File | Location | Purpose |
|------|----------|---------|
| `Types.h` | `inc/` | Type definitions, constants, error codes |
| `FrameIO.h` | `inc/` | Frame I/O functions (write/read/peek) |
| `Connection.h` | `inc/` | Connection struct, handshake functions |
| `Platform.h` | `inc/` | Platform abstraction (UDS, SHM, FD) |
| `ServiceBase.h` | `inc/` | Service base class |
| `ClientBase.h` | `inc/` | Client base class |
| `RingBuffer.h` | `deps/ms-ringbuffer/inc/spsc/` | Lock-free SPSC ring buffer |
| `Connection.cpp` | `src/` | Connection handshake implementation |
| `ServiceBase.cpp` | `src/` | Service implementation |
| `ClientBase.cpp` | `src/` | Client implementation |
| `FrameIO.cpp` | `src/` | Frame I/O implementation |
| `pr6_race_stress.cpp` | repo root | Stress tests |

---

