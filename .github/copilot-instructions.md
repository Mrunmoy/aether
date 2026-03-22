# Copilot Instructions for ms-ipc

## Architecture Overview

ms-ipc is a shared-memory IPC framework for Linux with an IDL-based code generator. The transport uses lock-free SPSC ring buffers (from `deps/ms-ringbuffer`) for the data plane and a Unix domain socket for the control plane (connection handshake, FD passing, wakeup signals).

**Layers (bottom to top):**
1. **Platform** (`inc/Platform.h`, `src/PlatformLinux.cpp`) — Linux OS abstractions: UDS abstract-namespace sockets, `memfd_create` shared memory, `SCM_RIGHTS` FD passing, 1-byte wakeup signals.
2. **Connection** (`inc/Connection.h`, `src/Connection.cpp`) — Handshake protocol; sets up shared memory and places `IpcRing` objects (from `deps/ms-ringbuffer`) into it. Each connection has two 256 KB rings (tx/rx).
3. **FrameIO** (`inc/FrameIO.h`, `src/FrameIO.cpp`) — Inline frame read/write on ring buffers. Wire frames have a 24-byte `FrameHeader` (version, flags, serviceId, messageId, seq, payloadBytes, aux) followed by a variable-length payload. All values are native endian.
4. **ServiceBase / ClientBase** (`inc/ServiceBase.h`, `inc/ClientBase.h`, `src/`) — Server and client runtime. ServiceBase runs an accept loop and per-client receiver threads (or integrates with `deps/ms-runloop` for event-driven dispatch). ClientBase manages sequence-number correlation for synchronous `call()`.
5. **Generated Code** (`tools/ipcgen/`) — IDL → 5 C++ files: `*Types.h`, `server/*.h`, `server/*.cpp`, `client/*.h`, `client/*.cpp`. Service IDs are FNV-1a 32-bit hashes of the service name.

**Key constraint:** `IpcRing` is SPSC (single-producer/single-consumer). `ServiceBase::ClientConn` uses a `sendMutex` to serialize all server→client ring writes from `receiverLoop` (responses), `sendNotify` (notifications), and `onClientReady` (RunLoop responses). Do not write to a client's `txRing` without holding `sendMutex`.

**Threading model:** By default, ServiceBase spawns one accept thread and one receiver thread per connected client. Passing a `RunLoop*` to the constructor switches to a single event-driven thread (epoll-based) via `deps/ms-runloop`.

## Build, Test, and Development Commands

```bash
# Standard workflow
python3 build.py              # build library → build/
python3 build.py -t           # build + run all C++ tests (ctest) + Python tests (pytest)
python3 build.py -c -t        # clean rebuild + full test pass

# Manual CMake (useful for Debug builds)
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DMS_IPC_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)

# C++ tests — run the full suite
ctest --test-dir build --output-on-failure

# C++ tests — run a single test by name (regex match)
ctest --test-dir build --output-on-failure -R ServiceBaseTest

# Python tests — run the full suite
python3 -m pytest tools/ipcgen/test/ -v

# Python tests — run a single test
python3 -m pytest tools/ipcgen/test/test_parser.py::TestParser::test_minimal_service -v
```

## Naming and Coding Conventions

- **C++ standard:** C++17. All public types in namespace `ms::ipc`; platform primitives in `ms::ipc::platform`.
- **Indentation:** 4 spaces; Allman brace style (opening brace on its own line).
- **Types:** `PascalCase` (classes, structs, enums, test files like `ConnectionTest.cpp`).
- **Functions/methods:** `camelCase` (`connectToServer`, `acceptLoop`).
- **Member variables:** `m_` prefix (`m_serviceName`, `m_clients`, `m_running`).
- **Constants:** `kCamelCase` (`kRingSize`, `kMaxPayload`, `kSocketTimeoutMs`).
- **Headers:** `#pragma once`, no implementation in headers except inline FrameIO helpers.
- **Wire types:** Use fixed-size integers (`uint32_t`, `uint16_t`); POD structs only; no `std::string` in wire format — use bounded `char[N+1]` arrays.
- **Payloads:** `std::vector<uint8_t>` for variable-length data above the wire layer.
- **Error handling:** No exceptions. Return negative `IpcError` codes for framework errors, `0` (`IPC_SUCCESS`) for success, positive integers for application-defined errors.

## IDL Code Generation

The IDL supports: `enum`, `struct`, `service` (methods), and `notifications` blocks.

**Run the generator:**
```bash
python3 -m ipcgen path/to/Foo.idl --outdir path/to/gen
```
This produces: `FooTypes.h`, `server/Foo.h`, `server/Foo.cpp`, `client/Foo.h`, `client/Foo.cpp`.

**Generated server class:** inherits `ServiceBase`, exposes pure-virtual `handleXxx()` methods for the user to implement and concrete `notifyXxx()` methods for broadcasting notifications.

**Generated client class:** inherits `ClientBase`, exposes typed RPC methods (`Foo(args..., timeoutMs)`) and virtual `onXxx()` callbacks for incoming notifications.

The emitter adds server-side payload size guards (rejects undersized requests) and value-initializes all `[out]` params before calling the handler. Client-side notification dispatch guards against truncated payloads.

## Testing Patterns

**C++ tests** (Google Test, `test/*.cpp`):
- Define a local fixture service (e.g., `EchoService : public ServiceBase`) inside the test file.
- Use `SVC_NAME` macro (`::testing::UnitTest::GetInstance()->current_test_info()->name()`) to get a unique per-test service name to avoid socket collisions.
- Use `settle()` (a short sleep) to let async threads complete before asserting.
- Test files are `PascalCase`: `ServiceBaseTest.cpp`, `ClientBaseTest.cpp`.

**Python tests** (pytest, `tools/ipcgen/test/test_*.py`):
- Use the `parse(idl_text)` fixture from `conftest.py` for unit tests; inspect AST fields directly.
- End-to-end tests generate all 5 files and assert on file content.
- Test error cases by passing invalid IDL and checking exception messages.
- Name files `test_*.py` and keep each test focused on one behavior.

## Commit Guidelines

Short, imperative commit subjects focused on observable behavior:
- ✅ `Return first error from sendNotify, not last`
- ✅ `Harden IPC layer against dead peers`
- ❌ `Fix bug` / `Refactor code`

PR descriptions should state the problem, summarize the fix, note any wire-format or codegen impact, and list the validation commands run.
