# Aether IPC — Vision & Platform Requirements

## What Aether Is

Aether is a cross-language RPC framework with IDL code generation. Write the service
contract once in IDL, get typed client and server stubs generated for C++, Python, and
C99 (MCU). RPC works over shared memory (same machine) and serial/USB
(desktop-to-device) — same wire protocol everywhere.

Aether is NOT a Python networking library. If you only need Python-to-Python, use
asyncio. If you need a Python GUI talking to a C++ service talking to an STM32 over
serial, with typed contracts generated from one IDL file — that's what aether does.

## Goal

RPC over "channels" between desktop and device, as well as between two applications
on the same desktop. Responsiveness comparable to existing tools, with the convenience
of IDL-generated code and minimal modifications to data access.

## Architecture Decisions

### Service host is C++

The service host is always C++ (or C99 on bare-metal MCU). This is the core runtime
and it runs everywhere — desktop (Linux, macOS, Windows), RTOS, and bare-metal.
There is one implementation, one codebase, one set of bugs to fix.

### Python is a client language

Python client code is generated from IDL as a **ctypes wrapper over libaether**
(the C API). This means:

- One runtime codebase (C++), not two
- Python gets the same performance as C++ for shared memory transport
- Every protocol fix or feature lands in C++ and Python gets it for free
- Desktop Python users (PySide6, etc.) get typed RPC with `pip install`

For serial/USB transport to devices, a thin pure-Python framing layer (~150 lines)
handles the 24-byte wire protocol + pyserial. This code is small and stable — it
just packs/unpacks frame headers.

### Why not a pure Python runtime?

Maintaining two separate codebases (C++ and Python) implementing the same wire
protocol, ring buffers, handshake, RPC correlation, and transport logic is a
maintenance burden. Every bug fix and protocol change must be done twice. The ctypes
approach eliminates this — Python wraps the C++ runtime.

### Why not asyncio?

asyncio is a Python event loop. Aether is a cross-language RPC framework. asyncio
can't generate your C++ service host, your MCU dispatch table, or your wire protocol.
They're different layers. The Python client could use asyncio internally for
serial I/O — they complement each other, they don't replace each other.

## Language Roles

| Language | Role | Generated from IDL |
|----------|------|--------------------|
| C++ | Service host + client | Server skeleton + client stubs |
| Python | Client (ctypes wrapper over C API) | Client stubs |
| C99 | Service host on MCU (aether-lite) | Dispatch table |

## Usage Scenarios

### Scenario 1: Two apps on the same desktop (shared memory)

```
Desktop                              Desktop
┌─────────────────┐   shared mem    ┌─────────────────┐
│ Python client   │◄──────────────►│ C++ service     │
│ (PySide6 GUI)   │   (SPSC rings)  │ host            │
└─────────────────┘                 └─────────────────┘
        OR
┌─────────────────┐   shared mem    ┌─────────────────┐
│ C++ client      │◄──────────────►│ C++ service     │
│ (Qt C++ GUI)    │   (SPSC rings)  │ host            │
└─────────────────┘                 └─────────────────┘
```

### Scenario 2: Desktop to device (serial / USB)

```
Desktop                              Device (RTOS / bare-metal)
┌─────────────────┐  serial/USB     ┌─────────────────┐
│ Python client   │────────────────►│ C++ service     │
│ (PySide6 GUI)   │                 │ host (full C++  │
└─────────────────┘                 │  or aether-lite)│
        OR                          └─────────────────┘
┌─────────────────┐  serial/USB
│ C++ client      │────────────────►
│ (Qt C++ GUI)    │
└─────────────────┘
```

### Scenario 3: Device to device (serial / USB)

```
Device A (Cortex-A)                  Device B (Cortex-M)
┌─────────────────┐  serial/USB     ┌─────────────────┐
│ C++ client      │────────────────►│ aether-lite     │
│ (full runtime)  │                 │ service host    │
└─────────────────┘                 └─────────────────┘
```

