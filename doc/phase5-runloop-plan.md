# Phase 5 — Vortex RunLoop Completion: Expert Review & Implementation Plan

## Status

| Priority | Description | Status |
|----------|-------------|--------|
| **P0** | Critical bug fixes (B1 slot budget, B6 pipe skips) | ✅ **DONE** — Vortex PR #9, Aether PR #48 |
| **P0-spec** | IOCP design fixes (B2-B5) | 📋 Captured as REVIEW FIX annotations below |
| **P1** | Hardening bugs (D1-D10) | ⏳ Pending |
| **F0** | Vortex IOCP rewrite | ⏳ Pending |
| **F1-F4** | Aether Windows overlapped I/O + RunLoop integration | ⏳ Pending (depends on F0) |

## Overview

Phase 5 makes RunLoop the primary dispatch model on all platforms. Today RunLoop
works on Linux (epoll) and macOS (kqueue) but is blocked on Windows because:

1. **Vortex Win32 backend** uses `WaitForMultipleObjects` — can't watch pipes/sockets,
   63-handle limit, no error callbacks.
2. **Aether PlatformWindows.cpp** uses synchronous named pipe I/O — no waitable handle
   for RunLoop to watch.

Four expert teams reviewed the problem, followed by five independent cross-review teams
that validated the original findings. Their combined recommendations follow.

> **Minimum supported platform: Windows 10.** `CancelIoEx` requires Vista+; zero-byte
> overlapped reads are reliable on Win8+. Aligning with Microsoft's support lifecycle,
> we target Windows 10 as the minimum. Document in README and enforce via CMake.

---

## Team 1: Vortex Win32 IOCP Architecture

### Problem

`WaitForMultipleObjects` can only watch waitable kernel objects (events, semaphores),
not pipes or sockets. Limited to 63 handles. IOCP (`CreateIoCompletionPort`) is the
Win32 mechanism that collects overlapped I/O completions into a single waitable queue.

### Core Design

`addSource(handle, handler)` on Linux means "tell me when this fd is readable." On
Windows with IOCP, it means "issue a zero-byte overlapped read on this handle, and
when it completes (data available or pipe broke), call the handler."

```
addSource(pipeHandle, handler)
  │
  ├─► CreateIoCompletionPort(pipeHandle, iocp, key, 0)  // associate once
  └─► ReadFile(pipeHandle, buf, 0, NULL, &overlapped)    // zero-byte probe read
                                                          // ERROR_IO_PENDING = normal
          ┌─────────────────────┐
          │ IOCP queue          │
          │ GetQueuedCompletion │ ◄── run() loop dequeues
          │ Status(iocp, ...)   │
          └─────────────────────┘
                    │
                    ▼
          handler() fires, then re-arm next probe read
```

### Per-Source State

```cpp
// Defined in backend_win32.cpp (not in header — OVERLAPPED is a Windows type)
struct OverlappedContext {
    OVERLAPPED ov;          // must remain valid until I/O completes or is cancelled
    HANDLE handle;          // the pipe/file handle
    bool pendingRead;       // true while zero-byte ReadFile is outstanding
    bool cancelled;         // set by removeSource, checked on completion
    uint8_t probeBuf[1];   // 1-byte buffer for probe read
};
```

Header adds an opaque pointer to `SourceEntry`:
```cpp
struct SourceEntry {
    std::function<void()> handler;
    std::function<void()> onError;
#if defined(_WIN32)
    void* platformCtx = nullptr;  // OverlappedContext*, owned by backend
#endif
};
```

### Wakeup Mechanism

Replace `m_wakeupHandle` (manual-reset event) with `PostQueuedCompletionStatus`:
```cpp
void RunLoop::wakeup() {
    PostQueuedCompletionStatus(m_pollHandle, 0, kKeyWakeup, nullptr);
}
```
Thread-safe by design. Simpler than the event-based approach. `m_wakeupHandle` is
removed entirely.

### Timer Implementation

Use `CreateThreadpoolTimer` with callbacks that post to IOCP:
```cpp
// Timer callback runs on threadpool thread, posts to IOCP:
PTP_TIMER tpTimer = CreateThreadpoolTimer(
    [](PTP_CALLBACK_INSTANCE, PVOID ctx, PTP_TIMER) {
        auto* tc = static_cast<TimerContext*>(ctx);
        PostQueuedCompletionStatus(tc->loop->m_pollHandle, 0,
            kKeyTimerTag | tc->id, nullptr);
    }, timerCtx, nullptr);
```
Timer handler runs on the RunLoop thread (not the threadpool thread) — no concurrency
issues. `removeTimer` uses `WaitForThreadpoolTimerCallbacks(tp, TRUE)` to cancel.

Stale `PostQueuedCompletionStatus` packets may linger after `removeTimer` — run loop
checks `m_timers.find(tid)` before dispatching.

### Error/Hangup Detection

Maps to epoll `EPOLLHUP`/`EPOLLERR`:

| Win32 Error | Meaning | Equivalent |
|---|---|---|
| `ERROR_BROKEN_PIPE` (109) | Peer closed pipe | `EPOLLHUP` |
| `ERROR_PIPE_NOT_CONNECTED` (233) | Pipe disconnected | `EPOLLHUP` |
| `ERROR_NO_DATA` (232) | Pipe closing | `EPOLLHUP` |
| `ERROR_OPERATION_ABORTED` (995) | `CancelIoEx` result | Not an error — normal removeSource |

After a successful zero-byte read completion, use `PeekNamedPipe` as a secondary
health check — the peer may have closed between completion and processing.

