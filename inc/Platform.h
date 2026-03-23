#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace aether::ipc::platform
{
#if defined(_WIN32)
    using Handle = void *;
    constexpr Handle kInvalidHandle = nullptr;
#else
    using Handle = int;
    constexpr Handle kInvalidHandle = -1;
#endif
    constexpr std::size_t kSharedMemoryNameMax = 128;

    struct SharedMemoryHandshake
    {
        uint16_t version = 0;
        uint16_t reserved = 0;
        char shmName[kSharedMemoryNameMax]{};
    };

    static_assert(std::is_trivially_copyable_v<SharedMemoryHandshake>);

    inline bool isValidHandle(Handle handle)
    {
#if defined(_WIN32)
        return handle != nullptr;
#else
        return handle >= 0;
#endif
    }

    // ── Transport Handles ───────────────────────────────────────────

    // Create a listening transport endpoint for a local service name.
    // Linux: abstract UDS SOCK_SEQPACKET.
    // Windows: listener token for a named-pipe endpoint.
    // Returns a valid handle on success, kInvalidHandle on failure.
    Handle serverSocket(const char *name);

    // Connect to a local service endpoint.
    // Returns a valid handle on success, kInvalidHandle on failure.
    Handle clientSocket(const char *name);

    // Accept a connection on a listening endpoint.
    // Returns a valid handle on success, kInvalidHandle on failure.
    Handle acceptClient(Handle listenFd);

    // Send the handshake payload over the connection.
    // Linux also transfers the shared-memory fd via SCM_RIGHTS.
    // Returns 0 on success, -1 on failure.
    int sendFd(Handle sockFd, Handle fdToSend, const void *data, uint32_t dataLen);

    // Receive the handshake payload and resolve/open the shared-memory handle.
    // Sets *receivedFd on success.
    // Returns bytes received (> 0) on success, -1 on failure.
    int recvFd(Handle sockFd, Handle *receivedFd, void *data, uint32_t dataLen);

    // Send a single wakeup byte (non-blocking).
    // Returns 0 on success, or if the peer's receive buffer already holds a
    // pending wakeup (EAGAIN/EWOULDBLOCK — both receiver paths drain all
    // available ring frames per wakeup, so no additional signal is needed).
    // Returns -1 only on a genuine peer-gone error (EPIPE, ECONNRESET, ENOTCONN).
    int sendSignal(Handle sockFd);

    // Receive a single wakeup byte (blocks until available).
    // Returns 0 on success, -1 on failure.
    int recvSignal(Handle sockFd);

    // Set a timeout on socket send operations (SO_SNDTIMEO only).
    // Returns 0 on success, -1 on failure.
    int setSocketTimeouts(Handle sockFd, uint32_t timeoutMs);

    // Cancel a blocked peer I/O operation so a receiver thread can exit.
    int shutdownConnection(Handle sockFd);

    // ── Shared Memory ───────────────────────────────────────────────

    // Create anonymous shared memory region of `size` bytes.
    // Linux: memfd_create + ftruncate. Windows: named file mapping.
    // Returns a valid handle on success, kInvalidHandle on failure.
    Handle shmCreate(uint32_t size, const char *name = nullptr);

    // Map and unmap the shared memory handle returned by shmCreate().
    void *mapSharedMemory(Handle shmFd, uint32_t size);
    void unmapSharedMemory(void *base, uint32_t size);

    // ── File Descriptor ─────────────────────────────────────────────

    // Close a file descriptor or handle. Safe to call with kInvalidHandle.
    void closeFd(Handle fd);

} // namespace aether::ipc::platform
