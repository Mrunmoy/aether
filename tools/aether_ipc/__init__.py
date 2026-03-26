"""aether_ipc -- Python transport layer for aether IPC services.

Provides a client that communicates with C++ aether services over
Unix domain sockets + shared memory ring buffers.
"""

import select
import threading
import time
from typing import Callable

from .constants import (
    PROTOCOL_VERSION,
    FRAME_REQUEST, FRAME_RESPONSE, FRAME_NOTIFY,
    IPC_SUCCESS, IPC_ERR_DISCONNECTED, IPC_ERR_TIMEOUT,
    IPC_ERR_RING_FULL, IPC_ERR_STOPPED,
    FRAME_HEADER_SIZE, MAX_PAYLOAD,
)
from .framing import FrameHeader, write_frame, read_frame
from .transport import AetherTransport

__all__ = ["AetherClient"]

# Re-export commonly used constants.
from .constants import (  # noqa: F401, E402
    IPC_SUCCESS, IPC_ERR_DISCONNECTED, IPC_ERR_TIMEOUT,
    IPC_ERR_RING_FULL, IPC_ERR_STOPPED,
    IPC_ERR_INVALID_SERVICE, IPC_ERR_INVALID_METHOD,
    IPC_ERR_VERSION_MISMATCH, IPC_ERR_INVALID_ARGUMENT,
    IPC_ERR_TRANSPORT, IPC_ERR_CRC, IPC_ERR_NOT_SUPPORTED,
    IPC_ERR_NO_SPACE, IPC_ERR_OVERFLOW,
)

NotificationHandler = Callable[[int, int, bytes], None]


class AetherClient:
    """High-level client for calling aether IPC services.

    Usage::

        client = AetherClient("my_service")
        client.connect()
        status, response = client.call(service_id=1, message_id=2,
                                       request=some_bytes)
        client.disconnect()
    """

    def __init__(self, service_name: str):
        self._service_name = service_name
        self._transport = AetherTransport()
        self._seq = 0
        self._notify_handler: NotificationHandler | None = None
        self._notify_thread: threading.Thread | None = None
        self._stop_event = threading.Event()

    # ---- Lifecycle ----------------------------------------------------------

    def connect(self) -> bool:
        ok = self._transport.connect(self._service_name)
        if ok and self._notify_handler is not None:
            self._start_notify_thread()
        return ok

    def disconnect(self):
        self._stop_event.set()
        if self._notify_thread is not None:
            self._notify_thread.join(timeout=2.0)
            self._notify_thread = None
        self._transport.disconnect()

    @property
    def is_connected(self) -> bool:
        return self._transport.is_connected

    # ---- RPC ----------------------------------------------------------------

    def call(self, service_id: int, message_id: int,
             request: bytes = b"", timeout_ms: int = 2000) -> "tuple[int, bytes]":
        """Send a request and wait for the matching response.

        Returns (status_code, response_payload).
        """
        if not self._transport.is_connected:
            return (IPC_ERR_DISCONNECTED, b"")

        # Build and send request frame.
        self._seq = (self._seq + 1) & 0xFFFF_FFFF
        seq = self._seq

        hdr = FrameHeader(
            version=PROTOCOL_VERSION,
            flags=FRAME_REQUEST,
            service_id=service_id,
            message_id=message_id,
            seq=seq,
            payload_bytes=len(request),
        )

        rc = write_frame(self._transport.tx_ring, hdr, request)
        if rc != IPC_SUCCESS:
            return (rc, b"")

        self._transport.send_signal()

        # Wait for matching response.
        deadline = time.monotonic() + timeout_ms / 1000.0
        while True:
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                return (IPC_ERR_TIMEOUT, b"")

            # Check if data is already in the ring.
            if self._transport.rx_ring.read_available() >= FRAME_HEADER_SIZE:
                status, resp_hdr, payload = read_frame(self._transport.rx_ring)
                if status == IPC_SUCCESS:
                    if resp_hdr.seq == seq and (resp_hdr.flags & FRAME_RESPONSE):
                        return (resp_hdr.aux, payload)
                    # Not our response -- could be a notification; dispatch it.
                    if resp_hdr.flags & FRAME_NOTIFY and self._notify_handler:
                        self._notify_handler(resp_hdr.service_id,
                                             resp_hdr.message_id, payload)
                    continue

            # Poll the socket for a signal with timeout.
            try:
                sock = self._transport._sock
                if sock is None:
                    return (IPC_ERR_DISCONNECTED, b"")
                ready = select.select([sock], [], [], min(remaining, 0.1))
                if ready[0]:
                    self._transport.recv_signal()
            except (OSError, ConnectionError):
                return (IPC_ERR_DISCONNECTED, b"")

    # ---- Notifications ------------------------------------------------------

    def set_notification_handler(self, handler: NotificationHandler | None):
        """Register a callback for server-pushed notifications.

        The handler receives (service_id, message_id, payload).
        Must be called before connect().
        """
        self._notify_handler = handler

    def _start_notify_thread(self):
        self._stop_event.clear()
        self._notify_thread = threading.Thread(
            target=self._notify_loop, daemon=True, name="aether-notify")
        self._notify_thread.start()

    def _notify_loop(self):
        while not self._stop_event.is_set():
            try:
                sock = self._transport._sock
                if sock is None:
                    break
                ready = select.select([sock], [], [], 0.1)
                if not ready[0]:
                    continue
                self._transport.recv_signal()
                # Drain all available notification frames.
                while self._transport.rx_ring.read_available() >= FRAME_HEADER_SIZE:
                    status, hdr, payload = read_frame(self._transport.rx_ring)
                    if status != IPC_SUCCESS:
                        break
                    if hdr.flags & FRAME_NOTIFY and self._notify_handler:
                        self._notify_handler(hdr.service_id, hdr.message_id,
                                             payload)
            except (OSError, ConnectionError):
                break

    # ---- Context manager ----------------------------------------------------

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, *exc):
        self.disconnect()