Dispatch logic (matching epoll):
- Error + `onError` callback → call `onError`, auto-remove source
- Error + no `onError` → call normal `handler` (app's read fails, it handles disconnect)
- No error → call `handler`

### addSource Flow

1. Lock `m_sourcesMutex`
2. If handle already in `m_sources`: update callbacks only (can't re-associate with IOCP)
3. If new: `CreateIoCompletionPort(handle, iocp, (ULONG_PTR)handle, 0)`
4. Allocate `OverlappedContext`, store in `SourceEntry.platformCtx`
5. Issue zero-byte `ReadFile` (probe read)
6. Wakeup loop

### removeSource Flow

1. Lock `m_sourcesMutex`
2. Mark `ctx->cancelled = true`
3. If `ctx->pendingRead`: call `CancelIoEx(handle, &ctx->ov)`
   - If `CancelIoEx` returns FALSE and `GetLastError() == ERROR_NOT_FOUND`:
     no pending I/O exists — **free the context immediately** (do NOT retire).
     Otherwise the context is retired but no cancellation completion ever arrives,
     causing an unbounded memory leak.
   - If `CancelIoEx` succeeds or returns another error: move to `m_retiredContexts`.
4. If no pending read: free context immediately (no retirement needed).
5. Remove from `m_sources`, wakeup loop

When a retired context's cancellation completion arrives (`ERROR_OPERATION_ABORTED`),
the run loop finds the handle not in `m_sources`, scans `m_retiredContexts`, and
frees it.

> **REVIEW FIX (B3):** Original unconditionally retired all contexts. If `CancelIoEx`
> fails with `ERROR_NOT_FOUND`, no completion ever arrives and the context leaks
> forever. Now checks return value and frees immediately when appropriate.

### run() Loop Structure

```
while (!stopRequested) {
    1. Drain post queue (executeOnRunLoop callables)
    2. GetQueuedCompletionStatusEx(iocp, entries[], count, INFINITE)
       — batch dequeue for O(1) per-event amortization (not GQCS which is 1-at-a-time)
    3. For each completion in entries[]:
       a. Check GQCS return: if FALSE with non-NULL overlapped → deferred error
          - look up source, fire onError or handler with error context
       b. key == kKeyWakeup → continue (drain post queue next iteration)
       c. key & kKeyTimerTag → fire timer handler (if still in m_timers)
       d. key matches retired context (ERROR_OPERATION_ABORTED) → free the
          OverlappedContext from m_retiredContexts. Do NOT free retired contexts
          at loop top — CancelIoEx is async, kernel may still reference OVERLAPPED.
       e. otherwise → source completion:
          - copy handler under m_sourcesMutex, then RELEASE lock before calling
            (prevents deadlock if handler calls addSource/removeSource)
          - if cancelled or not found → skip (stale completion)
          - if error (broken pipe etc) → fire onError or handler
          - if success → fire handler
    4. Re-issue probe reads for sources with pendingRead == false
       — AFTER handler dispatch, not before. This ensures removeSource from
       within a handler finds no pending read (safe, no CancelIoEx needed).
}
```

> **REVIEW FIX (B2):** Original freed retired contexts at loop top — use-after-free
> risk since CancelIoEx is async. Now freed only on ERROR_OPERATION_ABORTED dequeue.
>
> **REVIEW FIX (B5):** Original re-armed probes before dispatch. removeSource from
> handler would find pending read, contradicting safety claim. Now re-arms after.
>
> **REVIEW FIX (D5):** Changed GQCS → GQCSEx for batch dequeue, matching epoll/kqueue.
>
> **REVIEW FIX (M1):** Added step 3a — GQCS FALSE with non-NULL overlapped means
> deferred I/O error. Original dispatch logic only checked completion key.

### init() Changes

```cpp
void RunLoop::init(const char* name) {
    m_pollHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
    // concurrency = 1: single run-loop thread dequeues
    // No wakeup handle needed — PostQueuedCompletionStatus serves that role
}
```

### Edge Cases

1. **removeSource while read pending** — `CancelIoEx` + retire context (or free
   immediately if `CancelIoEx` returns `ERROR_NOT_FOUND`). Completion arrives as
   `ERROR_OPERATION_ABORTED`, freed from `m_retiredContexts`.
2. **addSource twice for same handle** — `CreateIoCompletionPort` only called once.
   Second call just updates callbacks.
3. **Handle closed externally** — overlapped read completes with error, routed to
   `onError` or handler.
4. **removeSource from within handler** — safe **because probe reads are re-armed
   AFTER handler dispatch** (run() loop step 4). The `OVERLAPPED` has already been
   dequeued (the completion that triggered this handler) and no new probe read has
   been issued yet. No pending kernel reference exists.
5. **CancelIoEx failure** — if `CancelIoEx` returns FALSE with `ERROR_NOT_FOUND`,
   no pending I/O exists. Free the context immediately; do not retire.

### Destruction Ordering (critical)

The destructor must follow this exact sequence to avoid use-after-free of OVERLAPPED
structures:

1. `stop()` — signal the loop to exit and wait for the run() thread to join
2. Cancel all pending reads via `CancelIoEx` for every source in `m_sources`
3. Drain remaining completions from IOCP until all cancellations are accounted for
   (or use `GetOverlappedResult(..., TRUE)` to block per-handle)
4. Free all `OverlappedContext` objects (both active and retired)
5. Close the IOCP handle via `CloseHandle(m_pollHandle)`

> **REVIEW FIX (M3):** Original plan said "init()/destructor — create IOCP, cleanup"
> without specifying the destruction order. Getting this wrong causes use-after-free.

### Test Changes Required

- `makePipe()` in `RunLoopTest.cpp` must create **overlapped named pipes** (not
  `CreatePipe` which creates synchronous anonymous pipes incompatible with IOCP)
- 3 error callback tests can be un-skipped
- 2 C API source tests can be un-skipped
- New test: `ManySourcesScalesPast63` — the raison d'etre of the IOCP rewrite

### Header Changes

```cpp
#elif defined(_WIN32)
    void *m_pollHandle = nullptr;   // IOCP handle (was: sentinel event)
    // m_wakeupHandle removed — wakeup via PostQueuedCompletionStatus
    std::vector<void*> m_retiredContexts;  // OverlappedContext* awaiting cleanup
#endif
```

### Implementation Sequence

1. Header changes (`RunLoop.h`) — `platformCtx`, remove `m_wakeupHandle`, add `m_retiredContexts`
2. `init()` / destructor — create IOCP, full destruction ordering (see above)
3. `wakeup()` / `executeOnRunLoop()` / `stop()` — `PostQueuedCompletionStatus`
4. `addSource()` / `removeSource()` + `run()` loop — **develop together** (probe reads
   produce completions only consumable by `run()`; testing step 4 in isolation is
   impossible without a minimal run loop). Use `GetQueuedCompletionStatusEx` for batch.
5. Timers — `CreateThreadpoolTimer` + `PostQueuedCompletionStatus`
6. Tests — rewrite `makePipe()`, un-skip error tests, add scaling test

> **REVIEW FIX:** Original sequence had addSource (step 4) and run() (step 6) as
> separate phases. They have a circular dependency and must be co-developed.

### Risks

- **Zero-byte read quirks** — supported on Windows 8+ for `FILE_FLAG_OVERLAPPED` named
  pipes. Anonymous pipes (`CreatePipe`) do NOT support overlapped I/O. **Assumption:
  all pipes passed to addSource are byte-mode** (`PIPE_TYPE_BYTE`). Message-mode pipes
  would need `ERROR_MORE_DATA` handling that this design does not provide.
- **IOCP association is permanent** — a handle can only be associated with one IOCP.
  Fine for our use case (one RunLoop per handle).
- **Threadpool timer race** — stale `PostQueuedCompletionStatus` after `removeTimer`.
  Mitigated by checking `m_timers.find(tid)` before dispatch. Safe because
  `m_nextTimerId` is monotonically increasing (never reuses IDs). **Document this
  invariant** — if someone resets the counter, the stale-packet guard breaks.
- **`FILE_SKIP_COMPLETION_PORT_ON_SUCCESS` optimization** — when a zero-byte `ReadFile`
  completes synchronously (data already available), the completion is still posted to
  IOCP by default. Call `SetFileCompletionNotificationModes(handle,
  FILE_SKIP_COMPLETION_PORT_ON_SUCCESS)` on each source handle. The run loop must then
  check `ReadFile` return: if it succeeds synchronously, dispatch the handler directly.
  Important for low-latency IPC.
- **C API signature mismatch** — `vortex_add_source(int fd)` uses 32-bit `int`, but
  Windows `HANDLE` is 64-bit on Win64. The C API needs either a `vortex_add_source_handle`
  variant or a typedef that adapts per-platform. Two C API tests are already skipped.

---

## Team 2: Aether Named Pipe Overlapped I/O

### Problem

Aether's Windows platform layer (`PlatformWindows.cpp`) uses synchronous (blocking)
pipe reads. `WaitForMultipleObjects` can't poll named pipes for readability. To
integrate with the RunLoop, each pipe needs a **persistently pending overlapped read**
whose `OVERLAPPED.hEvent` (a manual-reset event handle) is registered with
`RunLoop::addSource()`.

### How It Works

```
Pipe handle ──► overlapped ReadFile() ──► OVERLAPPED.hEvent ──► RunLoop::addSource()
                                              │
                    When data arrives: ◄──────┘
                    1. Event signals
                    2. RunLoop fires handler
                    3. Handler calls completeAsyncRead() (non-blocking)
                    4. Handler processes frames from ring buffer
                    5. Re-arms next overlapped ReadFile()
```

### New Platform API (Windows-only, added to Platform.h)

```cpp
#if defined(_WIN32)
    // Start an overlapped read. Returns event handle for RunLoop::addSource().
    // Calling twice on the same handle is an error (returns kInvalidHandle).
    // The caller must call completeAsyncRead or closeFd before re-starting.
    Handle beginAsyncRead(Handle sockFd);

    // Complete the read (non-blocking), drain data, ResetEvent, re-arm next read.
    // MUST call ResetEvent on the manual-reset event before re-arming ReadFile,
    // otherwise the event stays signaled and WFMO fires the handler in a busy loop.
    // Returns 0 on success, -1 on broken pipe.
    int completeAsyncRead(Handle sockFd);

    // Start an overlapped ConnectNamedPipe. Returns event handle for RunLoop.
    // *pipeHandle receives the pipe instance being waited on.
    // MUST handle ERROR_PIPE_CONNECTED: if a client connected before
    // ConnectNamedPipe was called, the event is never signaled. In this case,
    // manually SetEvent to trigger the handler, or return a sentinel value
    // to indicate synchronous completion.
    Handle beginAsyncAccept(Handle listenToken, Handle *pipeHandle);

    // Complete the accept. Returns accepted pipe handle or kInvalidHandle.
    Handle completeAsyncAccept(Handle listenToken);
#endif
```

### New Structures

```cpp
// Per-connection overlapped state (heap-allocated, tracked in global map)
struct OverlappedReadContext {
    OVERLAPPED ov{};           // The overlapped structure
    uint8_t buffer[64];        // Read buffer for signal bytes
    DWORD bytesRead = 0;
    bool pending = false;
};

// Per-listener accept state
struct OverlappedAcceptContext {
    OVERLAPPED ov{};
    HANDLE pendingPipe = INVALID_HANDLE_VALUE;
    bool pending = false;
};
```

### Files Changed

| File | Change |
|------|--------|
| `inc/Platform.h` | 4 new function declarations (`#if defined(_WIN32)`) |
| `src/PlatformWindows.cpp` | Overlapped context structs, 4 new functions, cleanup in `closeFd()` |
| `inc/ServiceBase.h` | Add `m_acceptEventHandle` + `readEventHandle` per `ClientConn` (Win32 only) |
| `src/ServiceBase.cpp` | Remove rejection guard (lines 25-33), wire async accept + read into RunLoop |
| `inc/ClientBase.h` | Add `m_readEventHandle` (Win32 only) |
| `src/ClientBase.cpp` | Remove rejection guard (lines 24-31), wire async read into RunLoop |

### ServiceBase RunLoop Wiring (Windows)

**start() — accept loop:**
```cpp
if (m_loop) {
    Handle pipeHandle = platform::kInvalidHandle;
    auto acceptEvent = platform::beginAsyncAccept(m_listenFd, &pipeHandle);
    m_pendingAcceptPipe = pipeHandle;
    m_loop->addSource(acceptEvent, [this] { onAcceptReady(); });
}
```

**onAcceptReady() — new client:**
```cpp
void ServiceBase::onAcceptReady() {
    Handle pipeHandle = platform::completeAsyncAccept(m_listenFd);
    // Re-arm for next client (reuses same event handle — must ResetEvent internally)
    platform::beginAsyncAccept(m_listenFd, &m_pendingAcceptPipe);

    // After handshake, monitor client pipe for signals
    auto readEvent = platform::beginAsyncRead(client->conn.socketFd);
    client->readEventHandle = readEvent;
    m_loop->addSource(readEvent,
        [this, client] { onClientReady(client); },
        [this, client] { removeClient(client); });
}
```

**stop() — Windows-specific teardown (MUST use event handles, not socket fds):**
```cpp
void ServiceBase::stop() {
    // ... signal m_running = false ...
    if (m_loop) {
#if defined(_WIN32)
        // On Windows, m_listenFd is a synthetic token, NOT a waitable handle.
        // Must remove the accept EVENT handle, not m_listenFd.
        m_loop->removeSource(m_acceptEventHandle);
        platform::CancelIoEx(m_pendingAcceptPipe, nullptr);

        // Remove each client's read event handle, not socketFd
        for (auto& client : m_clients) {
            m_loop->removeSource(client->readEventHandle);
            // CancelIoEx + closeFd per teardown order above
        }
#else
        m_loop->removeSource(m_listenFd);
        for (auto& client : m_clients)
            m_loop->removeSource(client->conn.socketFd);
#endif
    }
}
```

> **REVIEW FIX (D3/D6):** Original didn't show Windows stop()/disconnect() paths.
> On Windows, `m_listenFd` is `kTokenTag | counter` (a synthetic token), NOT a
> waitable handle. removeSource must use `m_acceptEventHandle` / `readEventHandle`.
> Same issue in ClientBase::disconnect() — use `m_readEventHandle`, not `socketFd`.

**onClientReady() — data available:**
```cpp
// Instead of platform::recvSignal() (blocking):
platform::completeAsyncRead(client->conn.socketFd);
// Then drain frames from ring buffer (same as Linux path)
```

### ClientBase RunLoop Wiring (Windows)

**connect():**
```cpp
if (m_loop) {
    auto readEvent = platform::beginAsyncRead(m_conn.socketFd);
    m_readEventHandle = readEvent;
    m_loop->addSource(readEvent, [this] { onDataReady(); });
}
```

**onDataReady():**
```cpp
platform::completeAsyncRead(m_conn.socketFd);  // instead of recvSignal()
// drain frames...
```

### Teardown Order (critical)

1. `removeSource(eventHandle)` — deregister from RunLoop
2. Acquire `handlerMutex` — wait for any in-flight handler to complete.
   Without this, the handler may be executing `completeAsyncRead` on the pipe
   while we close it (use-after-free). Linux code already does this
   (ServiceBase.cpp:279, ClientBase.cpp:288).
3. `CancelIoEx(pipeHandle, nullptr)` — cancel pending overlapped I/O
4. Wait for cancellation: either `GetOverlappedResult(pipeHandle, &ov, &bytes, TRUE)`
   or defer to the retired-context pattern (free on `ERROR_OPERATION_ABORTED`
   dequeue). The OVERLAPPED must remain valid until the kernel is done with it.
5. `closeFd(pipeHandle)` — closes pipe. `closeFd()` must also:
   - Close the event handle inside `OverlappedReadContext`
   - Remove the entry from the global context map
   - If step 4 used deferred cleanup, the context is freed when the cancellation
     completion arrives (same pattern as Team 1's `m_retiredContexts`)
6. Release `handlerMutex`

> **REVIEW FIX (D2):** Original omitted handlerMutex. If WFMO already returned and
> the handler is executing, CancelIoEx + closeFd while handler runs = crash.
>
> **REVIEW FIX (M4):** Original said "update closeFd()" without specifying cleanup.
> Now explicit: close event handle, remove context map entry, handle async cancel.

### What Doesn't Change

- `sendSignal()` — already async, works in both modes. Creates a stack-local
  `OVERLAPPED` with its own event; concurrent with the persistent overlapped read.
- `recvSignal()` — preserved for threaded (non-RunLoop) path. **Must NEVER be
  called in RunLoop mode** — it uses a temporary overlapped read that can collide
  with the persistent one from `beginAsyncRead`, delivering completions to the
  wrong `OVERLAPPED`. Document as threaded-mode-only.
- `acceptClient()` — preserved for threaded path
- Handshake — still synchronous. **WARNING:** `kSocketTimeoutMs = 5000` means a
  malicious or hung client that connects but never completes the handshake blocks
  the RunLoop thread for up to 5 seconds. All other sources (client reads, timers,
  accept re-arms) are frozen during this window. **Mitigation options:**
  1. Reduce `kSocketTimeoutMs` to 200ms for RunLoop path
  2. Run handshake on a thread pool, call `executeOnRunLoop` when done
  3. Accept the risk (simple, but creates a DoS vector)

> **REVIEW FIX (D1):** Original said "sub-ms, acceptable." Actual timeout is 5 seconds
> (kSocketTimeoutMs in Connection.cpp:43). This is a DoS vector on the RunLoop thread.

### Limitations

- **Client limit is a shared budget, not a fixed number.** WFMO supports 64 handles.
  The RunLoop uses: 1 wakeup event + 1 accept source + N client sources + M timers.
  Maximum clients = `62 − active_timers`. With no timers: 62 clients (not 61 — the
  original plan double-counted the wakeup). With 2 timers: 60 clients.
  `setMaxClients()` should enforce `min(requested, 62 - active_timers)` dynamically,
  NOT a hardcoded 61. The IOCP rewrite (Team 1) eliminates this budget entirely.
- **WFMO starvation** — `WaitForMultipleObjects` returns the lowest-index signaled
  handle. If low-indexed sources are chatty, high-indexed clients starve. This is
  a known WFMO limitation. Mitigation: after dispatching one source, check remaining
  source events with `WaitForSingleObject(h, 0)` before re-entering WFMO. The IOCP
  rewrite eliminates this by using a FIFO completion queue.
- **onError callbacks do NOT fire on WFMO.** The WFMO backend only signals when the
  event handle is set — it cannot distinguish data from error. Client disconnect is
  detected inside the handler via `completeAsyncRead` returning -1. The `onError`
  callback parameter in `addSource` is dead code on Windows until IOCP. The 3 error
  callback tests must remain skipped.
- **Handshake blocks RunLoop thread** — see "What Doesn't Change" above for timeout
  analysis and mitigation options.

> **REVIEW FIX (B1/D12):** Original said "61-client limit." Actual math: 64 − 1
> (wakeup) − 1 (accept) = 62, minus active timers. Timers share the same WFMO array
> but `addSource()` doesn't check for them — a pre-existing Vortex bug. Add combined
> `sources + timers + 1 ≤ MAXIMUM_WAIT_OBJECTS` check in both addSource and addTimer.
>
> **REVIEW FIX (D4):** Original assumed error callbacks work under WFMO. They don't.
> WFMO can only signal the event; it can't distinguish error from data.
>
> **REVIEW FIX (D10):** WFMO starvation not mentioned in original plan.

### Implementation Sequence

1. `PlatformWindows.cpp` — new structs and 4 functions, update `closeFd()`
2. `Platform.h` — new declarations
3. `ServiceBase.h` / `ClientBase.h` — new Win32 members
4. `ServiceBase.cpp` — remove guard, wire RunLoop
5. `ClientBase.cpp` — remove guard, wire RunLoop
6. Test on Windows CI

---

## Team 3: Vortex CI Matrix

### Current State

CI runs on Linux only (ubuntu-latest) with GCC and Clang. No macOS or Windows CI.

### Recommended CI Structure

Single `ci.yml` with 4 parallel jobs:

| Job | Runner | Compiler | Notes |
|-----|--------|----------|-------|
| `linux-build` | ubuntu-latest | GCC + Clang (matrix) | Already exists |
| `linux-sanitizers` | ubuntu-latest | GCC | New: ASan+UBSan, TSan |
| `macos-build` | macos-latest | AppleClang | New |
| `windows-build` | windows-latest | MSVC | New |

### Platform-Specific Build Commands

**Linux (unchanged):**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

**macOS:**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j3
ctest --test-dir build --output-on-failure --timeout 30
```

**Windows (multi-config generator):**
```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug --parallel
ctest --test-dir build --build-config Debug --output-on-failure --timeout 30
```

Key: `CMAKE_BUILD_TYPE` is ignored by VS generator. Must use `--config Debug` at
build time and `--build-config Debug` at test time.

### Sanitizer Strategy

- **Linux only** — ASan+UBSan and TSan (two matrix entries)
- **macOS** — skip (Apple TSan unreliable on GitHub CI)
- **Windows** — skip (MSVC `/fsanitize=address` incompatible with GoogleTest)

Requires adding `VORTEX_SANITIZERS` CMake variable (same pattern as aether's
`AETHER_SANITIZERS`).

### Required Fixes Before CI Works

1. ~~**`test/CMakeLists.txt`** — wrap `find_package(Threads REQUIRED)` in `if(NOT WIN32)`.~~
   **DROPPED (REVIEW FIX B7):** `find_package(Threads REQUIRED)` works fine on MSVC —
   CMake's `FindThreads` resolves to a no-op imported target (`Threads::Threads` with
   no link flags) because threading is built into the Windows CRT. Wrapping in
   `if(NOT WIN32)` would **break** the build: `target_link_libraries` references
   `Threads::Threads` unconditionally. Leave as-is.

2. **Root `CMakeLists.txt`** — add `VORTEX_SANITIZERS` option with validation, compile
   flags (`-fsanitize=`), and MSVC guard.

3. ~~**Windows test behavior** — source-based tests use `CreatePipe` handles. These ARE
   waitable kernel objects.~~
   **CORRECTED (REVIEW FIX B6):** `CreatePipe` handles are **NOT** waitable for
   readability via `WaitForMultipleObjects`. The Vortex backend itself documents this
   at `backend_win32.cpp:16-19`: *"Pipes and sockets are NOT waitable for readability
   via WaitForMultipleObjects."* The 6 pipe-based source tests (`AddAndRemoveSource`,
   `SourceCallbackRunsOnLoopThread`, `MultipleSourcesConcurrent`,
   `RemoveSourceFromHandler`, `UpdateSourceHandler`, `AddSourceFromAnyThread`) will
   **hang** on Windows. They must get `GTEST_SKIP()` guards on Win32, matching the
   existing pattern at RunLoopTest.cpp:877-878.

4. **Add `GTEST_SKIP()` to pipe-based source tests on Win32** (new item):
   - `RunLoopTest::AddAndRemoveSource`
   - `RunLoopTest::SourceCallbackRunsOnLoopThread`
   - `RunLoopTest::MultipleSourcesConcurrent`
   - `RunLoopTest::RemoveSourceFromHandler`
   - `RunLoopTest::UpdateSourceHandler`
   - `RunLoopTest::AddSourceFromAnyThread`
   - `CApiTest::AddSourceWithErrorCallback` (compile-time `#if` guard, not GTEST_SKIP)

### Test Skip Handling

Existing tests that skip on Windows via `GTEST_SKIP()`:
- `RunLoopTest::ErrorCallbackOnPeerClose`
- `RunLoopTest::NoErrorCallbackWithoutOnError`
- `RunLoopTest::ErrorCallbackAutoRemovesSource`
- `CApiTest::AddRemoveSource`
- `CApiTest::SourceCallback`

(5 tests, not 3 as originally stated — Review Fix M10.)

After adding pipe-based source skips, 6 more tests skip on Windows (12 total
skipped). These show as "skipped" in CTest output — no CI changes needed.

### Implementation Sequence

1. Add `GTEST_SKIP()` to 6 pipe-based source tests on Win32 + convert
   `CApiTest::AddSourceWithErrorCallback` from `#if` guard to `GTEST_SKIP()`
2. Add `VORTEX_SANITIZERS` to root `CMakeLists.txt`
3. Rewrite `.github/workflows/ci.yml` with 4 jobs (include `submodules: recursive`
   in checkout step for GoogleTest submodule)
4. Validate on feature branch
5. Merge

Estimated CI time: ~10–15 minutes with queuing (all jobs parallel).

> **REVIEW FIX (B7):** Removed `find_package(Threads)` guard — it's unnecessary and
> would break the build. **REVIEW FIX (B6):** Added pipe-test skip step.

---

## Team 4: Testing Strategy

### Current Test State

**Vortex:** 25 C++ tests + 16 C API tests. 3 RunLoopTest + 2 CApiTest skip on Win32
via `GTEST_SKIP()`, plus 1 CApiTest (`AddSourceWithErrorCallback`) behind `#if` guard.

> **REVIEW FIX (M10):** Original said 24+14=38. Actual: 25+16=41.

**Aether:** All RunLoop-mode tests behind `#if !defined(_WIN32)` with stub
`RunLoopModeNotSupportedOnWindows` tests on the else branch. Files affected:
- `test/ServiceBaseTest.cpp` — 8 RunLoop tests guarded
- `test/ClientBaseTest.cpp` — 5 RunLoop tests guarded
- `examples/getting-started/echo/CodeGenServerTest.cpp` — 1 RunLoop test guarded
- `examples/getting-started/echo/CodeGenClientTest.cpp` — 1 RunLoop test guarded

### Phase-by-Phase Test Plan

#### After F0 (Vortex IOCP Rewrite)

**Prerequisite:** Rewrite Win32 `makePipe()` in both test files to create overlapped
named pipe pairs instead of `CreatePipe` (anonymous pipes can't do IOCP).

**Un-skip 6 tests:**
- `RunLoopTest::ErrorCallbackOnPeerClose`
- `RunLoopTest::NoErrorCallbackWithoutOnError`
- `RunLoopTest::ErrorCallbackAutoRemovesSource`
- `CApiTest::AddRemoveSource`
- `CApiTest::SourceCallback`
- `CApiTest::AddSourceWithErrorCallback` (convert from `#if` guard to un-skip)

> **REVIEW FIX:** Original missed `AddSourceWithErrorCallback` (compile-time guard,
> not GTEST_SKIP). 6 un-skipped, not 5.

**New IOCP-specific tests for Vortex:**

| Test | Purpose |
|------|---------|
| `NamedPipeReadReady` | addSource on named pipe, write byte, verify handler fires |
| `NamedPipeMultipleWrites` | Rapid writes, verify handler fires for each |
| `NamedPipeRemoveSourceCancelsOverlapped` | addSource then removeSource, verify no stale completion |
| `ManySourcesScalesPast63` | 100 named pipe sources, write to all, verify all handlers fire. **The gating test for IOCP.** |
| `ManySourcesAddRemoveDynamic` | Start 10, add 90, remove 50, verify remaining 50 work |
| `StopCancelsAllPendingOverlapped` | 10 sources with pending reads, stop(), verify clean cancellation |
| `RemoveSourceDuringPendingRead` | Verify CancelIoEx called, no callback fires |
| `DestructorCancelsPendingIO` | Destroy RunLoop with pending reads, no use-after-free |
| ~~`PipeDisconnectFiresErrorCallback`~~ | ~~Close write end, verify onError fires~~ **DROPPED — redundant with un-skipped `ErrorCallbackOnPeerClose`** |
| ~~`PipeDisconnectAutoRemovesSource`~~ | ~~Close write end, verify source auto-removed~~ **DROPPED — redundant with un-skipped `ErrorCallbackAutoRemovesSource`** |
| `TimerAndSourcesMixed` | 3 pipe sources + 2 timers, verify both work via IOCP |
| `StopRacesAddSource` | stop() racing with addSource() from another thread — real-world accept-loop pattern |

> **REVIEW FIX (M9):** Removed 2 redundant tests. Added StopRacesAddSource (M5).

#### After F1-F3 (Aether Overlapped I/O + RunLoop Wiring)

**Remove `#if !defined(_WIN32)` guards from RunLoop tests.**

> **CAUTION (REVIEW FIX D9):** In `ClientBaseTest.cpp`, the `#if !defined(_WIN32)` block
> extends beyond RunLoop tests and captures 4 non-RunLoop tests:
> `ConcurrentCallsFromSameClient`, `ConnectToNonexistentService`, `ConnectTwiceFails`,
> `ReconnectAfterServerDisconnect`. Before removing the guard, either:
> (a) move these 4 tests above the guard now, or
> (b) verify they pass on Windows (they use UDS-based IPC, not RunLoop).

**Delete all `RunLoopModeNotSupportedOnWindows` stub tests from:**
- `test/ServiceBaseTest.cpp`
- `test/ClientBaseTest.cpp`
- `examples/getting-started/echo/CodeGenServerTest.cpp`
- `examples/getting-started/echo/CodeGenClientTest.cpp`

**Existing tests that become cross-platform (15 total):**

ServiceBase (8):
- `RunLoop_AcceptAndRespond`
- `RunLoop_MultipleClients`
- `RunLoop_ClientDisconnect`
- `RunLoop_StopCleansUp`
- `RunLoop_StopAfterClientMarkedDead`
- `RunLoop_DeadClientsReapedOnAccept`
- `RunLoop_ManyConnectDisconnectCycles`
- `RunLoop_MaxClientsRejectsExcess`

ClientBase (5):
- `RunLoop_CallAndResponse`
- `RunLoop_Notification`
- `RunLoop_Disconnect`
- `RunLoop_BothOnSameRunLoop`
- `RunLoop_ConnectTwiceFails`

CodeGen (2):
- `CodeGenServerTest::RunLoop_Dispatch`
- `CodeGenClientTest::RunLoop_TypedCall`

**New Windows-specific aether tests:**

| Test | Purpose |
|------|---------|
| `RunLoop_AcceptOverlapped` | Verify accept uses overlapped ConnectNamedPipe |
| `RunLoop_ClientSignalViaOverlapped` | Verify data-ready via IOCP, not blocking recvSignal |
| `RunLoop_StopCancelsAcceptOverlapped` | stop() cancels pending overlapped accept cleanly |
| `RunLoop_CallTimeout` | RPC timeout through RunLoop (cross-platform, not Windows-specific) |
| `RunLoop_ServerStopDisconnectsClient` | Server stops, client detects via IOCP error |

**Additional tests identified by cross-review (new):**

| Test | Purpose |
|------|---------|
| `RunLoop_ConcurrentCallsFromSameClient` | Multi-threaded `call()` under RunLoop dispatch — tests m_sendMutex + condvar interaction |
| `RunLoop_ReconnectAfterServerDisconnect` | Reconnection under RunLoop — IOCP requires re-registering overlapped I/O after handle replacement |
| `Stress_RunLoop_RingBackpressure` | Client floods 256KB ring while server dispatches via RunLoop — tests flow control under event-driven dispatch |

> **REVIEW FIX (M8):** No RunLoop variants of ConcurrentCalls or Reconnect existed.
> No ring-buffer backpressure test under RunLoop existed.

#### After F4 (Integration + Stress)

**End-to-end tests** (new file `test/IntegrationTest.cpp`):

| Test | Purpose |
|------|---------|
| `E2E_RunLoop_EchoRoundTrip` | Full stack: named pipe + overlapped I/O + IOCP + SHM rings + frame I/O |
| `E2E_RunLoop_NotificationRoundTrip` | Service broadcasts notification, client receives via RunLoop |
| `E2E_RunLoop_BothOnSameLoop` | Service + client share one RunLoop, no deadlock |
| `E2E_RunLoop_MultipleServicesOneLoop` | Two services on one RunLoop, two clients |

**Stress tests:**

| Test | Purpose |
|------|---------|
| `Stress_RunLoop_50ConcurrentClients` | 50 clients × 100 echo requests = 5000 round-trips |
| `Stress_RunLoop_RapidConnectDisconnect` | 100 connect/disconnect cycles, no resource leaks |
| `Stress_RunLoop_NotificationFanout` | 1 service, 50 clients, 100 notifications (reduced from 1000 — 50×1000=50K deliveries would timeout on Windows CI runners; use `STRESS` env var for full suite) |

**Error scenario tests:**

| Test | Purpose |
|------|---------|
| `Error_PipeDisconnectMidRequest` | Client sends request, server pipe closes, verify IPC_ERR_DISCONNECTED |
| `Error_OverlappedCancelOnShutdown` | Stop RunLoop with 10 pending reads, verify clean cleanup |
| `Error_HandleClosedExternally` | Close pipe externally, verify graceful error handling |
| `Error_StopFromErrorCallback` | Error callback calls loop.stop(), no deadlock |
| `Error_ServerStopWhileClientBlocked` | svc.stop() while client.call() blocked, verify timeout |

### Shared Test Infrastructure

**Extract to `test/TestPipeHelpers.h`:**
- `makePipe()` — platform-specific pipe pair creation
- `writeByte()` / `drainPipe()` / `closePipe()` — portable helpers
- Currently duplicated across 2 test files (RunLoopTest.cpp, CApiTest.cpp), but more
  test files will need it after Phase 5. Original plan said "5+ files" which was
  conflating with RunLoopGuard duplication.

**Extract to `test/RunLoopTestHelpers.h`:**
- `RunLoopGuard` RAII helper — currently duplicated in 5 files

### Test Count Summary (corrected)

| Phase | New Tests | Un-skipped | Total Added |
|-------|-----------|------------|-------------|
| F0 (Vortex IOCP) | ~9 | 6 | ~15 |
| F1-F3 (Aether overlapped) | ~8 | 15 | ~23 |
| F4 (Integration + stress) | ~12 | 0 | ~12 |
| **Total** | **~29** | **21** | **~50** |

> **REVIEW FIX:** Removed 2 redundant Vortex tests (−11 → 9), added 1 un-skip (+6
> vs +5), added 3 missing cross-platform tests (+5 → 8), fixed Vortex baseline
> (41 tests, not 38).

---

## Sequencing Decision: IOCP First or Overlapped I/O First?

Team 2's overlapped I/O plan works with the **current** WFMO-based Vortex backend
because event handles are waitable kernel objects. This gives us two options:

### Option A: IOCP First (F0 → F1 → F2 → F3 → F4)

Rewrite Vortex Win32 backend to IOCP, then do aether overlapped I/O. Eliminates the
63-handle limit from day one. More work upfront. Team 1 estimates 7 implementation
phases: header changes, init/destructor, wakeup, addSource/removeSource, timers,
run() loop, tests. Touches RunLoop.h API (adds `platformCtx` to `SourceEntry`).

### Option B: Overlapped I/O First (F1 → F2 → F3 → F0 → F4)

Do aether overlapped I/O against the current WFMO backend. RunLoop works on Windows
immediately with a ~62-client limit (minus active timers). IOCP rewrite comes later.
Team 2's plan is self-contained in aether's platform layer — no Vortex API changes.

### Recommendation

**Option B** — get RunLoop working on Windows sooner with the ~62-client cap. For most
real-world aether deployments (desktop IPC), 62 concurrent clients per service is more
than enough. The IOCP rewrite is a Vortex-internal change that doesn't affect aether's
API, so it can be done later without breaking anything.

**However**, IOCP is needed not just for scale but for **error callback parity** —
`onError` callbacks cannot fire distinctly from normal handlers under WFMO. This means
the RunLoop programming model is subtly different on Windows vs. Linux/macOS. IOCP
should be completed before v1.3.0 to achieve the vision doc's "primary dispatch model"
goal with consistent cross-platform semantics.

When IOCP is done later, Team 2's `beginAsyncRead`/`completeAsyncRead` API stays the
same — only the internal implementation changes (event handle → IOCP completion).
Aether code doesn't change at all.

### Revised Phase 5 Order

```
Phase 5a — Do now (parallel):
  F0c: Vortex CI (macOS + Windows)                    ← independent
  F1:  Aether overlapped I/O in PlatformWindows.cpp   ← Team 2's plan

Phase 5b — After F1:
  F2+F3: Wire into RunLoop + remove rejection guard   ← SINGLE PR (guard removal
         without wiring is dead code; wiring without guard removal is untestable)
  F4:    RunLoop integration tests on Windows CI       ← Team 4's test plan

Phase 5c — Before v1.3.0 (not just "when limit becomes a problem"):
  F0:  Vortex IOCP rewrite                            ← Team 1's plan
  F0b: Vortex Win32 error callbacks via IOCP
  Trigger: error callback parity across platforms, not just 62-client limit
```

> **REVIEW FIX (R4):** F2 and F3 merged into single PR — guard removal without wiring
> is dead code; wiring without guard removal is untestable.
>
> **REVIEW FIX (R8):** IOCP promoted from "future" to "before v1.3.0" — error callback
> parity is a release-quality requirement for "primary dispatch model" claim.

---

## Decisions Made

1. **Option B chosen** — overlapped I/O first, IOCP later.
2. **Client limit is a shared budget** (62 − active_timers), not a fixed 61.
   `setMaxClients()` should enforce dynamically. Add combined
   `sources + timers + 1 ≤ MAXIMUM_WAIT_OBJECTS` check in Vortex.
3. **Handshake stays synchronous but needs DoS mitigation** — actual timeout is
   5 seconds (kSocketTimeoutMs), not "sub-ms." Reduce to 200ms for RunLoop path
   or run handshake off-thread. Decision needed from owner.
4. **IOCP rewrite targeted for pre-v1.3.0** — needed for error callback parity
   across platforms, not just the client limit. Team 1's plan is preserved.
5. **CI first** — F0c (Vortex CI for macOS + Windows) is independent and should
   start immediately so we have CI coverage before the overlapped I/O work lands.
6. **Minimum Windows 10** — CancelIoEx requires Vista+, zero-byte reads Win8+.
   Align with Microsoft support lifecycle. Document in README and CMake.
7. **F2+F3 merged into single PR** — guard removal without wiring is dead code.
8. **3 error callback tests stay skipped** until IOCP — WFMO can't fire onError.

---

## Open Decisions (owner input needed)

1. **Handshake DoS mitigation:** reduce kSocketTimeoutMs to 200ms for RunLoop path,
   or async handshake on thread pool?
2. **C API handle size:** break `vortex_add_source(int fd)` ABI now for Win64
   HANDLE, or add `vortex_add_source_handle()` variant?
3. **Timer slot reservation:** dynamic combined check only, or also reserve N
   slots for timers in the MAX_SOURCES budget?
---

## Cross-Review Addendum

Five independent expert teams reviewed this plan against the actual codebase. Their
findings resulted in the `REVIEW FIX` annotations throughout this document.

### Review Teams

| # | Expert | Focus | Key Findings |
|---|--------|-------|-------------|
| 1 | IOCP Expert | Team 1 IOCP design | 3 bugs (retired-ctx lifecycle, CancelIoEx leak, re-arm order), batch dequeue |
| 2 | Overlapped I/O Expert | Team 2 async pipes | Handshake DoS (5s!), teardown race, onError dead on WFMO, ResetEvent |
| 3 | CI Expert | Team 3 CI matrix | CreatePipe NOT waitable, Threads guard breaks build, 6 more tests need skip |
| 4 | Testing Expert | Team 4 test strategy | 2 redundant tests, 4 trapped non-RunLoop tests, 3 missing scenarios |
| 5 | Architecture Expert | Cross-team + sequencing | WFMO slot budget bug, F2+F3 must merge, IOCP needed for error parity |

### Summary of Changes

**P0 fixes (7):** retired context lifecycle, CancelIoEx error handling, ResetEvent
before re-arm, probe re-arm after dispatch, CreatePipe claim corrected, Threads guard
dropped, GQCS→GQCSEx batch dequeue.

**P1 fixes (10):** handshake DoS documented, teardown handlerMutex, Windows stop()/
disconnect() paths shown, onError dead on WFMO documented, WFMO starvation noted,
m_listenFd token clarified, ERROR_PIPE_CONNECTED race noted, context map locking,
trapped non-RunLoop tests flagged, recvSignal threaded-only.

**Test changes:** 2 redundant tests removed, 1 un-skip added, 3 missing scenarios
added, stress parameters reduced for CI, makePipe duplication claim corrected.

**Sequencing changes:** F2+F3 merged to single PR, IOCP promoted to pre-v1.3.0,
implementation sequence reordered (addSource + run() co-developed).