## Transports

| Transport      | When used | Description |
|----------------|-----------|-------------|
| Shared Memory  | Same machine | Lock-free SPSC ring buffers. UDS for handshake + wakeup only. Fastest. |
| Serial (UART)  | Desktop ↔ device | Byte stream over serial port. Same wire protocol. |
| USB            | Desktop ↔ device | USB CDC — appears as virtual serial port. Same wire protocol. |

All transports use the same 24-byte frame header. Only the delivery mechanism changes.

**No network transport.** For cross-machine RPC, use gRPC. Aether is for local IPC
(same machine, shared memory) and desktop-to-device communication (serial/USB).

## Platforms

| Platform              | Role | Description |
|-----------------------|------|-------------|
| Linux                 | Desktop / embedded | Full support (UDS, memfd, epoll, serial, USB) |
| macOS                 | Desktop | Full support (UDS, shm_open, kqueue, serial, USB) |
| Windows               | Desktop | Full support (named pipes, named file mappings, IOCP, COM ports) |
| RTOS — Cortex-M (lite)| Device | Bare-metal C99 dispatch table. No OS needed. Reads frames from UART/USB. |
| RTOS — Cortex-A/R     | Device | Full aether C++ runtime. Requires POSIX-like primitives from RTOS. |

## What the OS / RTOS Must Provide

### Shared Memory Transport (same-machine)

| Primitive             | Linux                    | macOS              | Windows                | RTOS (Cortex-A/R)       |
|-----------------------|--------------------------|--------------------|------------------------|--------------------------|
| Local socket          | UDS (abstract namespace) | UDS (pathname)     | Named pipes            | POSIX UDS or equivalent  |
| Shared memory region  | memfd_create             | shm_open           | Named file mapping     | POSIX shm or MPU region  |
| FD / handle passing   | SCM_RIGHTS               | SCM_RIGHTS         | DuplicateHandle        | OS-specific              |
| Event loop (RunLoop)  | epoll                    | kqueue             | IOCP                   | select/poll or custom    |
| Threading             | pthreads                 | pthreads           | Win32 threads          | RTOS threads             |

### Serial / USB Transport

| Primitive       | Linux/macOS              | Windows        | MCU (bare-metal)       |
|-----------------|--------------------------|----------------|------------------------|
| UART / USB      | File descriptor (termios)| COM port API   | HAL UART / USB CDC     |
| Framing         | 24-byte header + payload | Same           | Same                   |

## Key Concepts

### RunLoop (Vortex)

The Vortex project (`deps/vortex`) provides the RunLoop — an event loop built
specifically for aether. It uses epoll on Linux, kqueue on macOS, and IOCP
on Windows. RunLoop is the **primary dispatch model**: the server handles all clients
on a single thread by waiting for I/O events.

Thread-per-client mode exists as a **fallback** for simple deployments or
platforms where RunLoop integration isn't available.

### aether-lite

Standalone C99 runtime for microcontrollers. No heap, no OS, no threads. Static
dispatch table that reads frames from a byte stream (UART/USB), calls the handler,
writes the response. Generated by `ipcgen --backend aether_lite`.

### Wire Protocol

All transports use the same 24-byte frame header:

| Field          | Size    | Purpose                           |
|----------------|---------|-----------------------------------|
| version        | 1 byte  | Protocol version                  |
| flags          | 1 byte  | Request / Response / Notification |
| service_id     | 4 bytes | FNV-1a hash of service name       |
| message_id     | 4 bytes | Method or notification ID         |
| seq            | 4 bytes | Sequence number (for RPC pairing) |
| payload_bytes  | 4 bytes | Payload length                    |
| aux            | 4 bytes | Status code (in responses)        |
| reserved       | 2 bytes | Future use                        |

The wire protocol is identical regardless of transport. Only the delivery mechanism changes.
