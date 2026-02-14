#pragma once

#include "Types.h"
#include <cstdint>

namespace ms::ipc
{

    // Represents one side of an established IPC connection.
    // Holds the UDS socket (signaling), shared memory fd, mmap'd region,
    // and pointers to the two ring buffers (tx and rx).
    //
    // This is internal infrastructure — not exposed to users.
    // The Service layer manages connections; generated code provides
    // the user-facing API.

    struct Connection
    {
        int socketFd = -1;       // UDS socket for signaling
        int shmFd = -1;          // shared memory file descriptor
        void *shmBase = nullptr; // mmap'd base pointer
        uint32_t shmSize = 0;    // total shared memory region size

        IpcRing *txRing = nullptr; // ring buffer for sending
        IpcRing *rxRing = nullptr; // ring buffer for receiving

        bool valid() const { return socketFd >= 0 && shmBase != nullptr; }

        // Clean up: munmap, close fds, reset all fields.
        void close();
    };

    // ── Handshake Functions ─────────────────────────────────────────
    // Internal — used by Service layer, not by user code.

    // Server side: accept one client connection and perform handshake.
    // listenFd: listening socket from platform::serverSocket().
    // Returns valid Connection on success, invalid (socketFd == -1) on failure.
    Connection acceptConnection(int listenFd);

    // Client side: connect to server and perform handshake.
    // name: service name (same name passed to platform::serverSocket).
    // Returns valid Connection on success, invalid on failure.
    Connection connectToServer(const char *name);

} // namespace ms::ipc
