"""Integration tests for AetherTransport using a mock server.

We simulate the C++ server side using a socketpair + manual handshake
so we can test the full protocol without needing a compiled C++ binary.
"""

import array
import mmap
import os
import socket
import struct
import sys
import threading
import pytest

_linux_only = pytest.mark.skipif(
    sys.platform != "linux", reason="Linux-only transport (UDS + memfd)"
)

from aether_ipc.constants import (
    PROTOCOL_VERSION, SHM_SIZE, SHM_HANDSHAKE_SIZE, SHM_HANDSHAKE_FORMAT,
    IPCRING_SIZE, FRAME_HEADER_SIZE,
    FRAME_REQUEST, FRAME_RESPONSE,
    IPC_SUCCESS,
)
from aether_ipc.framing import FrameHeader, write_frame, read_frame
from aether_ipc.ringbuffer import SpscRingWriter, SpscRingReader
from aether_ipc.transport import AetherTransport


# ---------------------------------------------------------------------------
# Mock server helpers
# ---------------------------------------------------------------------------

def _mock_server_accept(server_sock: socket.socket):
    """Accept one client, perform the server-side handshake, return (conn, shm_mmap).

    Mirrors acceptConnection() in Connection.cpp.
    """
    conn, _ = server_sock.accept()

    # Receive handshake + shm fd via SCM_RIGHTS.
    msg, ancdata, flags, addr = conn.recvmsg(
        SHM_HANDSHAKE_SIZE,
        socket.CMSG_SPACE(struct.calcsize("i")),
    )

    # Parse handshake.
    version, reserved, shm_name = struct.unpack(SHM_HANDSHAKE_FORMAT, msg)
    assert version == PROTOCOL_VERSION

    # Extract the shared memory fd.
    shm_fd = -1
    for level, typ, data in ancdata:
        if level == socket.SOL_SOCKET and typ == socket.SCM_RIGHTS:
            fds = array.array("i")
            fds.frombytes(data)
            shm_fd = fds[0]
            # Close any extras.
            for extra_fd in fds[1:]:
                os.close(extra_fd)
            break
    assert shm_fd >= 0

    # Validate size.
    st = os.fstat(shm_fd)
    assert st.st_size >= SHM_SIZE

    # Map shared memory.
    shm = mmap.mmap(shm_fd, SHM_SIZE, mmap.MAP_SHARED,
                     mmap.PROT_READ | mmap.PROT_WRITE)
    os.close(shm_fd)

    # Send ACK.
    conn.send(b"\x01")

    return conn, shm


def _echo_server_thread(server_sock: socket.socket, barrier: threading.Barrier):
    """Simple echo server: reads one frame and writes it back as a response."""
    barrier.wait()
    conn, shm = _mock_server_accept(server_sock)

    # Server: rx = Ring 0, tx = Ring 1 (opposite of client).
    rx_ring = SpscRingReader(shm, 0)
    tx_ring = SpscRingWriter(shm, IPCRING_SIZE)

    try:
        # Wait for signal.
        data = conn.recv(1)
        if len(data) != 1:
            return

        # Read frame from rx ring.
        status, hdr, payload = read_frame(rx_ring)
        if status != IPC_SUCCESS:
            return

        # Echo it back as a response.
        resp_hdr = FrameHeader(
            version=hdr.version,
            flags=FRAME_RESPONSE,
            service_id=hdr.service_id,
            message_id=hdr.message_id,
            seq=hdr.seq,
            payload_bytes=len(payload),
            aux=IPC_SUCCESS,
        )
        write_frame(tx_ring, resp_hdr, payload)

        # Signal the client.
        try:
            conn.send(b"\x01", socket.MSG_DONTWAIT)
        except BlockingIOError:
            pass
    finally:
        conn.close()
        shm.close()
        server_sock.close()


# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

@_linux_only
class TestTransportHandshake:
    """Test the handshake protocol with a mock server."""

    def test_connect_and_echo(self):
        # Create a UDS listener in abstract namespace with a unique name.
        import time
        svc_name = f"pytest_{os.getpid()}_{int(time.monotonic()*1e6)}"
        addr = "\0aether_" + svc_name

        server_sock = socket.socket(socket.AF_UNIX, socket.SOCK_SEQPACKET)
        server_sock.bind(addr)
        server_sock.listen(1)

        barrier = threading.Barrier(2, timeout=5)
        server_thread = threading.Thread(
            target=_echo_server_thread,
            args=(server_sock, barrier),
            daemon=True,
        )
        server_thread.start()
        barrier.wait()

        # Client connects.
        transport = AetherTransport()
        try:
            assert transport.connect(svc_name)
            assert transport.is_connected

            # Write a request frame.
            payload = b"ping"
            hdr = FrameHeader(
                version=PROTOCOL_VERSION,
                flags=FRAME_REQUEST,
                service_id=1,
                message_id=2,
                seq=100,
                payload_bytes=len(payload),
            )
            rc = write_frame(transport.tx_ring, hdr, payload)
            assert rc == IPC_SUCCESS

            # Signal the server.
            transport.send_signal()

            # Wait for response signal.
            transport.recv_signal()

            # Read response.
            status, resp_hdr, resp_payload = read_frame(transport.rx_ring)
            assert status == IPC_SUCCESS
            assert resp_hdr.flags == FRAME_RESPONSE
            assert resp_hdr.seq == 100
            assert resp_hdr.service_id == 1
            assert resp_hdr.message_id == 2
            assert resp_payload == b"ping"
            assert resp_hdr.aux == IPC_SUCCESS
        finally:
            transport.disconnect()
            server_thread.join(timeout=5)

    def test_disconnect_cleanup(self):
        transport = AetherTransport()
        # Disconnect on an unconnected transport should be safe.
        transport.disconnect()
        assert not transport.is_connected

    def test_connect_nonexistent_service(self):
        transport = AetherTransport()
        result = transport.connect("nonexistent_service_xyz_12345")
        assert not result
        assert not transport.is_connected


@_linux_only
class TestAetherClientIntegration:
    """Test the high-level AetherClient with a mock echo server."""

    def test_call(self):
        from aether_ipc import AetherClient
        import time

        svc_name = f"pytest_client_{os.getpid()}_{int(time.monotonic()*1e6)}"
        addr = "\0aether_" + svc_name

        server_sock = socket.socket(socket.AF_UNIX, socket.SOCK_SEQPACKET)
        server_sock.bind(addr)
        server_sock.listen(1)

        barrier = threading.Barrier(2, timeout=5)
        server_thread = threading.Thread(
            target=_echo_server_thread,
            args=(server_sock, barrier),
            daemon=True,
        )
        server_thread.start()
        barrier.wait()

        client = AetherClient(svc_name)
        try:
            assert client.connect()
            assert client.is_connected

            status, response = client.call(
                service_id=1, message_id=2,
                request=b"hello world", timeout_ms=5000,
            )
            assert status == IPC_SUCCESS
            assert response == b"hello world"
        finally:
            client.disconnect()
            server_thread.join(timeout=5)
