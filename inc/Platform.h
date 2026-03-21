#pragma once

#include <cstdint>

namespace ms::ipc::platform
{

    // ── Unix Domain Sockets ─────────────────────────────────────────

    // Create a listening UDS socket bound to abstract namespace.
    // Linux: SOCK_SEQPACKET, path = \0ipc_<name>
    // Returns fd >= 0 on success, -1 on failure.
    int serverSocket(const char *name);

    // Connect to a UDS server in abstract namespace.
    // Returns fd >= 0 on success, -1 on failure.
    int clientSocket(const char *name);

    // Accept a connection on a listening socket.
    // Returns fd >= 0 on success, -1 on failure.
    int acceptClient(int listenFd);

    // Send a file descriptor + ancillary data over UDS (SCM_RIGHTS).
    // Returns 0 on success, -1 on failure.
    int sendFd(int sockFd, int fdToSend, const void *data, uint32_t dataLen);

    // Receive a file descriptor + ancillary data over UDS (SCM_RIGHTS).
    // Sets *receivedFd on success.
    // Returns bytes received (> 0) on success, -1 on failure.
    int recvFd(int sockFd, int *receivedFd, void *data, uint32_t dataLen);

    // Send a single wakeup byte.
    // Returns 0 on success, -1 on failure.
    int sendSignal(int sockFd);

    // Receive a single wakeup byte (blocks until available).
    // Returns 0 on success, -1 on failure.
    int recvSignal(int sockFd);

    // Set send/receive timeouts on a socket.
    // Returns 0 on success, -1 on failure.
    int setSocketTimeouts(int sockFd, uint32_t timeoutMs);

    // ── Shared Memory ───────────────────────────────────────────────

    // Create anonymous shared memory region of `size` bytes.
    // Linux: memfd_create + ftruncate.
    // Returns fd >= 0 on success, -1 on failure.
    int shmCreate(uint32_t size);

    // ── File Descriptor ─────────────────────────────────────────────

    // Close a file descriptor. Safe to call with -1 (no-op).
    void closeFd(int fd);

} // namespace ms::ipc::platform
