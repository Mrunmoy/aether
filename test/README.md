# Tests

Unit tests for ms-ipc, using [Google Test](https://github.com/google/googletest) v1.14.0.

## Prerequisites

Google Test is a git submodule under `test/vendor/googletest`. Clone with `--recursive` or run:

```bash
git submodule update --init --recursive
```

## Running tests

```bash
# From the project root:
python3 build.py -t

# Or with CMake directly:
cmake -B build && cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

## Test files

| File | What it tests |
|------|---------------|
| `PlatformTest.cpp` | Platform layer: UDS socket creation, client connect, accept, signal byte round-trip, FD passing via SCM_RIGHTS with shared memory verification, shared memory create/mmap, edge cases (connect to nonexistent, zero-size shm, close -1). 11 tests. |
| `ConnectionTest.cpp` | Connection handshake: basic handshake (both sides valid), bidirectional ring buffer data flow, protocol version mismatch rejection, multiple independent clients, connection cleanup. 5 tests. |
| `FrameIOTest.cpp` | Frame I/O: write/read round-trip with all header fields, peek without consuming, ring-full detection, empty-ring errors, multiple frames in sequence, zero-payload frames, vector-based read, large (~200KB) payload, undersized buffer error recovery. 9 tests. |
| `ServiceBaseTest.cpp` | ServiceBase: start/stop lifecycle, single echo request/response, invalid method error in aux, multiple requests on same connection, multiple independent clients, notification broadcast to all clients, stop cleans up connections (client detects disconnect), client disconnect doesn't crash service. 8 tests. |
| `ClientBaseTest.cpp` | ClientBase: connect/disconnect lifecycle, single echo RPC call, invalid method error, multiple sequential calls, call timeout (IPC_ERR_TIMEOUT), disconnect fails pending calls, notification via virtual onNotification, call after disconnect returns error, server stop disconnects client. 9 tests. |
