"""Tests for the SPSC ring buffer reader/writer over mmap."""

import mmap
import struct
import pytest

from aether_ipc.constants import (
    RING_CAPACITY, RING_MASK, CONTROL_BLOCK_SIZE, DATA_OFFSET,
    HEAD_OFFSET, TAIL_OFFSET, IPCRING_SIZE, CACHE_LINE_SIZE,
)
from aether_ipc.ringbuffer import SpscRingWriter, SpscRingReader


def _make_pair(capacity=RING_CAPACITY):
    """Create a writer/reader pair backed by a single anonymous mmap."""
    size = CONTROL_BLOCK_SIZE + capacity
    shm = mmap.mmap(-1, size)
    writer = SpscRingWriter(shm, 0, capacity)
    reader = SpscRingReader(shm, 0, capacity)
    return shm, writer, reader


class TestLayoutConstants:
    """Verify our constants match the C++ ouroboros layout."""

    def test_control_block_size(self):
        # 2 cache lines: head + pad, tail + pad
        assert CONTROL_BLOCK_SIZE == 2 * CACHE_LINE_SIZE == 128

    def test_head_offset(self):
        assert HEAD_OFFSET == 0

    def test_tail_offset(self):
        assert TAIL_OFFSET == CACHE_LINE_SIZE == 64

    def test_data_offset(self):
        assert DATA_OFFSET == CONTROL_BLOCK_SIZE == 128

    def test_ipcring_size(self):
        assert IPCRING_SIZE == CONTROL_BLOCK_SIZE + RING_CAPACITY
        assert IPCRING_SIZE == 262272

    def test_capacity_is_power_of_2(self):
        assert RING_CAPACITY > 0
        assert (RING_CAPACITY & (RING_CAPACITY - 1)) == 0

    def test_mask(self):
        assert RING_MASK == RING_CAPACITY - 1


class TestBasicReadWrite:
    def test_empty(self):
        shm, writer, reader = _make_pair()
        assert reader.read_available() == 0
        assert writer.write_available() == RING_CAPACITY
        shm.close()

    def test_write_read_small(self):
        shm, writer, reader = _make_pair()
        data = b"hello"
        assert writer.write(data)
        assert reader.read_available() == 5
        assert writer.write_available() == RING_CAPACITY - 5
        result = reader.read(5)
        assert result == data
        assert reader.read_available() == 0
        shm.close()

    def test_write_read_exact_capacity(self):
        shm, writer, reader = _make_pair()
        data = bytes(range(256)) * (RING_CAPACITY // 256)
        assert len(data) == RING_CAPACITY
        assert writer.write(data)
        assert writer.write_available() == 0
        result = reader.read(RING_CAPACITY)
        assert result == data
        shm.close()

    def test_write_exceeds_capacity(self):
        shm, writer, reader = _make_pair()
        data = b"\x00" * (RING_CAPACITY + 1)
        assert not writer.write(data)
        shm.close()

    def test_read_more_than_available(self):
        shm, writer, reader = _make_pair()
        writer.write(b"abc")
        result = reader.read(4)
        assert result is None
        # Data should still be there.
        assert reader.read_available() == 3
        shm.close()


class TestPeek:
    def test_peek_does_not_consume(self):
        shm, writer, reader = _make_pair()
        writer.write(b"ABCDE")
        peeked = reader.peek(3)
        assert peeked == b"ABC"
        assert reader.read_available() == 5  # unchanged
        result = reader.read(5)
        assert result == b"ABCDE"
        shm.close()

    def test_peek_empty(self):
        shm, writer, reader = _make_pair()
        assert reader.peek(1) is None
        shm.close()


class TestSkip:
    def test_skip(self):
        shm, writer, reader = _make_pair()
        writer.write(b"ABCDE")
        assert reader.skip(3)
        assert reader.read_available() == 2
        result = reader.read(2)
        assert result == b"DE"
        shm.close()

    def test_skip_too_much(self):
        shm, writer, reader = _make_pair()
        writer.write(b"AB")
        assert not reader.skip(3)
        assert reader.read_available() == 2
        shm.close()


class TestWrapAround:
    """Test that reads/writes wrap correctly at the ring boundary."""

    def test_wraparound(self):
        # Use a small capacity for quick wraparound.
        cap = 64
        shm = mmap.mmap(-1, CONTROL_BLOCK_SIZE + cap)
        writer = SpscRingWriter(shm, 0, cap)
        reader = SpscRingReader(shm, 0, cap)

        # Fill and drain to advance head/tail near the boundary.
        for _ in range(10):
            chunk = bytes(range(48))
            assert writer.write(chunk)
            result = reader.read(48)
            assert result == chunk

        # Now head & tail are at 480 (480 & 63 = 32).
        # Write a chunk that wraps: 48 bytes starting at offset 32 wraps
        # around offset 64 back to offset 0.
        wrap_data = bytes(range(48))
        assert writer.write(wrap_data)
        result = reader.read(48)
        assert result == wrap_data
        shm.close()

    def test_peek_wraparound(self):
        cap = 64
        shm = mmap.mmap(-1, CONTROL_BLOCK_SIZE + cap)
        writer = SpscRingWriter(shm, 0, cap)
        reader = SpscRingReader(shm, 0, cap)

        # Advance near boundary.
        for _ in range(10):
            assert writer.write(b"X" * 48)
            reader.read(48)

        # Write wrapping data and peek.
        writer.write(b"ABCDEFGHIJKLMNOP")  # 16 bytes
        peeked = reader.peek(16)
        assert peeked == b"ABCDEFGHIJKLMNOP"
        shm.close()


class TestRawMemoryLayout:
    """Verify the actual byte offsets in the mmap match C++ expectations."""

    def test_head_tail_offsets(self):
        shm, writer, reader = _make_pair()
        # Initially both head and tail are 0.
        head_val = struct.unpack_from("<I", shm, HEAD_OFFSET)[0]
        tail_val = struct.unpack_from("<I", shm, TAIL_OFFSET)[0]
        assert head_val == 0
        assert tail_val == 0

        # Write 10 bytes -> head advances to 10.
        writer.write(b"\xFF" * 10)
        head_val = struct.unpack_from("<I", shm, HEAD_OFFSET)[0]
        tail_val = struct.unpack_from("<I", shm, TAIL_OFFSET)[0]
        assert head_val == 10
        assert tail_val == 0

        # Read 10 bytes -> tail advances to 10.
        reader.read(10)
        tail_val = struct.unpack_from("<I", shm, TAIL_OFFSET)[0]
        assert tail_val == 10
        shm.close()

    def test_data_at_correct_offset(self):
        shm, writer, reader = _make_pair()
        writer.write(b"\xDE\xAD\xBE\xEF")
        # Data should be at DATA_OFFSET (128).
        assert shm[DATA_OFFSET] == 0xDE
        assert shm[DATA_OFFSET + 1] == 0xAD
        assert shm[DATA_OFFSET + 2] == 0xBE
        assert shm[DATA_OFFSET + 3] == 0xEF
        shm.close()
