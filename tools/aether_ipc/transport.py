"""Low-level UDS + shared memory transport for aether IPC.

Implements the client-side handshake protocol matching
aether::ipc::connectToServer() in Connection.cpp.
"""

import array
import ctypes
import ctypes.util
import mmap
import os
import socket
import struct as _struct
import sys

from .constants import (
    PROTOCOL_VERSION, SHM_SIZE, IPCRING_SIZE,
    SHM_HANDSHAKE_FORMAT, SHM_HANDSHAKE_SIZE,
    SOCKET_TIMEOUT_MS,
)
from .ringbuffer import SpscRingWriter, SpscRingReader

__all__ = ["AetherTransport"]

# ---------------------------------------------------------------------------
# memfd_create via ctypes (Linux-only, avoids /dev/shm namespace pollution)
# ---------------------------------------------------------------------------

# Abstract-namespace Unix socket path limit (108 bytes including NUL).
# The usable prefix is 107 bytes (the leading NUL is the namespace marker).
_ABSTRACT_ADDR_MAX = 107

_memfd_create = None
_MFD_CLOEXEC = 0x0001

if sys.platform == "linux":
    try:
        _libc = ctypes.CDLL(ctypes.util.find_library("c"), use_errno=True)
        _memfd_create = _libc.memfd_create
        _memfd_create.restype = ctypes.c_int
        _memfd_create.argtypes = [ctypes.c_char_p, ctypes.c_uint]
    except (OSError, AttributeError):
        _memfd_create = None


def _create_memfd(name: str, size: int) -> int:
    """Create an anonymous shared memory fd via memfd_create + ftruncate."""
    if _memfd_create is None:
        raise OSError("memfd_create is only available on Linux")
    fd = _memfd_create(name.encode(), _MFD_CLOEXEC)
    if fd < 0:
        errno = ctypes.get_errno()
        raise OSError(errno, f"memfd_create failed: {os.strerror(errno)}")
    try:
        os.ftruncate(fd, size)
    except Exception:
        os.close(fd)
        raise
    return fd


class AetherTransport:
    """Low-level transport: UDS socket + shared memory + SPSC rings."""

    def __init__(self):
        self._sock: socket.socket | None = None
        self._shm_fd: int = -1
        self._shm: mmap.mmap | None = None
        self._tx_ring: SpscRingWriter | None = None
        self._rx_ring: SpscRingReader | None = None
        self._connected = False

    # ---- Connection ---------------------------------------------------------

    def connect(self, service_name: str) -> bool:
        """Connect to a C++ aether service (matches connectToServer)."""
        if self._connected:
            return True

        try:
            return self._do_connect(service_name)
        except Exception:
            self.disconnect()
            return False

    def _do_connect(self, service_name: str) -> bool:
        # 1. Create UDS SEQPACKET socket and connect to abstract namespace.
        self._sock = socket.socket(socket.AF_UNIX, socket.SOCK_SEQPACKET)
        addr = "\0aether_" + service_name
        # Validate that the abstract address fits in the kernel limit
        # (107 usable bytes after the leading NUL byte).
        addr_bytes = addr.encode("utf-8")
        if len(addr_bytes) - 1 > _ABSTRACT_ADDR_MAX:
            raise ValueError(
                f"service address 'aether_{service_name}' exceeds the "
                f"{_ABSTRACT_ADDR_MAX}-byte abstract namespace limit"
            )
        self._sock.connect(addr)

        # 2. Set send timeout only (matches C++ SO_SNDTIMEO).
        #    Python's settimeout() sets BOTH SO_SNDTIMEO and SO_RCVTIMEO;
        #    use setsockopt directly to match the C++ behaviour.
        timeout_sec = int(SOCKET_TIMEOUT_MS // 1000)
        timeout_usec = int((SOCKET_TIMEOUT_MS % 1000) * 1000)
        tv = _struct.pack("ll", timeout_sec, timeout_usec)
        self._sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDTIMEO, tv)

        # 3. Create shared memory via memfd_create.
        self._shm_fd = _create_memfd("aether_shm", SHM_SIZE)

        # 4. mmap the region.
        self._shm = mmap.mmap(self._shm_fd, SHM_SIZE,
                              mmap.MAP_SHARED, mmap.PROT_READ | mmap.PROT_WRITE)

        # 5. memfd pages are already zero-filled by the kernel; no explicit
        #    zeroing needed.

        # 6. Send handshake: protocol version + shm fd via SCM_RIGHTS.
        hs_data = _struct.pack(SHM_HANDSHAKE_FORMAT,
                              PROTOCOL_VERSION, 0, b"\x00" * 128)
        self._send_fd(self._shm_fd, hs_data)

        # 7. Wait for ACK signal byte from server.
        self._recv_signal()

        # 8. Set up ring pointers.
        # Client: tx = Ring 0 (offset 0), rx = Ring 1 (offset IPCRING_SIZE).
        self._tx_ring = SpscRingWriter(self._shm, 0)
        self._rx_ring = SpscRingReader(self._shm, IPCRING_SIZE)
        self._connected = True
        return True

    def disconnect(self):
        """Tear down connection and release resources."""
        self._connected = False
        self._tx_ring = None
        self._rx_ring = None
        if self._shm is not None:
            self._shm.close()
            self._shm = None
        if self._shm_fd >= 0:
            os.close(self._shm_fd)
            self._shm_fd = -1
        if self._sock is not None:
            try:
                self._sock.shutdown(socket.SHUT_RDWR)
            except OSError:
                pass
            self._sock.close()
            self._sock = None

    @property
    def is_connected(self) -> bool:
        return self._connected

    def mark_disconnected(self):
        """Mark this transport as disconnected (called by the receiver thread)."""
        self._connected = False

    # ---- Ring access --------------------------------------------------------

    @property
    def tx_ring(self) -> SpscRingWriter:
        assert self._tx_ring is not None, "not connected"
        return self._tx_ring

    @property
    def rx_ring(self) -> SpscRingReader:
        assert self._rx_ring is not None, "not connected"
        return self._rx_ring

    # ---- Signaling ----------------------------------------------------------

    def send_signal(self):
        """Send a single wakeup byte (matches C++ sendSignal)."""
        assert self._sock is not None
        try:
            self._sock.send(b"\x01", socket.MSG_DONTWAIT)
        except BlockingIOError:
            # EAGAIN/EWOULDBLOCK --- peer already has a pending wakeup.
            pass

    def recv_signal(self):
        """Block until a wakeup byte arrives (matches C++ recvSignal)."""
        self._recv_signal()

    def _recv_signal(self):
        assert self._sock is not None
        data = self._sock.recv(1)
        if len(data) != 1:
            raise ConnectionError("recv_signal: peer disconnected")

    # ---- SCM_RIGHTS fd passing ----------------------------------------------

    def _send_fd(self, fd: int, data: bytes):
        """Send *data* with an ancillary SCM_RIGHTS message carrying *fd*."""
        assert self._sock is not None
        fds = array.array("i", [fd])
        ancdata = [(socket.SOL_SOCKET, socket.SCM_RIGHTS, fds)]
        self._sock.sendmsg([data], ancdata)

    # ---- Context manager ----------------------------------------------------

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.disconnect()
