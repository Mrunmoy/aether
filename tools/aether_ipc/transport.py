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
import struct

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

_libc = ctypes.CDLL(ctypes.util.find_library("c"), use_errno=True)

# int memfd_create(const char *name, unsigned int flags)
_memfd_create = _libc.memfd_create
_memfd_create.restype = ctypes.c_int
_memfd_create.argtypes = [ctypes.c_char_p, ctypes.c_uint]

_MFD_CLOEXEC = 0x0001


def _create_memfd(name: str, size: int) -> int:
    """Create an anonymous shared memory fd via memfd_create + ftruncate."""
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
        self._sock.connect(addr)

        # 2. Set send timeout (matches C++ setSocketTimeouts — SO_SNDTIMEO only).
        timeout_sec = SOCKET_TIMEOUT_MS / 1000.0
        self._sock.settimeout(timeout_sec)

        # 3. Create shared memory via memfd_create.
        self._shm_fd = _create_memfd("aether_shm", SHM_SIZE)

        # 4. mmap the region.
        self._shm = mmap.mmap(self._shm_fd, SHM_SIZE,
                              mmap.MAP_SHARED, mmap.PROT_READ | mmap.PROT_WRITE)

        # 5. Zero-init both ring control blocks (placement-new equivalent).
        # The mmap of a fresh memfd is already zero-filled, but be explicit.
        self._shm[0:SHM_SIZE] = b"\x00" * SHM_SIZE

        # 6. Send handshake: protocol version + shm fd via SCM_RIGHTS.
        hs_data = struct.pack(SHM_HANDSHAKE_FORMAT,
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
            # EAGAIN/EWOULDBLOCK — peer already has a pending wakeup.
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
