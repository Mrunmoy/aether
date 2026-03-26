"""FrameHeader pack/unpack and frame-level read/write over SPSC rings."""

import struct
from .constants import (
    FRAME_HEADER_SIZE, FRAME_HEADER_FORMAT,
    FRAME_REQUEST, FRAME_RESPONSE, FRAME_NOTIFY,
    MAX_PAYLOAD, IPC_SUCCESS, IPC_ERR_RING_FULL, IPC_ERR_DISCONNECTED,
)

__all__ = ["FrameHeader", "write_frame", "peek_frame_header", "read_frame"]


class FrameHeader:
    """Mirror of the C++ aether::ipc::FrameHeader (24 bytes, native LE)."""

    __slots__ = ("version", "flags", "service_id", "message_id",
                 "seq", "payload_bytes", "aux")
    SIZE = FRAME_HEADER_SIZE

    def __init__(self, version=1, flags=0, service_id=0, message_id=0,
                 seq=0, payload_bytes=0, aux=0):
        self.version = version
        self.flags = flags
        self.service_id = service_id
        self.message_id = message_id
        self.seq = seq
        self.payload_bytes = payload_bytes
        self.aux = aux

    def pack(self) -> bytes:
        return struct.pack(
            FRAME_HEADER_FORMAT,
            self.version, self.flags, self.service_id,
            self.message_id, self.seq, self.payload_bytes, self.aux,
        )

    @classmethod
    def unpack(cls, data: bytes) -> "FrameHeader":
        if len(data) < FRAME_HEADER_SIZE:
            raise ValueError(f"need {FRAME_HEADER_SIZE} bytes, got {len(data)}")
        v, f, sid, mid, seq, pb, aux = struct.unpack(FRAME_HEADER_FORMAT, data[:FRAME_HEADER_SIZE])
        return cls(version=v, flags=f, service_id=sid, message_id=mid,
                   seq=seq, payload_bytes=pb, aux=aux)

    def __repr__(self):
        return (f"FrameHeader(version={self.version}, flags=0x{self.flags:04x}, "
                f"service_id={self.service_id}, message_id={self.message_id}, "
                f"seq={self.seq}, payload_bytes={self.payload_bytes}, aux={self.aux})")


# ---------------------------------------------------------------------------
# Frame I/O — mirrors FrameIO.h writeFrame / readFrame
# ---------------------------------------------------------------------------

def write_frame(ring, header: FrameHeader, payload: bytes = b"") -> int:
    """Write header + payload atomically to *ring* (SpscRingWriter).

    The header and payload are concatenated into a single ``ring.write()``
    call so that ``head`` is advanced only once, after ALL data is in the
    buffer.  Two separate writes would advance ``head`` after the header,
    allowing the consumer to see a partial frame.

    Returns IPC_SUCCESS or IPC_ERR_RING_FULL.
    """
    payload_len = len(payload)
    if payload_len > MAX_PAYLOAD:
        return IPC_ERR_RING_FULL

    # Ensure the serialized header always agrees with the actual payload
    # length, even if the caller passed a stale or mismatched value.
    header.payload_bytes = payload_len

    total = FRAME_HEADER_SIZE + payload_len
    if ring.write_available() < total:
        return IPC_ERR_RING_FULL

    # Single write: head is only advanced once, after both header and
    # payload are in the data region.
    frame_bytes = header.pack() + payload if payload_len > 0 else header.pack()
    if not ring.write(frame_bytes):
        return IPC_ERR_RING_FULL

    return IPC_SUCCESS


def peek_frame_header(ring) -> "FrameHeader | None":
    """Peek at the next header without consuming it.

    Returns a FrameHeader or None if not enough data.
    """
    if ring.read_available() < FRAME_HEADER_SIZE:
        return None
    data = ring.peek(FRAME_HEADER_SIZE)
    if data is None:
        return None
    return FrameHeader.unpack(data)


def read_frame(ring) -> "tuple[int, FrameHeader | None, bytes]":
    """Read a complete frame (header + payload).

    Returns (status, header, payload).
    """
    hdr = peek_frame_header(ring)
    if hdr is None:
        return (IPC_ERR_DISCONNECTED, None, b"")

    if hdr.payload_bytes > MAX_PAYLOAD:
        return (IPC_ERR_DISCONNECTED, None, b"")

    total = FRAME_HEADER_SIZE + hdr.payload_bytes
    if ring.read_available() < total:
        return (IPC_ERR_DISCONNECTED, None, b"")

    # Consume the header.
    ring.skip(FRAME_HEADER_SIZE)

    # Read payload.
    payload = b""
    if hdr.payload_bytes > 0:
        payload = ring.read(hdr.payload_bytes)
        if payload is None:
            return (IPC_ERR_DISCONNECTED, None, b"")

    return (IPC_SUCCESS, hdr, payload)
