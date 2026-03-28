# Aether IPC — Engineering Backlog

Local tracking doc. Do NOT commit.

---

## P0 — Critical / Safety (all done)

| # | Issue | Status |
|---|-------|--------|
| 1 | Client SPSC ring corruption on concurrent `call()` | **DONE** — PR #4 |
| 2 | SIGPIPE crash on dead client | **DONE** — PR #4 |
| 3 | Dead client not marked on sendSignal failure | **DONE** — PR #4 |
| 4 | Response path fire-and-forget | **DONE** — PR #6 |

## P1 — High (all done)

| # | Issue | Status |
|---|-------|--------|
| 5 | Unbounded client connections (DoS) | **DONE** — PR #24 `setMaxClients()` |
| 6 | Sequence number wraparound collision | **DONE** — PR #23 `nextUniqueSeq()` in ClientBase + TransportClientBase |
| 7 | Ring-full client never reaped | **DONE** — PR #7 marks dead on ring-full |
| 8 | `SO_PEERCRED` verification | **DONE** — PR #25 `getPeerUid()` + `setAllowedPeerUid()` |

## P2 — Medium (all done)

| # | Issue | Status |
|---|-------|--------|
| 9 | `m_clientsMutex` held during per-client I/O | **DONE** — PR #27 snapshot pattern with `shared_ptr` + `try_lock` reap |
| 10 | Dead clients linger in RunLoop mode | **DONE** — PR #30 reap on accept (test coverage for existing behavior) |
| 11 | Notification gap detection | **DONE** — PR #29 `m_notifySeq` in `FRAME_NOTIFY` aux field |
| 14 | Listen backlog hardcoded to 16 | **DONE** — PR #28 `SOMAXCONN` |
| 15 | Long service name silently truncated | **DONE** — PR #28 `buildAddr()` returns error on overflow |

## P3 — Low / Nice-to-Have (all done)

| # | Issue | Location | Status |
|---|-------|----------|--------|
| 16 | No destructor for placement-new'd IpcRing | `Connection.cpp` | **DONE** — PR #31 `static_assert(is_trivially_destructible)` |
| 17 | Unify SerialTransport across examples | `examples/` | **DONE** — PR #32 shared `examples/SerialTransport.h` |
| 18 | No handshake in serial examples | `examples/` | **DONE** — PR #33 handshake protocol + negotiated maxPayload |

## Phase 5 — RunLoop / Windows Integration

### P0 — Critical (all done)

| # | Issue | Status |
|---|-------|--------|
| B1 | WFMO slot budget overflow (sources+timers > 63) | **DONE** — Vortex PR #9, Aether PR #48 |
| B6 | CreatePipe "waitable" claim (pipe-based tests fail on Win32) | **DONE** — Vortex PR #9 (GTEST_SKIP) |
| B7 | find_package(Threads) guard breaks MSVC linking | **FALSE POSITIVE** — no change needed |

### P1 — Hardening (all done)

| # | Issue | Status |
|---|-------|--------|
| D1 | Handshake blocks RunLoop thread for 5s | **DONE** — Aether PR #50 (`timeoutMs` param, 500ms for RunLoop) |
| D4 | onError dead code on WFMO backend (undocumented) | **DONE** — Vortex PR #10 (docs in RunLoop.h + backend_win32.cpp) |
| D7 | ERROR_PIPE_CONNECTED race undocumented | **DONE** — Aether PR #50 (clarifying comment) |
| D9 | 4 tests trapped in `#if !defined(_WIN32)` guard | **DONE** — Aether PR #50 (moved outside guard) |
| D10 | WFMO starvation of high-index sources | **DONE** — Vortex PR #10 (round-robin sweep mitigation) |

### Deferred to IOCP rewrite (Phase 5c)

| # | Issue | Blocked on |
|---|-------|-----------|
| D2 | teardown handlerMutex race | IOCP rewrite |
| D3/D6 | m_listenFd token vs handle | IOCP rewrite |
| D5 | GQCSEx batch dequeue | IOCP rewrite |
| D8 | context map locking | IOCP rewrite |
| B2 | Retired context early-free (use-after-free) | IOCP rewrite |
| B3 | CancelIoEx failure leaks (context leaks) | IOCP rewrite |
| B4 | Missing ResetEvent (100% CPU busy loop) | Overlapped I/O rewrite |
| B5 | Re-arm before dispatch (contradicts removeSource safety) | IOCP rewrite |

## Done (reference)

- PR #4: Robustness hardening (MSG_NOSIGNAL, send mutex, socket timeouts, move semantics, version checks, CI)
- PR #5: Client lifecycle hardening + ipcgen validation
- PR #6: Response path hardening + generated string guards
- PR #7: Non-blocking sendSignal + ring-full dead marking
- PR #8: Rename to Aether + C API + SDK packaging
- PR #9: ipcgen C API backend
- PR #10: macOS platform backend
- PR #12: Windows platform backend
- PR #14: ITransport interface + Endian.h
- PR #15: TransportClientBase
- PR #16: aether-lite C99 embedded runtime
- PR #17: Serial loopback example
- PR #18: MCU firmware template
- PR #19: Serial sensor example
- PR #20: Error code alignment
- PR #21: Remove stale planning docs
- PR #23: Seq wraparound protection
- PR #24: maxClients connection limit
- PR #26: EINTR retry loops on serial example shutdown pipe writes
- PR #27: sendNotify snapshot pattern (shared_ptr, try_lock reap, deadlock-free)
- PR #28: Platform limits (reject long names, SOMAXCONN)
- PR #29: Notification sequence numbers (m_notifySeq in aux field)
- PR #30: RunLoop dead client reaping test coverage
- PR #31: IpcRing trivially-destructible static_assert
- PR #32: Unify SerialTransport across examples
- PR #33: Serial transport handshake protocol
- PR #34: Vortex upgrade + error/hangup callbacks
- PR #48: Vortex submodule update (P0 WFMO fix)
- PR #49: Design docs + dev journal (Phase 5 P0)
- PR #50: Handshake timeout + pipe docs + trapped tests (Phase 5 P1)
- PR #51: Vortex submodule update (P1 starvation sweep)
- PR #52: Phase 5 P1 development journal
