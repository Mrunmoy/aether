# Tests

Unit tests for aether. C++ tests use [Google Test](https://github.com/google/googletest) v1.14.0,
Python tests use pytest.

## Prerequisites

Google Test is a git submodule under `test/vendor/googletest`. Clone with `--recursive` or run:

```bash
git submodule update --init --recursive
```

## Running tests

```bash
# From the project root — runs all C++ and Python tests:
python3 build.py -t

# Or with CMake directly (C++ only):
cmake -B build -DMS_IPC_BUILD_EXAMPLES=ON && cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure

# Python tests only:
python3 -m pytest tools/ipcgen/test/ -v
```

## Test organization

Windows builds use `PlatformWindowsTest.cpp` instead of `PlatformTest.cpp`.

### Core C++ tests (test/) — 50 tests

| File | What it tests |
|------|---------------|
| `PlatformTest.cpp` | Platform layer: UDS socket creation, client connect, accept, signal byte round-trip, FD passing via SCM_RIGHTS with shared memory verification, shared memory create/mmap, edge cases (connect to nonexistent, zero-size shm, close -1). 11 tests. |
| `ConnectionTest.cpp` | Connection handshake: basic handshake (both sides valid), bidirectional ring buffer data flow, protocol version mismatch rejection, multiple independent clients, connection cleanup. 5 tests. |
| `FrameIOTest.cpp` | Frame I/O: write/read round-trip with all header fields, peek without consuming, ring-full detection, empty-ring errors, multiple frames in sequence, zero-payload frames, vector-based read, large (~200KB) payload, undersized buffer error recovery. 9 tests. |
| `ServiceBaseTest.cpp` | ServiceBase: start/stop lifecycle, single echo request/response, invalid method error in aux, multiple requests on same connection, multiple independent clients, notification broadcast to all clients, stop cleans up connections (client detects disconnect), client disconnect doesn't crash service. RunLoop mode: accept and respond, multiple clients, client disconnect handling, stop cleanup. 12 tests. |
| `ClientBaseTest.cpp` | ClientBase: connect/disconnect lifecycle, single echo RPC call, invalid method error, multiple sequential calls, call timeout (IPC_ERR_TIMEOUT), disconnect fails pending calls, notification via virtual onNotification, call after disconnect returns error, server stop disconnects client. RunLoop mode: call and response, notification delivery, clean disconnect, both service and client on same RunLoop. 13 tests. |

### Code generation C++ tests (example/) — 11 tests

| File | What it tests |
|------|---------------|
| `CodeGenServerTest.cpp` | Generated server dispatch: GetDeviceCount, GetDeviceStatus, DeviceConnected/Disconnected notifications, invalid method error, RunLoop mode dispatch. 6 tests. |
| `CodeGenClientTest.cpp` | Generated client RPC: GetDeviceCount, GetDeviceStatus, DeviceConnected/Disconnected notification callbacks, RunLoop typed call. 5 tests. |

### Python tests (tools/ipcgen/test/) — 45 tests

| File | What it tests |
|------|---------------|
| `test_hash.py` | FNV-1a 32-bit hash: empty string, known value, uniqueness. 3 tests. |
| `test_lexer.py` | IDL tokenizer: keywords, identifiers, numbers, symbols, attributes, comments, whitespace, error cases. 12 tests. |
| `test_parser.py` | IDL parser: minimal/full services, param directions, all scalar types, error validation (name mismatch, unknown types, pointer rules). 15 tests. |
| `test_server_emitter.py` | Server code emitter: header structure, service ID, handler/notify signatures, dispatch switch. 6 tests. |
| `test_client_emitter.py` | Client code emitter: header structure, RPC signatures, notification callbacks, dispatch switch. 6 tests. |
| `test_end_to_end.py` | Full pipeline: file generation, content matching, CLI invocation. 3 tests. |
