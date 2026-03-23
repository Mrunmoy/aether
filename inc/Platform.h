#pragma once

#include <cstdint>

namespace aether::ipc::platform
{

    // ── Unix Domain Sockets ─────────────────────────────────────────

    // Create a listening same-machine signaling endpoint.
    // Linux uses an abstract UDS name; macOS uses a pathname UDS.
    // Returns fd >= 0 on success, -1 on failure.
    int serverSocket(const char *name);

    // Connect to a signaling endpoint created by serverSocket().
    // Returns fd >= 0 on success, -1 on failure.
    int clientSocket(const char *name);

    // Accept a connection on a listening socket.
    // Returns fd >= 0 on success, -1 on failure.
    int acceptClient(int listenFd);

    // Send a file descriptor + ancillary data over the signaling socket.
    // Returns 0 on success, -1 on failure.
    int sendFd(int sockFd, int fdToSend, const void *data, uint32_t dataLen);

    // Receive a file descriptor + ancillary data over the signaling socket.
    // Sets *receivedFd on success.
    // Returns bytes received (> 0) on success, -1 on failure.
    int recvFd(int sockFd, int *receivedFd, void *data, uint32_t dataLen);

    // Send a single wakeup byte (non-blocking).
    // Returns 0 on success, or if the peer's receive buffer already holds a
    // pending wakeup (EAGAIN/EWOULDBLOCK — both receiver paths drain all
    // available ring frames per wakeup, so no additional signal is needed).
    // Returns -1 only on a genuine peer-gone error (EPIPE, ECONNRESET, ENOTCONN).
    int sendSignal(int sockFd);

    // Receive a single wakeup byte (blocks until available).
    // Returns 0 on success, -1 on failure.
    int recvSignal(int sockFd);

    // Set a timeout on socket send operations (SO_SNDTIMEO only).
    // Returns 0 on success, -1 on failure.
    int setSocketTimeouts(int sockFd, uint32_t timeoutMs);

    // ── Shared Memory ───────────────────────────────────────────────

    // Create anonymous or connection-owned shared memory region of `size` bytes.
    // Returns fd >= 0 on success, -1 on failure.
    int shmCreate(uint32_t size);

    // ── File Descriptor ─────────────────────────────────────────────

    // Close a file descriptor. Safe to call with -1 (no-op).
    void closeFd(int fd);

} // namespace aether::ipc::platform
