"""SPSC ring buffer reader/writer over an mmap'd shared memory region.

Memory layout must match the C++ ouroboros::spsc::RingBuffer<uint8_t, Capacity>:

    ControlBlock (128 bytes, aligned to 64-byte cache lines):
        offset  0: uint32  head   (producer writes, consumer reads)
        offset  4: pad[60]
        offset 64: uint32  tail   (consumer writes, producer reads)
        offset 68: pad[60]
    Data region (Capacity bytes):
        offset 128 .. 128+Capacity-1

Head and tail are monotonically increasing uint32 values. They wrap naturally
at 2**32 and are masked (& Mask) when used as indices into the data region.

Atomicity note: On x86-64, aligned 4-byte loads/stores are naturally atomic.
We use struct.pack_into / struct.unpack_from on the mmap which compile down to
single aligned accesses. This is sufficient for the single-producer /
single-consumer protocol on the same machine.
"""

import mmap
import struct
from .constants import (
    CACHE_LINE_SIZE, CONTROL_BLOCK_SIZE,
    HEAD_OFFSET, TAIL_OFFSET,
    RING_CAPACITY, RING_MASK, DATA_OFFSET,
)

__all__ = ["SpscRingWriter", "SpscRingReader"]

_UINT32 = struct.Struct("<I")
_UINT32_MAX = 0xFFFF_FFFF


class _SpscRingBase:
    """Shared helpers for reader and writer."""

    __slots__ = ("_shm", "_base", "_capacity", "_mask", "_data_off",
                 "_head_off", "_tail_off")

    def __init__(self, shm: mmap.mmap, base_offset: int,
                 capacity: int = RING_CAPACITY):
        self._shm = shm
        self._base = base_offset
        self._capacity = capacity
        self._mask = capacity - 1
        self._data_off = base_offset + DATA_OFFSET
        self._head_off = base_offset + HEAD_OFFSET
        self._tail_off = base_offset + TAIL_OFFSET

    def _load_head(self) -> int:
        return _UINT32.unpack_from(self._shm, self._head_off)[0]

    def _load_tail(self) -> int:
        return _UINT32.unpack_from(self._shm, self._tail_off)[0]

    def _store_head(self, val: int):
        _UINT32.pack_into(self._shm, self._head_off, val & _UINT32_MAX)

    def _store_tail(self, val: int):
        _UINT32.pack_into(self._shm, self._tail_off, val & _UINT32_MAX)


class SpscRingWriter(_SpscRingBase):
    """Producer side of the SPSC ring buffer."""

    __slots__ = ()

    def write_available(self) -> int:
        head = self._load_head()
        tail = self._load_tail()
        return self._capacity - ((head - tail) & _UINT32_MAX)

    def write(self, data: bytes) -> bool:
        count = len(data)
        if count == 0:
            return True

        head = self._load_head()
        tail = self._load_tail()
        available = self._capacity - ((head - tail) & _UINT32_MAX)
        if available < count:
            return False

        offset = head & self._mask
        first_chunk = self._capacity - offset

        if first_chunk >= count:
            self._shm[self._data_off + offset:
                       self._data_off + offset + count] = data
        else:
            self._shm[self._data_off + offset:
                       self._data_off + offset + first_chunk] = data[:first_chunk]
            remainder = count - first_chunk
            self._shm[self._data_off:
                       self._data_off + remainder] = data[first_chunk:]

        self._store_head(head + count)
        return True


class SpscRingReader(_SpscRingBase):
    """Consumer side of the SPSC ring buffer."""

    __slots__ = ()

    def read_available(self) -> int:
        head = self._load_head()
        tail = self._load_tail()
        return (head - tail) & _UINT32_MAX

    def peek(self, count: int) -> "bytes | None":
        if count == 0:
            return b""

        head = self._load_head()
        tail = self._load_tail()
        available = (head - tail) & _UINT32_MAX
        if available < count:
            return None

        offset = tail & self._mask
        first_chunk = self._capacity - offset

        if first_chunk >= count:
            return bytes(self._shm[self._data_off + offset:
                                    self._data_off + offset + count])
        else:
            part1 = bytes(self._shm[self._data_off + offset:
                                     self._data_off + offset + first_chunk])
            remainder = count - first_chunk
            part2 = bytes(self._shm[self._data_off:
                                     self._data_off + remainder])
            return part1 + part2

    def read(self, count: int) -> "bytes | None":
        if count == 0:
            return b""

        head = self._load_head()
        tail = self._load_tail()
        available = (head - tail) & _UINT32_MAX
        if available < count:
            return None

        offset = tail & self._mask
        first_chunk = self._capacity - offset

        if first_chunk >= count:
            result = bytes(self._shm[self._data_off + offset:
                                      self._data_off + offset + count])
        else:
            part1 = bytes(self._shm[self._data_off + offset:
                                     self._data_off + offset + first_chunk])
            remainder = count - first_chunk
            part2 = bytes(self._shm[self._data_off:
                                     self._data_off + remainder])
            result = part1 + part2

        self._store_tail(tail + count)
        return result

    def skip(self, count: int) -> bool:
        if count == 0:
            return True

        head = self._load_head()
        tail = self._load_tail()
        available = (head - tail) & _UINT32_MAX
        if available < count:
            return False

        self._store_tail(tail + count)
        return True
