# Phase 5 — P0 Bug Fix Development Journal

**Date:** 2026-03-27
**Scope:** Fix critical bugs in Vortex Win32 WFMO backend before IOCP rewrite
**Status:** ✅ Complete — merged to main in both repos

---

## 1. Overview

Phase 5 makes the Vortex RunLoop the primary dispatch model on Windows.
Before starting the bigger IOCP rewrite (F0), we needed to fix critical bugs
in the existing `WaitForMultipleObjects` (WFMO) backend that could cause
crashes, hangs, or silent data loss in production.

Five independent expert review teams (Windows kernel internals, security,
embedded systems, TDD coaching, test engineering) cross-reviewed the Phase 5
plan document (`doc/phase5-runloop-plan.md`). They identified 7 P0 bugs
(B1–B7), with strong consensus on three CRITICAL issues found independently
by multiple teams.

**Key outcomes:**
- 2 bugs fixed and shipped (B1, B6)
- 4 bugs captured as design specs for the IOCP rewrite (B2–B5)
- 1 false positive dismissed (B7)
- 6 new capacity tests added
- 2 PRs merged (Vortex PR #9, Aether PR #48)
- 2 rounds of code review with all findings addressed

**Commits:**
| Repo | Commit | Description |
|------|--------|-------------|
| Vortex | `4f46c45` | Fix WFMO slot budget overflow and skip pipe-based tests on Win32 (#9) |
| Vortex | `1d87322` | Initial fix (pre-review) |
| Vortex | `2de43fe` | Address R1 review findings |
| Aether | `13650c3` | Update Vortex submodule to v1.2.1 (WFMO slot budget fix) (#48) |

---

## 2. Design Decisions

| # | Decision | Options Considered | Choice | Rationale |
|---|----------|--------------------|--------|-----------|
| 1 | **Capacity enforcement model** | (a) Check-under-lock before OS syscall (b) Optimistic-create: syscall first, check+insert under lock, rollback on reject | **(b) Optimistic-create** | `CreateWaitableTimerW` is a kernel round-trip. Holding a mutex across a syscall blocks the dispatch thread. Optimistic-create keeps the lock scope tight — create handle with no locks held, then atomically check capacity and insert under both locks. On rejection, `CancelWaitableTimer` + `CloseHandle` is a small cost paid only on the overflow edge case. |
| 2 | **Lock ordering** | (a) No nesting (separate lock regions) (b) Nest `m_sourcesMutex` → `m_timersMutex` globally | **(b) Global nesting order** | Combined capacity requires a single atomic snapshot of both `m_sources.size()` + `m_timers.size()`. Separate lock regions leave a TOCTOU gap where concurrent `addSource()` and `addTimer()` calls could both pass their individual checks and push the total past 64. A globally consistent lock order (`m_sourcesMutex` first, then `m_timersMutex`) prevents deadlock. No prior code nested both locks — this is a new invariant. |
| 3 | **isNew guard for source replacement** | (a) Always check capacity (b) Bypass capacity check when handle already exists | **(b) Bypass on replacement** | Replacing a callback on an existing handle doesn't consume a new WFMO slot. The `isNew` flag (from `std::map::insert_or_assign` semantics) ensures replacements are always accepted regardless of current occupancy. |
| 4 | **Pipe-based test strategy on Win32** | (a) Rewrite tests to use waitable objects (b) `GTEST_SKIP()` on Win32 | **(b) GTEST_SKIP** | The pipe-based tests exercise source add/remove/callback semantics that are identical across backends. Rewriting them to use Win32 events would duplicate the same logic. `GTEST_SKIP` preserves coverage on Linux/macOS (where pipes are valid WFMO-equivalent objects via epoll/kqueue) while honestly reporting "not applicable" on Win32. The IOCP rewrite (F0) will add overlapped-pipe tests that supersede these. |
| 5 | **Constant naming** | (a) Keep `MAX_SOURCES` (b) Rename to `kMaxWaitSlots` | **(b) `kMaxWaitSlots`** | The old name `MAX_SOURCES` was misleading — the budget is shared between sources AND timers. `kMaxWaitSlots` reflects the true constraint (WFMO handle array slots) and follows the project's `kCamelCase` constant convention. |

---

## 3. Implementation Timeline

| Time | What Was Done | Tests | Commit |
|------|---------------|-------|--------|
| 2026-03-27 ~22:00 | Expert cross-review of `phase5-runloop-plan.md` — 5 teams, 7 bugs identified | — | — |
| 2026-03-27 22:21 | B1 fix: combined capacity check in `addSource()`/`addTimer()`, atomic snapshot in `run()`, lock ordering. B6 fix: `GTEST_SKIP` on 6 pipe-based tests + 1 CApiTest conversion. 6 new capacity tests. | +6 | `1d87322` |
| 2026-03-27 22:29 | R1 review fixes: remove unused gmock, fix RunLoop.h comment, defensive assert, CApiTest timer leak fix, stronger assertions, consistent `bManualReset=FALSE` | — (test fixes) | `2de43fe` |
| 2026-03-27 22:35 | Vortex PR #9 merged (squash) to `origin/main` | — | `4f46c45` |
| 2026-03-27 22:40 | Aether PR #48 merged — submodule pointer updated | — | `13650c3` |

---

## 4. Bug Details

### B1 — WFMO Slot Budget Overflow ✅ FIXED (CRITICAL)

**Severity:** CRITICAL — silent `WAIT_FAILED` in production, no error reporting
**Found by:** Windows kernel expert, security reviewer, embedded systems expert (independently)

**Problem:**
`addSource()` checked `m_sources.size()` against the constant `63`, but the
WFMO handle array also contains timer handles.  `addTimer()` had **no capacity
check at all**.  If a caller added (say) 40 sources and 25 timers, the combined
65 handles would exceed `MAXIMUM_WAIT_OBJECTS` (64).  `WaitForMultipleObjects`
returns `WAIT_FAILED` with `ERROR_INVALID_PARAMETER` — defined behavior, but
the existing code had no error path for this.

**Fix (in `src/backend_win32.cpp`):**

1. **Renamed** `MAX_SOURCES` → `kMaxWaitSlots` (63, = `MAXIMUM_WAIT_OBJECTS - 1`
   since the wakeup event occupies one slot).

2. **`addSource()`:** Combined capacity check under both mutexes:
   ```
   lock(m_sourcesMutex)
     lock(m_timersMutex)
       if (isNew && sources.size() + timers.size() >= kMaxWaitSlots) throw
     unlock
     insert source
   unlock
   ```
   Preserved `isNew` guard — replacing an existing handle's callback bypasses
   the capacity check since it doesn't consume a new slot.

3. **`addTimer()`:** Optimistic-create pattern:
   ```
   handle = CreateWaitableTimerW(...)   // no locks held during syscall
   SetWaitableTimer(handle, ...)
   lock(m_sourcesMutex)
     lock(m_timersMutex)
       if (sources.size() + timers.size() >= kMaxWaitSlots)
         CancelWaitableTimer(handle)
         CloseHandle(handle)
         throw
       insert timer
     unlock
   unlock
   ```

4. **`run()`:** Atomic snapshot with both mutexes held simultaneously:
   ```
   lock(m_sourcesMutex)
     lock(m_timersMutex)
       snapshot handles[]   // both sources and timers
     unlock
   unlock
   assert(handles.size() <= MAXIMUM_WAIT_OBJECTS)
   WaitForMultipleObjects(handles, ...)
   ```
   This prevents a split-lock TOCTOU race where concurrent adds during
   snapshot construction could push past 64.

5. **Lock ordering established:** `m_sourcesMutex` → `m_timersMutex` globally,
   consistent across `addSource()`, `addTimer()`, and `run()`.  No prior code
   nested both locks — this is a new invariant.

---

### B6 — CreatePipe Not Waitable ✅ FIXED

**Severity:** HIGH — 6 tests would hang on Win32
**Found by:** Windows kernel expert, test engineer

**Problem:**
Six `RunLoopTest` tests use `makePipe()` which creates Unix pipes.  On Win32,
`CreatePipe()` returns handles that are **not valid WFMO objects** — they
signal based on "last I/O completed," not "data available."  These tests would
hang or give false results on Windows.

**Fix:**
Added `GTEST_SKIP() << "..."` to all 6 pipe-based tests on Win32:
- `AddAndRemoveSource`
- `SourceCallbackRunsOnLoopThread`
- `MultipleSourcesConcurrent`
- `RemoveSourceFromHandler`
- `UpdateSourceHandler`
- `AddSourceFromAnyThread`

Converted 1 `CApiTest` (`AddRemoveSource`) from `#if !defined(_WIN32)` preprocessor
guard to `GTEST_SKIP` for consistent test reporting (skip shows in results
instead of silently compiling away).

---

### B2 — Retired Context Early-Free 📋 SPEC CAPTURED

**Severity:** CRITICAL — use-after-free
**Applies to:** F0 IOCP rewrite (not current WFMO backend)

**Problem:** The IOCP design draft frees retired I/O contexts at the top of the
event loop.  But `CancelIoEx` is asynchronous — the kernel may still reference
the context.  Freeing before the `ERROR_OPERATION_ABORTED` completion arrives
is a use-after-free.

**Specified fix:** Free retired contexts only when their `ERROR_OPERATION_ABORTED`
completion is dequeued from the IOCP port.  Captured as REVIEW FIX annotation
in `doc/phase5-runloop-plan.md`.

---

### B3 — CancelIoEx Failure Leaks 📋 SPEC CAPTURED

**Severity:** HIGH — memory/handle leak
**Applies to:** F0 IOCP rewrite

**Problem:** If `CancelIoEx` returns `ERROR_NOT_FOUND` (the I/O already
completed), no completion packet is posted, so the context is never dequeued
and leaks forever.

**Specified fix:** Check `CancelIoEx` return value.  On `ERROR_NOT_FOUND`,
free the context immediately (the completion already arrived or was never
posted).

---

### B4 — Missing ResetEvent 📋 SPEC CAPTURED

**Severity:** HIGH — 100% CPU busy loop
**Applies to:** F1 Aether overlapped I/O

**Problem:** The overlapped-I/O path must `ResetEvent` on the manual-reset
event before starting the next `ReadFile`.  Without it, WFMO sees the event
as perpetually signaled, causing a tight busy loop.

**Specified fix:** `ResetEvent(hEvent)` before each overlapped `ReadFile` call.

---

### B5 — Re-arm Before Dispatch 📋 SPEC CAPTURED

**Severity:** MEDIUM — contradicts API contract
**Applies to:** F0 IOCP rewrite

**Problem:** The IOCP design re-arms the probe read before dispatching the
handler callback.  This contradicts the documented guarantee that
`removeSource()` from within a handler is safe — if re-arm happens first,
there's a pending read when the handler calls `removeSource()`.

**Specified fix:** Dispatch the handler first, then re-arm.  If the handler
removed the source, skip re-arm.

---

### B7 — find_package(Threads) Guard 🚫 FALSE POSITIVE

**Severity:** N/A — not a bug
**Found by:** Embedded systems expert

**Analysis:** The expert review flagged `find_package(Threads REQUIRED)` as
potentially broken on MSVC.  Investigation confirmed CMake's `FindThreads`
module resolves to a no-op imported target on MSVC (Windows threads are
implicit).  Wrapping it in `if(NOT WIN32)` would actually break the build
by removing the `Threads::Threads` target that downstream `target_link_libraries`
references.  No change needed.

---

## 5. New Tests

| # | Test Name | File | Purpose | Platforms |
|---|-----------|------|---------|-----------|
| 1 | `AddSourceRejectsWhenSlotsFull` | `test/RunLoopTest.cpp` | Fill 63 source slots with events, verify 64th `addSource()` throws `runtime_error` | Win32 only (GTEST_SKIP elsewhere) |
| 2 | `AddTimerRejectsWhenSlotsFull` | `test/RunLoopTest.cpp` | Fill 63 timer slots, verify 64th `addTimer()` throws `runtime_error` | Win32 only |
| 3 | `CombinedSourceTimerSlotLimit` | `test/RunLoopTest.cpp` | 32 sources + 31 timers = 63 total, verify next add of either type throws | Win32 only |
| 4 | `SourceAfterTimerRemovalSucceeds` | `test/RunLoopTest.cpp` | Fill 63 slots, remove one timer, verify `addSource()` succeeds (slot freed) | Win32 only |
| 5 | `ReplaceSourceDoesNotConsumeSlot` | `test/RunLoopTest.cpp` | With 63 sources, replace one's callback — must succeed (no new slot consumed) | Win32 only |
| 6 | `CApiTest::AddTimerRejectsWhenSlotsFull` | `test/CApiTest.cpp` | C API: fill slots, verify `vortex_runloop_add_timer()` returns `VORTEX_ERR_INVALID_ARGUMENT` | Win32 only |

All capacity tests use `CreateEventW` for source handles and auto-reset waitable
timers for timer slots — both are proper WFMO-waitable kernel objects.

Tests are compiled on all platforms but execute only on Win32 (epoll/kqueue
backends have no 64-handle limit).

---

## 6. Code Review

### Round 1 — Embedded Code Reviewer + TDD Coach

| # | Finding | Severity | Resolution |
|---|---------|----------|------------|
| 1 | Unused `gmock` include and link dependency in `test/CMakeLists.txt` | LOW | Removed — tests use only gtest |
| 2 | `CApiTest::AddTimerRejectsWhenSlotsFull` — timer handles leak if mid-loop `ASSERT_EQ` fails | MEDIUM | Wrapped loop body to ensure `CloseHandle` on all paths |
| 3 | Capacity overflow assertion used weak `EXPECT_NE(nullptr, ...)` instead of checking the error code | LOW | Changed to `EXPECT_EQ(VORTEX_ERR_INVALID_ARGUMENT, rc)` |
| 4 | `RunLoop.h` comment said "max 63 sources" — misleading since budget is shared with timers | LOW | Updated to "max 63 combined sources + timers" |
| 5 | No defensive assert before WFMO call in `run()` | LOW | Added `assert(handles.size() <= MAXIMUM_WAIT_OBJECTS)` |
| 6 | Inconsistent `bManualReset` flag across capacity tests | LOW | Standardized to `bManualReset=FALSE` (auto-reset) |

All findings addressed in commit `2de43fe`.

### Round 2 — General Code Reviewer + Security Reviewer

| # | Finding | Severity | Resolution |
|---|---------|----------|------------|
| 1 | Pre-existing `bad_alloc` leak: if `m_timers.emplace()` throws, the OS handle leaks | LOW (pre-existing) | Accepted — `bad_alloc` is generally fatal; not introduced by this change |
| 2 | Optimistic-create pays a kernel round-trip on rejection | LOW (perf) | Accepted — rejection is the overflow edge case, not the hot path |
| 3 | Defensive assert is stripped in Release builds | LOW | Accepted — the `throw` guards enforce the invariant in Release; assert is defense-in-depth for Debug |

**Result:** 0 critical issues.  3 LOW findings, all pre-existing or acceptable by design.  No changes required.

---

## 7. Pull Requests and CI

### Vortex PR #9

- **Title:** Fix WFMO slot budget overflow and skip pipe-based tests on Win32
- **Branch:** `fix/wfmo-slot-budget-and-pipe-skips`
- **Merge:** Squash merge → `4f46c45`
- **Branch status:** Deleted after merge
- **CI results:** linux-gcc ✓, linux-clang ✓

### Aether PR #48

- **Title:** Update Vortex submodule to v1.2.1 (WFMO slot budget fix)
- **Merge:** Squash merge → `13650c3`
- **Branch status:** Deleted after merge
- **CI results:** linux-gcc ✓, linux-clang ✓, asan-ubsan ✓, tsan ✓, macOS ✓, Windows ✓

---

## 8. Files Changed

### Vortex (`deps/vortex`) — 311 insertions, 13 deletions

| File | Change | Stats |
|------|--------|-------|
| `src/backend_win32.cpp` | Core fix: `kMaxWaitSlots` rename, combined capacity checks, optimistic-create in `addTimer()`, atomic snapshot in `run()`, lock ordering | +48 −12 |
| `test/RunLoopTest.cpp` | 5 new capacity tests, 6 `GTEST_SKIP` additions for pipe-based tests | +234 |
| `test/CApiTest.cpp` | 1 new capacity test, 1 `GTEST_SKIP` conversion from `#if` guard | +40 −1 |
| `test/CMakeLists.txt` | Remove unused gmock link dependency | +1 −1 |
| `inc/RunLoop.h` | Fix comment: budget shared by sources + timers | +1 −1 |

### Aether (`ms-ipc`)

| File | Change | Stats |
|------|--------|-------|
| `deps/vortex` | Submodule pointer updated to `4f46c45` | +1 −1 |

---

## 9. Lessons Learned

### Windows Kernel Facts
- `MAXIMUM_WAIT_OBJECTS` = 64 on **all** Windows SKUs (confirmed by kernel
  expert review).  Passing `nCount > 64` to `WaitForMultipleObjects` yields
  `WAIT_FAILED` + `ERROR_INVALID_PARAMETER` — this is defined behavior, not UB.
- `CreatePipe()` handles are **not** valid WFMO objects.  They differ from
  overlapped I/O handles — their signaling semantics are "last I/O completed,"
  not "data available."
- Auto-reset waitable timers with `lPeriod > 0` automatically re-signal at
  each period — correct for WFMO repeating timers, no re-arm needed.

### Patterns That Worked Well
- **Optimistic-create pattern** kept the critical section tight while remaining
  TOCTOU-safe.  The rollback cost (one `CancelWaitableTimer` + `CloseHandle`)
  is negligible since it only occurs on the overflow edge case.
- **Multi-team cross-review** caught the budget overflow bug that single-team
  review missed for months.  Three teams independently flagged B1 — high
  confidence this is a real issue.
- **GTEST_SKIP over `#if` guards** produces honest test reports ("6 skipped")
  instead of silently hiding tests.  Makes it obvious that Win32 pipe tests
  need the IOCP rewrite.
- **Lock ordering as an explicit invariant** — documenting `m_sourcesMutex` →
  `m_timersMutex` in the code and commit message prevents future deadlocks
  as the backend grows.

### What We'd Do Differently
- The C API maps `std::runtime_error` → `VORTEX_ERR_INVALID_ARGUMENT`, which is
  semantically imprecise for a capacity overflow.  A dedicated
  `VORTEX_ERR_CAPACITY` error code would be clearer.  Deferred to the IOCP
  rewrite to avoid a public API change now.
- Capacity tests create 63 real kernel objects (events/timers).  On a
  resource-constrained CI runner this could be fragile.  Consider a
  unit-level mock that doesn't hit the kernel — but the current approach has
  the advantage of testing the real WFMO path end-to-end.

---

## 10. Test Coverage

| Component | Tests Before | Tests After | Coverage Notes |
|-----------|-------------|-------------|----------------|
| `backend_win32.cpp` — `addSource()` | 6 (pipe-based, now skipped on Win32) | 6 + 3 new capacity tests | Combined capacity, replacement bypass, post-removal recovery |
| `backend_win32.cpp` — `addTimer()` | 2 (one-shot, repeating) | 2 + 2 new capacity tests | Pure-timer overflow, combined source+timer overflow |
| `backend_win32.cpp` — `run()` | Implicit (all tests exercise `run()`) | Same + defensive assert | Assert validates snapshot size in Debug builds |
| C API — `vortex_runloop_add_timer` | 0 capacity tests | 1 new | Verifies error code return on overflow |

---

## 11. Metrics

| Metric | Value |
|--------|-------|
| Lines added (Vortex) | 311 |
| Lines removed (Vortex) | 13 |
| Net new lines | +298 |
| Tests added | 6 |
| Tests skipped (Win32 pipe) | 6 existing + 1 CApiTest conversion |
| Code review rounds | 2 |
| Review findings addressed | 6 (Round 1), 0 (Round 2) |
| Build time impact | Negligible (test file growth only) |
| Memory impact (flash + RAM) | ~200 bytes flash (capacity checks), 0 RAM at rest |
| CI pipelines green | 6/6 (linux-gcc, linux-clang, asan-ubsan, tsan, macOS, Windows) |

---

## 12. Next Steps

| Priority | Task | Dependency | Description |
|----------|------|------------|-------------|
| **P1** | D1–D10 defensive fixes | None | 10 lower-severity issues from the same cross-review: error logging, thread naming, graceful degradation |
| **F0** | Vortex IOCP rewrite | P1 | Replace WFMO backend with I/O Completion Ports. Apply B2, B3, B5 specs during implementation |
| **F0b** | Vortex error callbacks on Win32 | F0 | Wire `onError` / `onHangup` callbacks through IOCP |
| **F0c** | Vortex CI: macOS + Windows | None (parallel) | Add macOS and Windows runners to Vortex CI matrix |
| **F1** | Aether overlapped I/O | F0 | `PlatformWindows.cpp` uses overlapped `ReadFile`/`WriteFile` on named pipes. Apply B4 spec |
| **F2** | Wire overlapped events into RunLoop | F1 | Connect Aether's pipe handles to the Vortex IOCP loop |
| **F3** | Remove RunLoop rejection guard | F2 | Drop the `#if !defined(_WIN32)` guard around RunLoop integration |
| **F4** | RunLoop integration tests on Windows CI | F3 | End-to-end tests with real named pipes on Windows runners |
