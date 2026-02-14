# Examples

Usage examples for ms-ipc.

## Building

```bash
# From the project root:
python3 build.py -e

# Or with CMake directly:
cmake -B build -DMS_IPC_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

## Running

Start the server in one terminal, then run the client in another:

```bash
# Terminal 1:
./build/example/echo_server

# Terminal 2:
./build/example/echo_client
```

---

## echo_server

**File:** `echo_server.cpp`

A minimal IPC service that echoes back any request payload. Demonstrates
ServiceBase subclassing, request handling, and clean shutdown via Ctrl-C.

### What it does

```
Echo server running (Ctrl-C to stop)...
[server] request: messageId=1, 5 bytes
[server] request: messageId=1, 5 bytes
[server] request: messageId=1, 13 bytes
^CServer stopped.
```

### How it works

**1. Subclass ServiceBase and override `onRequest()`**

```cpp
class EchoService : public ms::ipc::ServiceBase
{
protected:
    int onRequest(uint32_t messageId, const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override
    {
        *response = request;  // echo back
        return IPC_SUCCESS;
    }
};
```

This is all you need to implement a service. The base class handles listening
for connections, accepting clients, receiving requests, and sending responses.

**2. Start, wait, stop**

```cpp
EchoService service("echo");
service.start();
// ... wait for Ctrl-C ...
service.stop();
```

`start()` creates a UDS listening socket (abstract namespace `\0ipc_echo`)
and spawns an accept thread. Each connecting client gets its own receiver
thread. `stop()` shuts everything down cleanly.

---

## echo_client

**File:** `echo_client.cpp`

A minimal client that connects to the echo service, sends three messages,
and prints the responses. Demonstrates ClientBase connection, synchronous
RPC calls, and error handling.

### What it does

```
Connected to echo service.
[client] sent: "Hello"  ->  received: "Hello"
[client] sent: "World"  ->  received: "World"
[client] sent: "ms-ipc works!"  ->  received: "ms-ipc works!"
Disconnected.
```

### How it works

**1. Connect to a named service**

```cpp
ms::ipc::ClientBase client("echo");
client.connect();
```

`connect()` performs the full handshake: creates shared memory, sends the
file descriptor to the server via UDS, and waits for acknowledgment. On
success, it spawns a receiver thread for responses and notifications.

**2. Make synchronous RPC calls**

```cpp
std::vector<uint8_t> request(msg, msg + strlen(msg));
std::vector<uint8_t> response;
int rc = client.call(1, 1, request, &response);
```

`call()` writes the request to the shared memory ring buffer, signals the
server, and blocks until the response arrives (or timeout). The response
is correlated to the request via an internal sequence number.

**3. Disconnect**

```cpp
client.disconnect();
```

Shuts down the receiver thread, fails any pending calls, and closes the
connection. Also called automatically by the destructor.

---

## When to use this pattern

This pattern — ServiceBase subclass on the server, ClientBase on the client —
is the low-level API. You work directly with `std::vector<uint8_t>` payloads
and numeric message IDs.

For production use, generated code (future) will provide typed methods and
notification callbacks, so you won't need to handle raw bytes or message IDs
directly.
