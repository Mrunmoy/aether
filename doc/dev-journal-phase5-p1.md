# Phase 5 — P1 Hardening Development Journal

**Date:** 2026-03-28
**Scope:** Address design gaps found during expert review (D1–D10 hardening items)
**Status:** ✅ Complete — 5 of 10 items shipped, 5 deferred to IOCP rewrite
**Predecessor:** [Phase 5 P0 — Critical bug fixes](dev-journal-phase5-p0.md)

---

## 1. Overview

Phase 5 P0 fixed the critical WFMO slot overflow (B1) and pipe test skips (B6).
P1 addresses lower-severity design gaps (D1–D10) identified during the same
expert cross-review.  Five items were completed; the remaining five are blocked
on the IOCP rewrite (F0) and would affect code that will be replaced entirely.

**Key outcomes:**
- 5 deliverables shipped (D1, D4, D7, D9, D10)
- 5 deferred to IOCP rewrite (D2, D3, D5, D6, D8)
- 3 PRs merged (Vortex PR #10, Aether PR #50, Aether PR #51)
- 2 rounds of code review across 5 reviewers, all findings addressed
- 0 new test failures; all existing suites green

**Commits:**

| Repo | Commit | Description |
|------|--------|-------------|
| Vortex | `2a790e4` | Document onError limitation, mitigate WFMO starvation (pre-review) |
| Vortex | `513b530` | Document onError limitation, mitigate WFMO starvation (#10) — squash merge |
| Aether | `8c46385` | Harden handshake timeout, document pipe race, untrap Win32 tests (#50) |
| Aether | `c459dea` | Update Vortex submodule to include PR #10 (#51) |

---

## 2. Design Decisions

| # | Decision | Options Considered | Choice | Rationale |
|---|----------|--------------------|--------|-----------|
| 1 | **Handshake timeout value for RunLoop path** | (a) 200 ms (b) 500 ms (c) Keep default 5000 ms | **(b) 500 ms** | 200 ms was too aggressive — CI runners under load occasionally exceed it. 5000 ms blocks the RunLoop dispatch thread unacceptably on slow/malicious peers. 500 ms is a compromise: tight enough to limit event-loop stalls, loose enough for real-world handshake latency on loaded CI. |
| 2 | **Timeout parameter default** | (a) No default (force callers to choose) (b) Named constant default in header | **(b) `kDefaultHandshakeTimeoutMs = 5000`** | Backward compatible — existing call sites don't need changes. Named constant replaces the magic `5000` that was previously a `static constexpr` in Connection.cpp. `inline constexpr` in the header makes the default self-documenting. |
| 3 | **WFMO starvation fix approach** | (a) Randomize start index per iteration (b) Round-robin sweep with zero-timeout polls (c) Defer entirely to IOCP | **(b) Sweep** | Randomization changes dispatch ordering semantics, which could break handler ordering expectations. The sweep is simple, has zero cost when only one handle is signaled (the common case), and doesn't change the primary dispatch path. Documented as mitigation — true fairness requires IOCP. |
| 4 | **Sweep dispatch code structure** | (a) Duplicate dispatch logic in sweep loop (b) Extract `dispatchHandle` lambda | **(b) Lambda** | Eliminates ~30 lines of duplication between primary dispatch and sweep loop. Lambda captures `[this]` (not `[&]` — see review findings). |
| 5 | **onError dead code strategy** | (a) Remove onError storage from WFMO backend (b) Keep stored, document limitation | **(b) Document** | Removing storage would change the API surface and break the addSource(handle, handler, onError) signature. Keeping it stored means the IOCP rewrite can wire it up without API changes. |
| 6 | **Deferred D2/D3/D5/D6/D8** | (a) Fix now in WFMO backend (b) Defer to IOCP rewrite | **(b) Defer** | All five affect code that will be replaced entirely by the IOCP backend. Fixing teardown races (D2), token-vs-handle semantics (D3/D6), batch dequeue (D5), and context map locking (D8) in throw-away code adds risk with no long-term value. |

---

## 3. Implementation Timeline

| Time | What Was Done | Tests | Commit |
|------|---------------|-------|--------|
| 2026-03-28 ~07:30 | P0 journal written and design docs updated | — | `82818dd` |
| 2026-03-28 ~08:00 | D1: Add `timeoutMs` parameter to `acceptConnection()`/`connectToServer()`. Move `kSocketTimeoutMs` to header as `kDefaultHandshakeTimeoutMs`. Add `kRunLoopHandshakeTimeoutMs = 500` to ServiceBase.cpp. | — | `60d6570` |
| 2026-03-28 ~08:10 | D7: Add clarifying comment at PlatformWindows.cpp:291 for `ERROR_PIPE_CONNECTED` race condition. | — | `60d6570` |
| 2026-03-28 ~08:15 | D9: Move 4 non-RunLoop tests outside `#if !defined(_WIN32)` guard in ClientBaseTest.cpp. Add `RunLoopModeNotSupportedOnWindows` test. | +1 | `60d6570` |
| 2026-03-28 ~08:12 | D4: Document onError limitation in RunLoop.h and backend_win32.cpp. D10: Add starvation sweep with `dispatchHandle` lambda. Remove dead `timerIds` vector. | — | `2a790e4` |
| 2026-03-28 08:29 | R1 review fixes applied to both repos (see §6). Aether PR #50 merged (squash). | — | `8c46385` |
| 2026-03-28 08:29 | Vortex PR #10 merged (squash). | — | `513b530` |
| 2026-03-28 08:34 | Aether PR #51 merged — submodule pointer updated `4f46c45` → `513b530`. | — | `c459dea` |
| 2026-03-28 ~09:00 | R2 review (code-review + security-reviewer) — clean pass, no code changes. | — | — |

---

## 4. Deliverable Details

### D1 — Handshake Timeout Parameter ✅ SHIPPED

**Problem:**
`acceptConnection()` and `connectToServer()` hard-coded a 5000 ms socket
timeout via an internal `kSocketTimeoutMs` constant.  When called from the
RunLoop `onAcceptReady()` path, a slow or malicious peer could block the
event-loop thread for up to 5 seconds — stalling all other dispatch.

**Fix:**
- Added `uint32_t timeoutMs` parameter to both functions with a default of
  `kDefaultHandshakeTimeoutMs = 5000` (backward compatible).
- Removed private `kSocketTimeoutMs` from Connection.cpp; replaced with the
  public `inline constexpr` in Connection.h.
- `ServiceBase::onAcceptReady()` now passes `kRunLoopHandshakeTimeoutMs = 500`.

**Platform note (documented in code):**
On Linux, `setSocketTimeouts()` constrains `SO_SNDTIMEO` but not `SO_RCVTIMEO`
for the handshake phase.  The asymmetry is inherent to the blocking accept
protocol and is documented in ServiceBase.cpp.

**Files changed:**
| File | Stats |
|------|-------|
| `inc/Connection.h` | +13 −2 |
| `src/Connection.cpp` | +4 −5 |
| `src/ServiceBase.cpp` | +6 −1 |

---

### D4 — onError Dead Code Documentation ✅ SHIPPED

**Problem:**
`addSource(handle, handler, onError)` accepts an `onError` callback that fires
on EPOLLHUP/EPOLLERR (Linux) or EV_EOF (macOS).  On the Win32 WFMO backend,
the callback is stored in `SourceEntry` but **never invoked** — WFMO cannot
distinguish data-ready from error/hangup.  No auto-removal occurs either.

**Fix:**
- Added 4-line doc block to `RunLoop.h` at the `addSource` overload documenting
  the WFMO limitation and referencing the IOCP backend (Phase 5c) as the fix.
- Added 5-line `NOTE` comment in `backend_win32.cpp` source dispatch section
  explaining why only the data handler is invoked.

**Files changed:**
| File | Stats |
|------|-------|
| `inc/RunLoop.h` (Vortex) | +5 |
| `src/backend_win32.cpp` (Vortex) | (included in D10 stats) |

---

### D7 — ERROR_PIPE_CONNECTED Documentation ✅ SHIPPED

**Problem:**
`PlatformWindows.cpp` line 291 handled `ERROR_PIPE_CONNECTED` correctly (return
the pipe as usable), but the code had no comment explaining the race condition:
a client connecting between `CreateNamedPipe` and `ConnectNamedPipe`.

**Fix:**
Added a 5-line comment block explaining:
- The race condition (client connects before `ConnectNamedPipe` is called).
- The pipe is already usable — skip the wait.
- A future async accept path must handle this identically.

**Files changed:**
| File | Stats |
|------|-------|
| `src/PlatformWindows.cpp` | +6 |

---

### D9 — Trapped Tests Fix ✅ SHIPPED

**Problem:**
Four tests in `ClientBaseTest.cpp` were inside a `#if !defined(_WIN32)` guard
that was intended only for RunLoop-dependent tests.  These four tests do **not**
use RunLoop and should run on all platforms:
- `ConcurrentCallsFromSameClient`
- `ConnectToNonexistentService`
- `ConnectTwiceFails`
- `ReconnectAfterServerDisconnect`

**Fix:**
- Moved `RunLoop_ConnectTwiceFails` (which correctly uses RunLoop) to just
  before the `#else` clause, keeping it inside the non-Win32 guard.
- Moved the `#else` / `#endif` boundary so the four non-RunLoop tests fall
  outside the guard and compile on all platforms.
- Added `RunLoopModeNotSupportedOnWindows` test in the `#else` branch to
  verify that RunLoop-based `ClientBase::connect()` correctly fails on Win32.

**Files changed:**
| File | Stats |
|------|-------|
| `test/ClientBaseTest.cpp` | +37 −37 (restructured, net zero lines) |

---

### D10 — WFMO Starvation Mitigation ✅ SHIPPED

**Problem:**
`WaitForMultipleObjects` always returns the lowest signaled index.  Under
sustained load, higher-indexed sources are starved indefinitely — their
handlers never fire while lower-indexed handles are continuously signaled.

**Fix:**
After dispatching the primary signaled handle, a sweep loop polls all remaining
handles (index `idx+1` through `count-1`) with `WaitForSingleObject(h, 0)`:

```cpp
for (DWORD i = idx + 1; i < count; ++i)
{
    if (m_stopRequested.load(std::memory_order_acquire))
        break;
    dispatchHandle(handles[i]);  // only if WaitForSingleObject returns WAIT_OBJECT_0
}
```

The dispatch logic was extracted into a `dispatchHandle` lambda (captures
`[this]`) to avoid duplicating the ~30-line timer-vs-source lookup between
the primary dispatch and the sweep.

**Ancillary cleanup:**
- Removed dead `timerIds` vector that was populated during handle snapshot but
  never read after the dispatch was restructured.

**Documented limitation:** The sweep is a mitigation, not true fairness.
Handles at index 0 still get first dispatch every iteration.  True fairness
requires the IOCP backend where completions arrive in FIFO order.

**Files changed:**
| File | Stats |
|------|-------|
| `src/backend_win32.cpp` (Vortex) | +66 −35 (total for D4 + D10) |

---

### Deferred Items ⏳ BLOCKED ON IOCP (F0)

| ID | Description | Why Deferred |
|----|-------------|--------------|
| D2 | Teardown `handlerMutex` race | Affects WFMO dispatch loop that IOCP replaces entirely |
| D3 | `m_listenFd` is a token, not a handle (Win32) | Token semantics eliminated by overlapped I/O design |
| D5 | `GetQueuedCompletionStatusEx` batch dequeue | Only applies to IOCP backend (not yet written) |
| D6 | `m_listenFd` naming ambiguity | Same as D3 — resolved by IOCP design |
| D8 | Context map locking gaps | Context map redesigned in IOCP backend |

---

## 5. Pull Requests and CI

### Vortex PR #10

- **Title:** Document onError limitation, mitigate WFMO starvation
- **Deliverables:** D4, D10
- **Branch:** merged via squash → `513b530`
- **CI results:** linux-gcc ✓, linux-clang ✓ (2/2)
- **Stats:** 2 files changed, 66 insertions, 35 deletions

### Aether PR #50

- **Title:** Harden handshake timeout, document pipe race, untrap Win32 tests
- **Deliverables:** D1, D7, D9
- **Branch:** merged via squash → `8c46385`
- **CI results:** linux-gcc ✓, linux-clang ✓, asan-ubsan ✓, tsan ✓, macOS ✓, Windows ✓ (6/6)
- **Stats:** 5 files changed, 63 insertions, 45 deletions

### Aether PR #51

- **Title:** Update Vortex submodule to include PR #10
- **Branch:** merged via squash → `c459dea`
- **CI results:** linux-gcc ✓, linux-clang ✓, asan-ubsan ✓, tsan ✓, macOS ✓, Windows ✓ (6/6)
- **Submodule pointer:** `4f46c45` → `513b530`

---

## 6. Code Review

### Round 1 — 3 reviewers: embedded-code-review ×2, TDD coach

| # | Finding | Severity | Resolution | Deliverable |
|---|---------|----------|------------|-------------|
| 1 | Dead `kSocketTimeoutMs` constant left in Connection.cpp after moving to header | MEDIUM (Clang `-Werror` failure) | Removed — the constant now lives as `kDefaultHandshakeTimeoutMs` in Connection.h | D1 |
| 2 | Magic `5000` in header comment, no named constant | LOW | Replaced with `kDefaultHandshakeTimeoutMs` reference | D1 |
| 3 | RunLoop handshake timeout 200 ms too aggressive for CI | MEDIUM | Bumped to 500 ms (`kRunLoopHandshakeTimeoutMs`) | D1 |
| 4 | Missing `m_stopRequested` check in sweep loop — shutdown could stall | MEDIUM | Added `m_stopRequested.load(memory_order_acquire)` check between each sweep dispatch | D10 |
| 5 | Dead `timerIds` vector: populated but never read | LOW | Removed | D10 |
| 6 | Lambda capture `[&]` in dispatch lambda — captures mutable locals unnecessarily | LOW | Changed to `[this]` — lambda only needs member access | D10 |
| 7 | `SO_SNDTIMEO` asymmetry on Linux not documented | LOW | Added `NOTE` comment in ServiceBase.cpp | D1 |

All 7 findings addressed before merge.

### Round 2 — code-review + security-reviewer

**Aether:** Clean pass — 0 issues found across all severity levels.

**Vortex:**
| # | Finding | Severity | Assessment |
|---|---------|----------|------------|
| 1 | No unit test for starvation sweep behavior | MEDIUM | Accepted — test requires Win32 CI runner; added to backlog |
| 2 | No unit test verifying onError never fires on WFMO | MEDIUM | Accepted — documents limitation, not a code bug; added to backlog |

**Security review conclusion:**
- No double-dispatch risk (sweep checks `WAIT_OBJECT_0` return value).
- No stale-handle UB (handles array is a snapshot; mutations go to the map).
- No lock ordering issues (sweep operates on snapshot, no locks held).
- 0 CRITICAL, 0 HIGH findings.

---

## 7. Problems Encountered

| Problem | Root Cause | Resolution | Time to Fix |
|---------|-----------|------------|-------------|
| Clang `-Werror` build failure on CI | Dead `kSocketTimeoutMs` constant left in Connection.cpp after moving to header | Removed the old constant | < 5 min |
| RunLoop accept timeout flaking on CI | 200 ms too tight for loaded CI runners — handshake occasionally exceeded it | Bumped to 500 ms | < 5 min |
| Lambda captured `[&]` triggering review flags | Mutable local captures in a lambda that outlives the scope where locals are defined could cause UB in a different context | Changed to `[this]` — lambda only accesses members | < 5 min |

No debugging sessions exceeded 30 minutes.

---

## 8. Lessons Learned

### What worked well

- **Splitting Vortex and Aether into separate PRs** kept reviews focused and
  allowed independent CI.  Vortex PR #10 could merge before Aether PR #50.
- **Named constants over magic literals** caught a real `-Werror` issue.
  The dead `kSocketTimeoutMs` would have remained unnoticed without the
  constant migration.
- **Documenting limitations over removing code** (D4 onError) preserves API
  stability and sets up a clean upgrade path when IOCP lands.
- **Extracting `dispatchHandle` lambda** made the starvation sweep trivially
  correct — same dispatch path for primary and sweep, no duplication bugs.

### What we'd do differently

- **Start with 500 ms**, not 200 ms, for the RunLoop timeout.  The CI flake
  was predictable — loaded runners add latency.  Rule of thumb: any timeout
  under 1 s should be tested on CI before code review.
- **Write the `SO_SNDTIMEO` documentation up front** rather than as a review
  finding.  Platform asymmetries are exactly the kind of thing that gets
  forgotten later.

### Patterns to carry forward

- **Review-then-defer**: explicitly deferring D2/D3/D5/D6/D8 with documented
  rationale prevents them from being lost while avoiding throw-away work.
- **Starvation sweep as interim mitigation**: zero-timeout polls on remaining
  handles is a cheap, safe technique applicable any time WFMO is used.

---

## 9. Test Coverage

| Component | Tests | Coverage Notes |
|-----------|-------|----------------|
| `Connection::acceptConnection()` | Existing connection tests | Timeout parameter exercised implicitly via ServiceBase. **Backlog:** explicit `AcceptWithShortTimeout_SlowPeerFails` test. |
| `Connection::connectToServer()` | Existing client tests | Timeout parameter uses default 5000 ms in all existing tests. |
| `ServiceBase::onAcceptReady()` | RunLoop integration tests | Now calls with 500 ms timeout. No explicit slow-peer test yet. |
| `PlatformWindows.cpp` `ERROR_PIPE_CONNECTED` | Existing pipe tests | Race is hard to reproduce deterministically — documentation-only change. |
| `ClientBaseTest` trapped tests | 4 tests now run on Win32 | `ConcurrentCallsFromSameClient`, `ConnectToNonexistentService`, `ConnectTwiceFails`, `ReconnectAfterServerDisconnect` |
| `backend_win32.cpp` starvation sweep | No dedicated test | **Backlog:** `Win32_StarvationSweepDispatchesMultipleSignaledHandles` (requires Win32 CI runner) |
| `backend_win32.cpp` onError limitation | No dedicated test | **Backlog:** `Win32_OnErrorNeverFiresOnWFMO` (documents limitation) |

### Backlogged tests (from TDD coach review)

| Test Name | Purpose | Blocked On |
|-----------|---------|------------|
| `ConnectionTest::AcceptWithShortTimeout_SlowPeerFails` | Validates D1 timeout rejection with a deliberately slow peer | Test infrastructure for simulating slow handshake |
| `RunLoopTest::Win32_StarvationSweepDispatchesMultipleSignaledHandles` | Validates D10 sweep dispatches multiple signaled handles in one iteration | Win32 CI runner |
| `RunLoopTest::Win32_OnErrorNeverFiresOnWFMO` | Documents D4 limitation — verifies onError callback is never invoked | Win32 CI runner |

---

## 10. Metrics

| Metric | Value |
|--------|-------|
| **Lines added (Aether)** | 63 (PR #50) |
| **Lines removed (Aether)** | 45 (PR #50) |
| **Lines added (Vortex)** | 66 (PR #10) |
| **Lines removed (Vortex)** | 35 (PR #10) |
| **Net new lines** | +49 |
| **Tests untrapped (Win32)** | 4 (`ClientBaseTest`) |
| **Tests added** | 1 (`RunLoopModeNotSupportedOnWindows`) |
| **Tests backlogged** | 3 |
| **Code review rounds** | 2 |
| **Review findings addressed** | 7 (Round 1), 0 code changes (Round 2) |
| **PRs merged** | 3 (Vortex #10, Aether #50, Aether #51) |
| **CI pipelines green** | 14/14 total (2 Vortex + 6 Aether #50 + 6 Aether #51) |
| **Aether total tests** | 140 C++ + 303 Python — all pass |
| **Vortex total tests** | 47 (6 Win32-only skipped on Linux/macOS) — all pass |
| **Build time impact** | Negligible |
| **Memory impact** | ~100 bytes flash (two `constexpr` constants, sweep loop code), 0 RAM at rest |

---

## 11. Files Changed — Complete List

### Aether (PR #50 — `8c46385`)

| File | Change | Stats |
|------|--------|-------|
| `inc/Connection.h` | Add `kDefaultHandshakeTimeoutMs`, `timeoutMs` parameter to both handshake functions | +13 −2 |
| `src/Connection.cpp` | Remove `kSocketTimeoutMs`, use `timeoutMs` parameter | +4 −5 |
| `src/ServiceBase.cpp` | Add `kRunLoopHandshakeTimeoutMs = 500`, pass to `acceptConnection()` | +6 −1 |
| `src/PlatformWindows.cpp` | Document `ERROR_PIPE_CONNECTED` race condition | +6 |
| `test/ClientBaseTest.cpp` | Move `#else`/`#endif` to untrap 4 tests, add Windows guard test | +37 −37 |

### Aether (PR #51 — `c459dea`)

| File | Change | Stats |
|------|--------|-------|
| `deps/vortex` | Submodule `4f46c45` → `513b530` | +1 −1 |

### Vortex (PR #10 — `513b530`)

| File | Change | Stats |
|------|--------|-------|
| `inc/RunLoop.h` | Document WFMO onError limitation | +5 |
| `src/backend_win32.cpp` | Extract `dispatchHandle` lambda, add starvation sweep, remove dead `timerIds`, document onError dead code | +66 −35 |

---

## 12. Next Steps

| Priority | Task | Dependency | Description |
|----------|------|------------|-------------|
| **F0** | Vortex IOCP rewrite | None | Replace WFMO backend with I/O Completion Ports. Resolves D2, D3, D5, D6, D8. Enables true fairness (replacing D10 sweep). |
| **F0b** | Wire onError via IOCP | F0 | Implement error/hangup detection using IOCP completion status, resolving D4. |
| **F1** | Aether overlapped I/O | F0 | `PlatformWindows.cpp` moves to overlapped `ReadFile`/`WriteFile` on named pipes. |
| **F2** | Wire overlapped events into RunLoop | F1 | Connect Aether pipe handles to Vortex IOCP loop. |
| **F3** | Remove RunLoop rejection guard | F2 | Drop `#if !defined(_WIN32)` guard around RunLoop integration. |
| **F4** | RunLoop integration tests on Windows CI | F3 | End-to-end tests with real named pipes on Windows runners. |
| **Backlog** | 3 backlogged tests | Win32 CI runner | See §9 backlogged tests table. |
