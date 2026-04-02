# Adoption Guide

Use this guide when you are past "can I run it?" and are deciding whether
Aether fits a real application.

The goal is not to restate the full design docs. The goal is to make the
operational model, guarantees, and common traps easy to evaluate quickly.

## What You Can Rely On

- **Typed contract by default**: the recommended path is IDL -> generated C++
  client/server glue -> user-written business logic.
- **Single-host focus**: Aether is for processes on the same machine, and for
  desktop-to-device links over serial/USB. It is not a distributed RPC system.
- **Shared-memory hot path**: payloads move through shared memory ring buffers;
  wakeups happen over a small control channel.
- **Cross-platform runtime**: Linux, macOS, and Windows share the same public
  runtime model.
- **Warnings-as-errors in CI**: CI builds compile with warnings treated as
  errors, so new warnings fail the build.

## Threading Guarantees

- `ClientBase::call()` is synchronous and thread-safe. Multiple threads may call
  it concurrently on the same connected client.
- `ClientBase::connect()` / `disconnect()` are lifecycle operations, not
  concurrent throughput APIs. Call them deliberately from one controlling
  thread.
- In default threaded mode, service handlers run on per-client receiver
  threads. If multiple handlers touch shared state, you must synchronize it.
- In RunLoop mode, dispatch moves onto the RunLoop thread. Do not call
  synchronous `call()` from that same RunLoop thread; it prevents
  `onDataReady()` from running and the call will typically block until its
  timeout expires.

## Ownership And Lifetime

- The client owns the shared-memory connection setup and its receiver path.
- The service owns accepted client connections and their runtime cleanup.
- `disconnect()` and `stop()` are the normal shutdown boundaries. Do not rely on
  process teardown as your cleanup strategy.
- Generated code does not own your domain state. It only marshals, dispatches,
  and routes callbacks.

## Failure Semantics

- Disconnection is a normal runtime event. Treat negative return codes as part
  of the contract, not as exceptional edge cases.
- Timeouts usually mean a blocked handler, a deadlocked RunLoop usage pattern,
  or an unavailable peer.
- Notification fan-out is best-effort per client. If one client stops draining
  its ring, Aether disconnects that client rather than stalling healthy ones.
- After changing an `.idl` file, regenerate the full output set together.
  Partial regeneration causes avoidable mismatches.

## Anti-Patterns

- Calling `call()` from the RunLoop thread.
- Doing long blocking work directly inside service handlers.
- Ignoring return codes from RPC calls or notification sends.
- Choosing the raw API before the generated path without a concrete reason.
- Treating Aether like a cross-machine RPC framework.

## When Aether Is A Good Fit

Choose Aether when you want:

- a typed contract between local processes
- low-latency host IPC without a daemon in the middle
- one protocol shape that can extend to serial/USB device links
- generated glue that keeps business logic separate from framing details

Choose something else when you need:

- cross-machine networking
- public internet exposure
- service mesh / load balancing / distributed tracing semantics

## Read Next

- [Aether in 5 Minutes](AetherIn5Minutes.md) for the full runtime mental model
- [architecture-guide.md](architecture-guide.md) for the visual walkthrough
- [aether-lld.md](aether-lld.md) for detailed protocol and API behavior
