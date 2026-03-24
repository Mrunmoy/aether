# Serial Loopback Example

Demonstrates `aether::ipc::ITransport` and `TransportClientBase` over a
PTY-based serial loopback.  No hardware required — the example creates a
pseudo-terminal pair and runs a simple echo server on the slave side.

## What it shows

- `SerialTransport` — a reusable POSIX serial transport implementing
  `ITransport` with the aether-lite wire format
  (`[sync][length][header][payload][CRC32]`).
- Creating a PTY pair with `openpty()` for zero-hardware testing.
- Plugging the transport into `TransportClientBase` for synchronous RPC.

## Building

```bash
cmake -B build -DAETHER_BUILD_EXAMPLES=ON
cmake --build build --target serial_loopback
```

## Running

```bash
./build/examples/serial-loopback/serial_loopback
```

Linux only (requires POSIX PTY and termios).
