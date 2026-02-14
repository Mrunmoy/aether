#include "Platform.h"

#include <cstddef>
#include <cstring>

#include <fcntl.h>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace ms::ipc::platform
{

    // ── Helpers ─────────────────────────────────────────────────────

    // Build abstract namespace address: \0ipc_<name>
    // Returns the total sockaddr_un length to pass to bind/connect.
    static socklen_t buildAddr(sockaddr_un *addr, const char *name)
    {
        std::memset(addr, 0, sizeof(*addr));
        addr->sun_family = AF_UNIX;
        addr->sun_path[0] = '\0'; // abstract namespace

        // "ipc_" prefix + name
        constexpr char prefix[] = "ipc_";
        constexpr size_t prefixLen = sizeof(prefix) - 1;
        size_t nameLen = std::strlen(name);

        // Clamp to available space (sun_path[0] is \0, leaves 107 bytes)
        size_t maxPayload = sizeof(addr->sun_path) - 1;
        size_t payloadLen = prefixLen + nameLen;
        if (payloadLen > maxPayload)
        {
            payloadLen = maxPayload;
            nameLen = maxPayload - prefixLen;
        }

        std::memcpy(addr->sun_path + 1, prefix, prefixLen);
        std::memcpy(addr->sun_path + 1 + prefixLen, name, nameLen);

        return static_cast<socklen_t>(offsetof(sockaddr_un, sun_path) + 1 + payloadLen);
    }

    // ── Unix Domain Sockets ─────────────────────────────────────────

    int serverSocket(const char *name)
    {
        int fd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
        if (fd < 0)
        {
            return -1;
        }

        sockaddr_un addr{};
        socklen_t len = buildAddr(&addr, name);

        if (bind(fd, reinterpret_cast<sockaddr *>(&addr), len) != 0)
        {
            close(fd);
            return -1;
        }

        if (listen(fd, 16) != 0)
        {
            close(fd);
            return -1;
        }

        return fd;
    }

    int clientSocket(const char *name)
    {
        int fd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
        if (fd < 0)
        {
            return -1;
        }

        sockaddr_un addr{};
        socklen_t len = buildAddr(&addr, name);

        if (connect(fd, reinterpret_cast<sockaddr *>(&addr), len) != 0)
        {
            close(fd);
            return -1;
        }

        return fd;
    }

    int acceptClient(int listenFd) { return accept4(listenFd, nullptr, nullptr, SOCK_CLOEXEC); }

    int sendFd(int sockFd, int fdToSend, const void *data, uint32_t dataLen)
    {
        msghdr msg{};

        // At least 1 byte of data is required for sendmsg to work.
        uint8_t dummy = 0;
        iovec iov{};
        iov.iov_base = (dataLen > 0) ? const_cast<void *>(data) : &dummy;
        iov.iov_len = (dataLen > 0) ? dataLen : 1;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        // Control message carries the file descriptor.
        char control[CMSG_SPACE(sizeof(int))];
        std::memset(control, 0, sizeof(control));
        msg.msg_control = control;
        msg.msg_controllen = sizeof(control);

        cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        std::memcpy(CMSG_DATA(cmsg), &fdToSend, sizeof(int));

        ssize_t n = sendmsg(sockFd, &msg, 0);
        return (n > 0) ? 0 : -1;
    }

    int recvFd(int sockFd, int *receivedFd, void *data, uint32_t dataLen)
    {
        msghdr msg{};

        uint8_t dummy = 0;
        iovec iov{};
        iov.iov_base = (dataLen > 0) ? data : &dummy;
        iov.iov_len = (dataLen > 0) ? dataLen : 1;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        char control[CMSG_SPACE(sizeof(int))];
        std::memset(control, 0, sizeof(control));
        msg.msg_control = control;
        msg.msg_controllen = sizeof(control);

        ssize_t n = recvmsg(sockFd, &msg, 0);
        if (n <= 0)
        {
            return -1;
        }

        *receivedFd = -1;
        for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg != nullptr; cmsg = CMSG_NXTHDR(&msg, cmsg))
        {
            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS)
            {
                std::memcpy(receivedFd, CMSG_DATA(cmsg), sizeof(int));
                break;
            }
        }

        return static_cast<int>(n);
    }

    int sendSignal(int sockFd)
    {
        uint8_t byte = 1;
        ssize_t n = send(sockFd, &byte, 1, 0);
        return (n == 1) ? 0 : -1;
    }

    int recvSignal(int sockFd)
    {
        uint8_t byte = 0;
        ssize_t n = recv(sockFd, &byte, 1, 0);
        return (n == 1) ? 0 : -1;
    }

    // ── Shared Memory ───────────────────────────────────────────────

    int shmCreate(uint32_t size)
    {
        int fd = static_cast<int>(syscall(SYS_memfd_create, "ipc_shm", MFD_CLOEXEC));
        if (fd < 0)
        {
            return -1;
        }

        if (ftruncate(fd, size) != 0)
        {
            close(fd);
            return -1;
        }

        return fd;
    }

    // ── File Descriptor ─────────────────────────────────────────────

    void closeFd(int fd)
    {
        if (fd >= 0)
        {
            close(fd);
        }
    }

} // namespace ms::ipc::platform
