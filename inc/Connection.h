#pragma once

#include "Platform.h"
#include "Types.h"
#include <cstdint>

namespace aether::ipc
{

    // Represents one side of an established IPC connection.
    // Holds the signaling handle, shared memory handle, mapped region,
    // and pointers to the two ring buffers (tx and rx).
    //
    // This is internal infrastructure — not exposed to users.
    // The Service layer manages connections; generated code provides
    // the user-facing API.

    struct Connection
    {
        platform::Handle socketFd = platform::kInvalidHandle; // signaling handle
        platform::Handle shmFd = platform::kInvalidHandle;     // shared memory handle
        void *shmBase = nullptr;                               // mapped base pointer
        uint32_t shmSize = 0;                                  // total shared memory region size

        IpcRing *txRing = nullptr; // ring buffer for sending
        IpcRing *rxRing = nullptr; // ring buffer for receiving

        Connection() = default;

        // Non-copyable: owns fds and mmap'd memory.
        Connection(const Connection &) = delete;
        Connection &operator=(const Connection &) = delete;

        // Move transfers ownership and resets the source.
        Connection(Connection &&other) noexcept
            : socketFd(other.socketFd), shmFd(other.shmFd), shmBase(other.shmBase),
              shmSize(other.shmSize), txRing(other.txRing), rxRing(other.rxRing)
        {
            other.socketFd = platform::kInvalidHandle;
            other.shmFd = platform::kInvalidHandle;
            other.shmBase = nullptr;
            other.shmSize = 0;
            other.txRing = nullptr;
            other.rxRing = nullptr;
        }

        Connection &operator=(Connection &&other) noexcept
        {
            if (this != &other)
            {
                close();
                socketFd = other.socketFd;
                shmFd = other.shmFd;
                shmBase = other.shmBase;
                shmSize = other.shmSize;
                txRing = other.txRing;
                rxRing = other.rxRing;
                other.socketFd = platform::kInvalidHandle;
                other.shmFd = platform::kInvalidHandle;
                other.shmBase = nullptr;
                other.shmSize = 0;
                other.txRing = nullptr;
                other.rxRing = nullptr;
            }
            return *this;
        }

        bool valid() const { return platform::isValidHandle(socketFd) && shmBase != nullptr; }

        // Clean up: munmap, close fds, reset all fields.
        void close();
    };

    // ── Handshake Functions ─────────────────────────────────────────
    // Internal — used by Service layer, not by user code.

    // Server side: accept one client connection and perform handshake.
    // listenFd: listening socket from platform::serverSocket().
    // Returns valid Connection on success, invalid (socketFd == kInvalidHandle) on failure.
    Connection acceptConnection(platform::Handle listenFd);

    // Client side: connect to server and perform handshake.
    // name: service name (same name passed to platform::serverSocket).
    // Returns valid Connection on success, invalid on failure.
    Connection connectToServer(const char *name);

} // namespace aether::ipc
