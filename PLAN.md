# ms-ipc — Phase 1 Implementation Plan

## Goal

Set up the `ms-ipc` project and implement the platform abstraction layer
with unit tests. Linux backend first.

---

## Step 1: Project Scaffolding

Create the directory structure matching `ms-runloop` and `ms-ringbuffer` conventions:

```
ms-ipc/
├── inc/
│   └── Platform.h          # OS-agnostic platform API
├── src/
│   └── PlatformLinux.cpp   # Linux implementation
├── test/
│   ├── PlatformTest.cpp    # Unit tests
│   ├── CMakeLists.txt      # Test target
│   └── vendor/googletest/  # Git submodule
├── CMakeLists.txt           # Root CMake
├── build.py                 # Build script
├── .gitignore               # Same as other ms-* repos
├── .gitmodules              # googletest submodule
└── LICENSE                  # Already exists
```

### CMakeLists.txt (root)

- `project(ms-ipc LANGUAGES CXX DESCRIPTION "...")`
- Library target: `add_library(ms-ipc src/PlatformLinux.cpp)`
- Public include: `inc/` via `BUILD_INTERFACE` / `INSTALL_INTERFACE`
- C++17 via `target_compile_features`
- Link: `pthread` (private)
- Conditional test subdirectory (only when standalone, same pattern as other repos)

### test/CMakeLists.txt

- Same submodule check pattern as ms-runloop/ms-ringbuffer
- Target: `ipc_tests`
- Links: `ms-ipc`, `GTest::gtest_main`, `pthread`

### build.py

- Same structure as ms-ringbuffer (clean, configure, build, test)
- Simpler initially — no benchmarks, just `-c`, `-t` flags

### .gitignore

- Identical to ms-ringbuffer's

### Submodules

- `test/vendor/googletest` → `https://github.com/google/googletest.git` (v1.14.0)
- `ms-runloop` and `ms-ringbuffer` are NOT submodules yet — they're not needed
  for the platform layer. We'll add them when we build the service layer.

---

## Step 2: Platform.h — Public API

Namespace: `ms::ipc::platform`

Embedded-friendly: no `std::string`, no heap allocations, `const char*` and
fixed-size buffers only.

```cpp
#pragma once

#include <cstdint>

namespace ms::ipc::platform {

// ── Unix Domain Sockets ─────────────────────────────────────────

// Create a listening UDS socket bound to abstract namespace.
// Linux: SOCK_SEQPACKET, path = \0ipc_<name>
// Returns fd >= 0 on success, -1 on failure.
int serverSocket(const char* name);

// Connect to a UDS server in abstract namespace.
// Returns fd >= 0 on success, -1 on failure.
int clientSocket(const char* name);

// Accept a connection on a listening socket.
// Returns fd >= 0 on success, -1 on failure.
int acceptClient(int listenFd);

// Send a file descriptor + ancillary data over UDS (SCM_RIGHTS).
// Returns 0 on success, -1 on failure.
int sendFd(int sockFd, int fdToSend, const void* data, uint32_t dataLen);

// Receive a file descriptor + ancillary data over UDS (SCM_RIGHTS).
// Sets *receivedFd on success.
// Returns bytes received (> 0) on success, -1 on failure.
int recvFd(int sockFd, int* receivedFd, void* data, uint32_t dataLen);

// Send a single wakeup byte.
// Returns 0 on success, -1 on failure.
int sendSignal(int sockFd);

// Receive a single wakeup byte (blocks until available).
// Returns 0 on success, -1 on failure.
int recvSignal(int sockFd);

// ── Shared Memory ───────────────────────────────────────────────

// Create anonymous shared memory region of `size` bytes.
// Linux: memfd_create + ftruncate.
// Returns fd >= 0 on success, -1 on failure.
int shmCreate(uint32_t size);

// ── File Descriptor ─────────────────────────────────────────────

// Close a file descriptor. Safe to call with -1 (no-op).
void closeFd(int fd);

} // namespace ms::ipc::platform
```

**Design notes:**
- `serverSocket` / `clientSocket` — named for what they create, not the verb
- `sendFd` / `recvFd` — generic FD passing with arbitrary ancillary data
  (the handshake layer decides what data to send, e.g. protocol version)
- `sendSignal` / `recvSignal` — lightweight wakeup (1 byte), no payload
- `shmCreate` — anonymous only, no name needed (Linux memfd is anonymous)
- `closeFd` — convenience, handles -1 gracefully

---

## Step 3: PlatformLinux.cpp — Implementation

Implements all functions from Platform.h using Linux syscalls:

- `serverSocket`: `socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0)` +
  `bind` to abstract namespace `\0ipc_<name>` + `listen(fd, 16)`
- `clientSocket`: `socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0)` +
  `connect` to abstract namespace `\0ipc_<name>`
- `acceptClient`: `accept4(listenFd, nullptr, nullptr, SOCK_CLOEXEC)`
- `sendFd`: `sendmsg` with `SCM_RIGHTS` control message
- `recvFd`: `recvmsg` with `SCM_RIGHTS` control message
- `sendSignal`: `send(sockFd, &byte, 1, 0)`
- `recvSignal`: `recv(sockFd, &byte, 1, 0)`
- `shmCreate`: `memfd_create("ipc_shm", MFD_CLOEXEC)` + `ftruncate`
- `closeFd`: `if (fd >= 0) close(fd)`

**Socket path buffer**: Fixed `char[108]` (sizeof `sun_path`), no allocation.

---

## Step 4: Unit Tests (PlatformTest.cpp)

Each test is self-contained — creates resources, tests them, cleans up.

### UDS Tests:
1. **ServerSocketCreation** — `serverSocket("test1")` returns fd >= 0
2. **ClientConnects** — server + client sockets, verify both valid
3. **AcceptClient** — server accepts, returns valid fd
4. **SendRecvSignal** — round-trip a signal byte through connected pair
5. **SendRecvFd** — send a memfd over UDS, verify received fd is valid
   and maps to the same memory contents
6. **ConnectToNonexistent** — `clientSocket("doesnotexist")` returns -1

### Shared Memory Tests:
7. **ShmCreate** — create shared memory, verify fd valid
8. **ShmWriteAndMmap** — create shm, mmap it, write data, verify readable
9. **ShmZeroSize** — edge case, `shmCreate(0)` behavior

### Close Tests:
10. **CloseFdNegativeOne** — `closeFd(-1)` doesn't crash

---

## What we are NOT doing yet

- No `Types.h` / `FrameHeader` — that's Phase 2
- No `ms-runloop` / `ms-ringbuffer` integration — not needed for platform layer
- No Service / Client classes — much later
- No macOS backend — Linux first
- No examples — nothing useful to show until we have the service layer
- No benchmarks — platform ops are OS-bound, benchmarking them isn't useful

---

## Build & Verify

After implementation:
```bash
cd /home/litu/sandbox/cpp/ms-ipc
git submodule update --init --recursive
python3 build.py -t
```

All 10 tests should pass.
