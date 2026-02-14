# Platform Layer Walkthrough

The platform layer (`ms::ipc::platform`) wraps OS-specific IPC primitives
behind a clean C++ interface. Linux is implemented; macOS is planned.

## Why a platform layer?

The IPC framework needs three OS primitives:

1. **Unix Domain Sockets** — for connection lifecycle and signaling
2. **Shared memory** — for zero-copy data transfer
3. **FD passing** — to share the memory region between processes

Each of these has different APIs on Linux vs macOS. The platform layer
hides those differences behind a single header.

## Files

| File | Purpose |
|------|---------|
| `inc/Platform.h` | OS-agnostic declarations |
| `src/PlatformLinux.cpp` | Linux implementation |

## API overview

### Unix Domain Sockets

```cpp
int serverSocket(const char* name);   // create + bind + listen
int clientSocket(const char* name);   // create + connect
int acceptClient(int listenFd);       // accept one client
```

Sockets use `SOCK_SEQPACKET` on Linux (message boundaries preserved)
and bind to the **abstract namespace** (`\0ipc_<name>`). Abstract
namespace sockets are automatically cleaned up when the process exits —
no stale socket files on disk.

### FD passing (SCM_RIGHTS)

```cpp
int sendFd(int sockFd, int fdToSend, const void* data, uint32_t dataLen);
int recvFd(int sockFd, int* receivedFd, void* data, uint32_t dataLen);
```

These use `sendmsg` / `recvmsg` with `SCM_RIGHTS` control messages to
pass a file descriptor from one process to another. The `data` / `dataLen`
parameters carry ancillary data alongside the FD (e.g., protocol version
during the handshake).

This is how the client sends its shared memory FD to the server.

### Signal byte

```cpp
int sendSignal(int sockFd);   // send 1 byte
int recvSignal(int sockFd);   // receive 1 byte (blocks)
```

Lightweight wakeup mechanism. Used for:
- ACK during the handshake
- "Message ready" notifications after writing to the ring buffer

### Shared memory

```cpp
int shmCreate(uint32_t size);
```

Creates an anonymous shared memory region using `memfd_create` + `ftruncate`.
Returns a file descriptor that can be `mmap`'d and passed to another process
via `sendFd`.

### Cleanup

```cpp
void closeFd(int fd);   // safe close, -1 is a no-op
```

## Design decisions

**No `std::string`** — all APIs use `const char*` and fixed-size buffers.
The socket path is built into a stack-allocated `sockaddr_un` (108 bytes).
No heap allocations anywhere in the platform layer.

**No epoll helpers** — epoll is managed by `ms-runloop`, not the platform
layer. Keeping them separate avoids overlap.

**`SOCK_CLOEXEC` everywhere** — all file descriptors are created with
`SOCK_CLOEXEC` or `MFD_CLOEXEC` to prevent leaking into child processes.

## macOS differences (planned)

| Primitive | Linux | macOS |
|-----------|-------|-------|
| Socket type | `SOCK_SEQPACKET` | `SOCK_STREAM` |
| Socket namespace | abstract (`\0ipc_...`) | filesystem path |
| Shared memory | `memfd_create` | `shm_open` + `shm_unlink` |
| FD passing | `SCM_RIGHTS` | `SCM_RIGHTS` |
| Accept | `accept4` | `accept` + `fcntl` |

The platform header stays the same; only the `.cpp` implementation changes.
