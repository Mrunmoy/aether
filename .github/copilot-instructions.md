# Copilot Instructions for Aether

## Build and test commands

The repo's standard path is the root `build.py` helper. Repo docs usually show `python3`; on Windows use `py -3` if `python3` is not on `PATH`.

```bash
python build.py              # build the core library into build/
python build.py -e           # build the library plus examples
python build.py -t           # build examples, run all C++ tests, then run ipcgen pytest
python build.py -c -t        # clean rebuild + full test pass
python build.py -p           # build examples and package the SDK tarball
```

Direct CMake is useful when you need a specific config:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_EXAMPLES=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Run one test instead of the full suite:

```bash
ctest --test-dir build --output-on-failure -R ServiceBaseTest.SingleRequestResponse
ctest --test-dir build --output-on-failure -R CodeGenClientTest.GetDeviceCount
python -m pytest tools/ipcgen/test/test_parser.py::TestParser::test_minimal_service -v
```

There is no standalone linter target. The CMake build enforces warnings-as-errors (`-Wall -Wextra -Wpedantic -Werror` on GCC/Clang, `/W4 /WX` on MSVC). Optional CI-style debug configs:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAETHER_SANITIZERS=address,undefined
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAETHER_SANITIZERS=thread
```

Code generation commands that show up throughout the docs:

```bash
python -m tools.ipcgen examples/getting-started/echo/DeviceMonitor.idl --outdir examples/getting-started/echo/gen
python -m tools.ipcgen path/to/Foo.idl --outdir gen --backend c_api
python -m tools.ipcgen path/to/Foo.idl --outdir gen --backend python
python -m tools.ipcgen path/to/Foo.idl --outdir gen --backend aether_lite
```

## High-level architecture

Aether is a cross-platform IPC framework that uses shared memory for the hot path and IDL code generation for the user-facing API. The runtime stack is:

1. **Platform** (`Platform.h`, `PlatformLinux.cpp`, `PlatformMac.cpp`, `PlatformWindows.cpp`) wraps local sockets, shared memory, handle passing, signaling, and peer-credential checks.
2. **Connection** (`Connection.h/.cpp`) performs the client/server handshake and maps a shared-memory region with two `IpcRing` instances: client->server and server->client.
3. **FrameIO** (`FrameIO.h/.cpp`) reads and writes a 24-byte `FrameHeader` plus payload. The same frame format is reused across shared-memory IPC and stream transports.
4. **ServiceBase / ClientBase** implement the default threaded runtime and the optional `RunLoop`-driven mode for deterministic single-thread dispatch.
5. **Generated code** from `tools/ipcgen/` sits above the runtime and is the normal integration path: IDL defines the contract, generated code handles marshalling/dispatch, user code fills in handlers and callbacks.

The runtime has a split control/data plane. Payload bytes move through shared-memory SPSC rings; the control channel only carries handshake metadata and wakeup signals. For non-shared-memory links, `ITransport` plus `TransportClientBase` reuse the exact same frame protocol over serial/USB-style byte streams.

There are four codegen targets with different integration stories:

- **C++ backend**: typed client/server glue for source builds, built on `ServiceBase` and `ClientBase`
- **c_api backend**: SDK-oriented wrappers around the stable C API in `inc/aether_ipc.h`
- **python backend**: typed Python client stubs layered over the C API
- **aether_lite backend**: C99 dispatch tables for bare-metal MCU firmware

`inc/aether_ipc.h` is the only installed public SDK header. The richer C++ headers in `inc/` are for source-build/runtime work inside this repo.

## Key conventions and repo-specific patterns

- The canonical learning and debugging path is `examples/getting-started/echo/`. It is the repo's default "first success" flow and the example most docs assume.
- Generated example output is checked into the repository. Only rerun `tools.ipcgen` after changing an `.idl` file, and regenerate the whole output set together rather than editing generated files by hand.
- `IpcRing` is **single-producer/single-consumer**. Any code that writes server->client frames must serialize access to a connection's `txRing` with `ServiceBase::ClientConn::sendMutex`. Client-side shared-memory sends are similarly serialized with `ClientBase::m_sendMutex`.
- Aether treats disconnects and timeouts as part of the normal contract. Do not add exception-based flows; framework errors are negative codes, success is `0`, and application-defined handler errors may be positive.
- Default mode is threaded: one accept thread plus one receiver thread per client. Passing `ms::RunLoop*` switches service/client objects into single-thread event-driven mode. Do not issue synchronous `call()` from the same RunLoop thread that must deliver the response.
- The shared-memory transport and the stream transport share the same wire format. If a change touches `FrameHeader`, framing, or error semantics, inspect both the shared-memory path (`ClientBase`/`ServiceBase`) and the transport path (`ITransport`, `TransportClientBase`).
- Core C++ tests define small local services or stubs inside the test file. Use the `SVC_NAME` macro to get a unique per-test service name and `settle()` to give async threads time to drain before asserting.
- `test/vendor/googletest` is a required submodule for C++ tests. Clone with `--recursive` or run `git submodule sync --recursive && git submodule update --init --recursive` before assuming a broken test configuration.
- Style is consistent across the repo: C++17, Allman braces, `PascalCase` types, `camelCase` functions, `m_` members, `kCamelCase` constants, `#pragma once`, fixed-size wire types, and no `std::string` in wire payload structs.
