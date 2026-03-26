"""Tests for FrameHeader pack/unpack and frame I/O."""

import pytest
from aether_ipc.constants import (
    FRAME_HEADER_SIZE, FRAME_REQUEST, FRAME_RESPONSE, FRAME_NOTIFY,
    IPC_SUCCESS, IPC_ERR_RING_FULL, IPC_ERR_DISCONNECTED, MAX_PAYLOAD,
)
from aether_ipc.framing import FrameHeader, write_frame, peek_frame_header, read_frame
from aether_ipc.ringbuffer import SpscRingWriter, SpscRingReader
from aether_ipc.constants import IPCRING_SIZE
import mmap


# ---------------------------------------------------------------------------
# FrameHeader pack / unpack
# ---------------------------------------------------------------------------

class TestFrameHeader:
    def test_size(self):
        assert FrameHeader.SIZE == 24

    def test_round_trip_defaults(self):
        hdr = FrameHeader()
        data = hdr.pack()
        assert len(data) == 24
        out = FrameHeader.unpack(data)
        assert out.version == 1
        assert out.flags == 0
        assert out.service_id == 0
        assert out.message_id == 0
        assert out.seq == 0
        assert out.payload_bytes == 0
        assert out.aux == 0

    def test_round_trip_values(self):
        hdr = FrameHeader(version=1, flags=FRAME_REQUEST, service_id=42,
                          message_id=7, seq=1000, payload_bytes=256, aux=-1)
        data = hdr.pack()
        out = FrameHeader.unpack(data)
        assert out.version == 1
        assert out.flags == FRAME_REQUEST
        assert out.service_id == 42
        assert out.message_id == 7
        assert out.seq == 1000
        assert out.payload_bytes == 256
        # aux is signed int32 -> -1 round trips
        assert out.aux == -1

    def test_unpack_too_short(self):
        with pytest.raises(ValueError):
            FrameHeader.unpack(b"\x00" * 20)

    def test_repr(self):
        hdr = FrameHeader(flags=FRAME_RESPONSE, service_id=1, seq=5)
        r = repr(hdr)
        assert "RESPONSE" not in r  # repr uses hex
        assert "0x0002" in r
        assert "service_id=1" in r

    def test_all_flag_values(self):
        for flag in (FRAME_REQUEST, FRAME_RESPONSE, FRAME_NOTIFY):
            hdr = FrameHeader(flags=flag)
            out = FrameHeader.unpack(hdr.pack())
            assert out.flags == flag


# ---------------------------------------------------------------------------
# Frame I/O over ring buffers
# ---------------------------------------------------------------------------

def _make_ring_pair():
    """Create a writer/reader pair backed by a single anonymous mmap region."""
    shm = mmap.mmap(-1, IPCRING_SIZE)
    writer = SpscRingWriter(shm, 0)
    reader = SpscRingReader(shm, 0)
    return shm, writer, reader


class TestFrameIO:
    def test_write_read_empty_payload(self):
        shm, writer, reader = _make_ring_pair()
        hdr = FrameHeader(version=1, flags=FRAME_REQUEST, service_id=1,
                          message_id=2, seq=10, payload_bytes=0)
        assert write_frame(writer, hdr) == IPC_SUCCESS
        status, out_hdr, payload = read_frame(reader)
        assert status == IPC_SUCCESS
        assert out_hdr.seq == 10
        assert out_hdr.service_id == 1
        assert payload == b""
        shm.close()

    def test_write_read_with_payload(self):
        shm, writer, reader = _make_ring_pair()
        body = b"Hello, aether!"
        hdr = FrameHeader(version=1, flags=FRAME_RESPONSE, service_id=5,
                          message_id=3, seq=42, payload_bytes=len(body), aux=0)
        assert write_frame(writer, hdr, body) == IPC_SUCCESS
        status, out_hdr, payload = read_frame(reader)
        assert status == IPC_SUCCESS
        assert out_hdr.seq == 42
        assert payload == body
        shm.close()

    def test_multiple_frames(self):
        shm, writer, reader = _make_ring_pair()
        for i in range(10):
            body = bytes([i & 0xFF]) * (i * 100)
            hdr = FrameHeader(flags=FRAME_REQUEST, seq=i,
                              payload_bytes=len(body))
            assert write_frame(writer, hdr, body) == IPC_SUCCESS

        for i in range(10):
            status, out_hdr, payload = read_frame(reader)
            assert status == IPC_SUCCESS
            assert out_hdr.seq == i
            assert payload == bytes([i & 0xFF]) * (i * 100)
        shm.close()

    def test_peek_header(self):
        shm, writer, reader = _make_ring_pair()
        assert peek_frame_header(reader) is None

        hdr = FrameHeader(flags=FRAME_NOTIFY, seq=99, payload_bytes=4)
        write_frame(writer, hdr, b"\xDE\xAD\xBE\xEF")

        peeked = peek_frame_header(reader)
        assert peeked is not None
        assert peeked.seq == 99
        assert peeked.payload_bytes == 4

        # Data should still be there after peek.
        assert reader.read_available() == FRAME_HEADER_SIZE + 4
        shm.close()

    def test_read_empty_ring(self):
        shm, writer, reader = _make_ring_pair()
        status, hdr, payload = read_frame(reader)
        assert status == IPC_ERR_DISCONNECTED
        assert hdr is None
        shm.close()

    def test_ring_full(self):
        shm, writer, reader = _make_ring_pair()
        # Write a frame whose payload fills nearly all the ring.
        big = bytes(MAX_PAYLOAD)
        hdr = FrameHeader(flags=FRAME_REQUEST, payload_bytes=len(big))
        assert write_frame(writer, hdr, big) == IPC_SUCCESS

        # Second write should fail — ring is full.
        small_hdr = FrameHeader(flags=FRAME_REQUEST, payload_bytes=1)
        assert write_frame(writer, small_hdr, b"\x00") == IPC_ERR_RING_FULL
        shm.close()
