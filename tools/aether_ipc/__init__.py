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
    aux_to_signed,
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


class _PendingCall:
    """Slot used by call() to wait for its response from the receiver thread."""
    __slots__ = ("cv", "result")

    def __init__(self):
        self.cv = threading.Condition()
        self.result: "tuple[int, bytes] | None" = None


class AetherClient:
    """High-level client for calling aether IPC services.

    Architecture -- single-consumer SPSC compliance:
        The rx_ring is an SPSC (single-producer, single-consumer) ring buffer.
        Only ONE thread may read from it.  A dedicated *receiver thread* is the
        sole consumer of rx_ring.  It dispatches:
          - **responses** to the ``call()`` that is waiting for them, via a
            dict of ``_PendingCall`` condition variables keyed by sequence
            number (mirrors the C++ ``ClientBase`` pattern).
          - **notifications** to the registered notification handler.

        ``call()`` never touches rx_ring directly; it registers a pending
        slot, writes to tx_ring (safe -- only one producer thread at a time
        due to the GIL + _tx_lock), sends a signal, and waits on its
        condition variable.

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
        self._rx_thread: threading.Thread | None = None
        self._stop_event = threading.Event()
        # Pending RPC calls: seq -> _PendingCall.  Protected by _pending_lock.
        self._pending: dict[int, _PendingCall] = {}
        self._pending_lock = threading.Lock()
        # Serialize tx_ring writes from multiple caller threads.
        self._tx_lock = threading.Lock()

    # ---- Lifecycle ----------------------------------------------------------

    def connect(self) -> bool:
        # Join any leftover receiver thread from a previous connection
        # to avoid leaking threads (mirrors C++ ClientBase::connect).
        if self._rx_thread is not None:
            self._stop_event.set()
            self._rx_thread.join(timeout=2.0)
            self._rx_thread = None
        ok = self._transport.connect(self._service_name)
        if ok:
            self._start_rx_thread()
        return ok

    def disconnect(self):
        self._stop_event.set()
        if self._rx_thread is not None:
            self._rx_thread.join(timeout=2.0)
            self._rx_thread = None
        self._transport.disconnect()
        # Wake any callers still waiting.
        with self._pending_lock:
            for pc in self._pending.values():
                with pc.cv:
                    if pc.result is None:
                        pc.result = (IPC_ERR_DISCONNECTED, b"")
                    pc.cv.notify()
            self._pending.clear()

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

        # Allocate a sequence number and register the pending slot BEFORE
        # writing the frame, matching C++ ClientBase::call() ordering.
        with self._tx_lock:
            self._seq = (self._seq + 1) & 0xFFFF_FFFF
            if self._seq == 0:
                self._seq = 1
            seq = self._seq

            # Skip sequence numbers that collide with in-flight calls
            # (mirrors C++ PR #23 wraparound collision fix).
            with self._pending_lock:
                while seq in self._pending:
                    seq = (seq + 1) & 0xFFFF_FFFF
                    if seq == 0:
                        seq = 1
                self._seq = seq

                # Register pending BEFORE writeFrame so the receiver
                # thread cannot miss the response.
                pending = _PendingCall()
                self._pending[seq] = pending

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
                # Write failed -- erase the pending entry.
                with self._pending_lock:
                    self._pending.pop(seq, None)
                return (rc, b"")

            self._transport.send_signal()

        # Wait for the receiver thread to deliver our response.
        deadline = time.monotonic() + timeout_ms / 1000.0
        with pending.cv:
            while pending.result is None:
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    break
                pending.cv.wait(timeout=remaining)

        # Clean up the pending entry.
        with self._pending_lock:
            self._pending.pop(seq, None)

        if pending.result is not None:
            return pending.result
        return (IPC_ERR_TIMEOUT, b"")

    # ---- Notifications ------------------------------------------------------

    def set_notification_handler(self, handler: NotificationHandler | None):
        """Register a callback for server-pushed notifications.

        The handler receives (service_id, message_id, payload).
        Must be called before connect().
        """
        self._notify_handler = handler

    # ---- Receiver thread (sole consumer of rx_ring) -------------------------

    def _start_rx_thread(self):
        self._stop_event.clear()
        self._rx_thread = threading.Thread(
            target=self._rx_loop, daemon=True, name="aether-rx")
        self._rx_thread.start()

    def _rx_loop(self):
        """Single reader of rx_ring.  Dispatches responses and notifications."""
        while not self._stop_event.is_set():
            try:
                sock = self._transport._sock
                if sock is None:
                    break
                ready = select.select([sock], [], [], 0.1)
                if ready[0]:
                    self._transport.recv_signal()
                # Drain all available frames from rx_ring.
                while self._transport.rx_ring.read_available() >= FRAME_HEADER_SIZE:
                    status, hdr, payload = read_frame(self._transport.rx_ring)
                    if status != IPC_SUCCESS:
                        break
                    # Version check -- skip frames with mismatched protocol
                    # version (matches C++ receiverLoop behaviour).
                    if hdr.version != PROTOCOL_VERSION:
                        continue
                    if hdr.flags & FRAME_RESPONSE:
                        self._dispatch_response(hdr, payload)
                    elif hdr.flags & FRAME_NOTIFY:
                        if self._notify_handler:
                            self._notify_handler(
                                hdr.service_id, hdr.message_id, payload)
            except (OSError, ConnectionError):
                break

        # Loop exited (socket error or stop requested) -- fail all pending
        # calls with IPC_ERR_DISCONNECTED (matches C++ receiverLoop exit).
        self._transport._connected = False
        with self._pending_lock:
            for pc in self._pending.values():
                with pc.cv:
                    if pc.result is None:
                        pc.result = (IPC_ERR_DISCONNECTED, b"")
                    pc.cv.notify()

    def _dispatch_response(self, hdr: FrameHeader, payload: bytes):
        """Deliver a response frame to the call() waiting on this seq."""
        with self._pending_lock:
            pending = self._pending.get(hdr.seq)
        if pending is not None:
            # aux is unsigned on the wire; reinterpret as signed status code.
            status = aux_to_signed(hdr.aux)
            with pending.cv:
                pending.result = (status, payload)
                pending.cv.notify()

    # ---- Context manager ----------------------------------------------------

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, *exc):
        self.disconnect()
