# Windows Platform Port

## Scope
This port keeps the core request/response framing intact and swaps only the platform seam in `inc/Platform.h`. The backend lives in `src/PlatformWindows.cpp`, selected in CMake with `WIN32`.

## Architecture
Use a named pipe per connection for handshake traffic and wakeup bytes. That preserves the current `sendSignal()` / `recvSignal()` shape without inventing a second control transport. Do not emulate Unix FD passing in v1. The client creates a named file mapping, sends the mapping name in `SharedMemoryHandshake`, and the server opens it with `OpenFileMappingA`.

This keeps the transport same-machine only and replaces Linux `memfd` plus `SCM_RIGHTS` with a Windows-native named-object model.

Stop and disconnect behavior is explicit. `shutdownConnection()` cancels blocked accept or recv paths so service and client threads unwind promptly. `call()` timeout semantics stay unchanged, and the platform seam now enforces the configured timeout by bounding each overlapped pipe read or write wait.

The current Windows scope is intentionally threaded-mode only. `RunLoop` integration is rejected when a `RunLoop*` is supplied, because named-pipe readability is not wired into the existing `vortex` waitable-handle model.

## TDD Order
1. `PlatformWindowsTest.cpp` covers pipe connect, wakeup round-trip, shared-memory mapping, timeout enforcement, dummy-byte handshake behavior for zero-length payloads, and shutdown unblock behavior.
2. `ConnectionTest.cpp` stays platform-neutral and validates the common handshake and ring behavior.
3. `ServiceBaseTest.cpp`, `ClientBaseTest.cpp`, and generated-code tests explicitly assert that Windows rejects `RunLoop` mode.
4. Native validation runs in GitHub Actions on `windows-latest`.

## CI And Developer Flow
From a Linux workstation, the practical path is GitHub-hosted `windows-latest` runners plus PR validation. A local Windows PC is optional and mainly useful for interactive debugging.

## Follow-Up
The next Windows milestone is a real `RunLoop` story. That likely needs a new waitable readiness abstraction at the platform seam rather than more special-casing in `ClientBase` and `ServiceBase`.
