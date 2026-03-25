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
