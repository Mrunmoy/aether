# macOS Platform Port

This port keeps the public IPC API and frame protocol unchanged. The work is confined to a new macOS platform backend plus the build, test, and CI wiring needed to exercise it.

## Layer Contract

- Signaling socket: use `AF_UNIX` pathname sockets with `SOCK_STREAM`. macOS does not give us Linux abstract-namespace sockets, so the server binds a deterministic path such as `/tmp/aether_<uid>_<hash>.sock`, unlinks any stale file before bind, and removes the socket path on normal stop.
- Handshake and FD passing: keep the existing version byte, shared-memory FD transfer, and ACK byte flow. `sendmsg`/`recvmsg` with `SCM_RIGHTS` remain the right mechanism. Because the socket is stream-oriented, reads must tolerate coalesced bytes and short reads.
- Shared memory: replace `memfd_create` with `shm_open` + `ftruncate` + `mmap`. Immediately `shm_unlink` the object after successful creation so the lifetime stays anonymous and connection-owned.
- Close and timeout semantics: preserve `shutdown(SHUT_RDWR)` as the disconnect primitive and keep `SO_SNDTIMEO` for send timeouts. On macOS, suppress `SIGPIPE` with `SO_NOSIGPIPE` rather than Linux’s `MSG_NOSIGNAL`.

## Build Selection

Add a dedicated `src/PlatformMac.cpp` and select it from `CMakeLists.txt` with `if(APPLE)`. Keep `src/PlatformLinux.cpp` for Linux and mirror the same split later for Windows. Any helper code that is truly shared should be factored only after it is proven reusable.

## TDD Plan

1. Add macOS-specific platform tests first: socket bind/connect, FD passing, shared-memory creation, stale socket cleanup, and `SIGPIPE`-safe send behavior.
2. Add one end-to-end smoke test for client/server connect, request, response, and notification delivery on macOS CI.
3. Keep the existing Linux tests intact for shared protocol and ring-buffer behavior. Only the low-level platform tests need OS-specific variants.
4. Validate the backend in GitHub Actions on `macos-15` or `macos-latest` from this Ubuntu host; local Linux runs should cover the shared code path, while macOS runtime validation happens in CI.

## Future Ownership

- `inc/Platform.h`: keep the cross-platform contract and remove Linux-specific wording.
- `src/PlatformMac.cpp`: macOS socket, FD passing, shared-memory, and timeout implementation.
- `CMakeLists.txt`: platform-specific source selection.
- `test/PlatformTest.cpp`: platform backend tests and macOS-only regressions.
- `test/ConnectionTest.cpp` and `test/ServiceBaseTest.cpp`: only if the macOS backend exposes new runtime edge cases.
- `.github/workflows/ci.yml`: add macOS job and keep Linux as the shared baseline.
- `doc/aether-lld.md` and `README.md`: document macOS-specific pathname socket behavior, including stale-path recovery after crashes.
