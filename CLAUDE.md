# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and Test Commands

```bash
python3 build.py              # Build library into build/
python3 build.py -t           # Build + run all C++ and Python tests
python3 build.py -c -t        # Clean rebuild + full test pass
python3 build.py -e           # Build + examples
python3 build.py -p           # Build + SDK packaging

# Manual CMake
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DMS_IPC_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)

# C++ tests (Google Test via CTest)
ctest --test-dir build --output-on-failure              # Full suite
ctest --test-dir build --output-on-failure -R ServiceBaseTest  # Single test by name

# Python tests (ipcgen)
python3 -m pytest tools/ipcgen/test/ -v                 # Full suite
python3 -m pytest tools/ipcgen/test/test_parser.py::TestParser::test_minimal_service -v  # Single test

# Code generation
python3 -m ipcgen path/to/Foo.idl --outdir gen/                    # C++ (default)
python3 -m ipcgen path/to/Foo.idl --outdir gen/ --backend c_api    # C API wrapper
python3 -m ipcgen path/to/Foo.idl --outdir gen/ --backend python   # Python client
python3 -m ipcgen path/to/Foo.idl --outdir gen/ --backend aether_lite  # C99 MCU dispatch

# Sanitizers (CI uses these)
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAETHER_SANITIZERS=address,undefined
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAETHER_SANITIZERS=thread
```

## Architecture

Aether is a shared-memory IPC framework with an IDL code generator. The transport splits into a **data plane** (lock-free SPSC ring buffers from `deps/ouroboros`) and a **control plane** (Unix domain sockets for handshake, FD passing, wakeup signals).

**Runtime layers (bottom to top):**
1. **Platform** (`Platform.h` / `PlatformLinux.cpp`, `PlatformMac.cpp`, `PlatformWindows.cpp`) -- OS abstractions: sockets, shared memory, FD passing, peer credentials.
2. **Connection** (`Connection.h/.cpp`) -- Handshake protocol; sets up shared memory with two 256KB SPSC rings per connection (tx/rx).
3. **FrameIO** (`FrameIO.h/.cpp`) -- 24-byte `FrameHeader` (version, flags, serviceId, messageId, seq, payloadBytes, aux) + variable payload. Native endian wire format.
4. **ServiceBase / ClientBase** (`ServiceBase.h/.cpp`, `ClientBase.h/.cpp`) -- Server accept loop + per-client receiver threads (or RunLoop from `deps/vortex` for single-threaded dispatch). ClientBase does sequence-number-correlated synchronous `call()`.
5. **Generated code** -- IDL compiler (`tools/ipcgen/`) produces typed server skeletons and client stubs.

**Critical constraint:** `IpcRing` is SPSC. `ServiceBase::ClientConn` uses a `sendMutex` to serialize all server->client ring writes. Never write to a client's `txRing` without holding `sendMutex`.

**Threading model:** Default is one accept thread + one receiver thread per client. Passing `RunLoop*` to constructor switches to single event-driven thread (epoll/kqueue).

**C API** (`aether_ipc.h` / `CApi.cpp`) -- Stable C interface with opaque handles. This is the only installed public header.

**Transport abstraction** (`ITransport.h`, `TransportClientBase.h/.cpp`) -- For non-SHM transports (serial, USB). Same wire protocol, no shared memory.

## Code Generation (ipcgen)

IDL supports: `enum`, `struct`, `service` (methods with `[in]`/`[out]` params), `notifications` blocks. Types: uint8-64, int8-64, float32/64, bool, T[N] arrays, string[N].

Default C++ backend produces 5 files: `FooTypes.h`, `server/Foo.h`, `server/Foo.cpp`, `client/Foo.h`, `client/Foo.cpp`. Service IDs are FNV-1a 32-bit hashes of the service name.

## Coding Conventions

- **C++17**, namespace `aether::ipc` (public), `aether::ipc::platform` (internals)
- 4-space indentation, Allman braces (opening brace on own line)
- `PascalCase` types, `camelCase` functions, `m_` member prefix, `kCamelCase` constants
- `#pragma once`, no implementation in headers except inline FrameIO helpers
- Wire types: fixed-size integers, POD structs only, no `std::string` -- use `char[N+1]`
- Payloads above wire layer: `std::vector<uint8_t>`
- No exceptions. Return negative `IpcError` for framework errors, `0` for success, positive for app errors
- Compiler warnings: `-Wall -Wextra -Wpedantic -Werror` (GCC/Clang), `/W4 /WX` (MSVC)

## Testing Patterns

**C++ tests** (`test/*.cpp`): Use `SVC_NAME` macro for unique per-test service names (avoids socket collisions). Use `settle()` helper for async thread synchronization. Define local fixture services inside test files.

**Python tests** (`tools/ipcgen/test/test_*.py`): Use `parse(idl_text)` fixture from `conftest.py`. End-to-end tests generate all files and assert on content. Name files `test_*.py`, one behavior per test.

**Codegen tests** (`examples/getting-started/echo/Codegen*Test.cpp`): Verify generated code actually works against the runtime.

## Commit Style

Short, imperative subjects focused on observable behavior. PR descriptions: state the problem, summarize the fix, note wire-format/codegen impact, list validation commands. Run `python3 build.py -t` before opening a PR.
