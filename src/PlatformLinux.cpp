#include "Platform.h"

#include <cerrno>
#include <cstddef>
#include <cstring>

#include <fcntl.h>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace aether::ipc::platform
{

    // ── Helpers ─────────────────────────────────────────────────────

    // Build abstract namespace address: \0aether_<name>
    // Returns the total sockaddr_un length to pass to bind/connect.
    static socklen_t buildAddr(sockaddr_un *addr, const char *name)
    {
        std::memset(addr, 0, sizeof(*addr));
        addr->sun_family = AF_UNIX;
        addr->sun_path[0] = '\0'; // abstract namespace

        // "aether_" prefix + name
        constexpr char prefix[] = "aether_";
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

    Handle serverSocket(const char *name)
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

    Handle clientSocket(const char *name)
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

    Handle acceptClient(Handle listenFd)
    {
        return accept4(static_cast<int>(listenFd), nullptr, nullptr, SOCK_CLOEXEC);
    }

    int sendFd(Handle sockFd, Handle fdToSend, const void *data, uint32_t dataLen)
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
        int nativeFd = static_cast<int>(fdToSend);
        std::memcpy(CMSG_DATA(cmsg), &nativeFd, sizeof(int));

        ssize_t n = sendmsg(static_cast<int>(sockFd), &msg, MSG_NOSIGNAL);
        return (n > 0) ? 0 : -1;
    }

    int recvFd(Handle sockFd, Handle *receivedFd, void *data, uint32_t dataLen)
    {
        msghdr msg{};

        uint8_t dummy = 0;
        iovec iov{};
        iov.iov_base = (dataLen > 0) ? data : &dummy;
        iov.iov_len = (dataLen > 0) ? dataLen : 1;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        // Allocate enough control space for a few fds so we can detect (and
        // close) any extra fds a peer might send.
        static constexpr int kMaxFds = 4;
        char control[CMSG_SPACE(kMaxFds * sizeof(int))];
        std::memset(control, 0, sizeof(control));
        msg.msg_control = control;
        msg.msg_controllen = sizeof(control);

        ssize_t n = recvmsg(static_cast<int>(sockFd), &msg, 0);
        if (n <= 0)
        {
            return -1;
        }

        // If control data was truncated, the kernel may have installed fds
        // we can't see. Treat this as a protocol error — close any fds we
        // did receive and fail.
        if (msg.msg_flags & MSG_CTRUNC)
        {
            for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg))
            {
                if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS)
                {
                    int nfds = static_cast<int>((cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(int));
                    int *fds = reinterpret_cast<int *>(CMSG_DATA(cmsg));
                    for (int i = 0; i < nfds; ++i)
                        close(fds[i]);
                }
            }
            return -1;
        }

        *receivedFd = kInvalidHandle;
        bool firstFdTaken = false;
        for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg != nullptr; cmsg = CMSG_NXTHDR(&msg, cmsg))
        {
            if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS)
            {
                int nfds = static_cast<int>((cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(int));
                int *fds = reinterpret_cast<int *>(CMSG_DATA(cmsg));
                for (int i = 0; i < nfds; ++i)
                {
                    if (!firstFdTaken)
                    {
                        *receivedFd = static_cast<Handle>(fds[i]);
                        firstFdTaken = true;
                    }
                    else
                    {
                        close(fds[i]); // close unexpected extra fds
                    }
                }
            }
        }

        return static_cast<int>(n);
    }

    int sendSignal(Handle sockFd)
    {
        uint8_t byte = 1;
        ssize_t n = send(static_cast<int>(sockFd), &byte, 1, MSG_NOSIGNAL | MSG_DONTWAIT);
        if (n == 1)
            return 0;
        // EAGAIN/EWOULDBLOCK: the peer's receive buffer already holds a pending
        // wakeup byte.  Both receiver paths drain *all* available ring frames per
        // wakeup, so the frame we just wrote will be picked up on the next
        // iteration — no additional signal is needed.
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;
        // Any other error (EPIPE, ECONNRESET, ENOTCONN) means the peer is gone.
        return -1;
    }

    int recvSignal(Handle sockFd)
    {
        uint8_t byte = 0;
        ssize_t n = recv(static_cast<int>(sockFd), &byte, 1, 0);
        return (n == 1) ? 0 : -1;
    }

    int setSocketTimeouts(Handle sockFd, uint32_t timeoutMs)
    {
        timeval tv{};
        tv.tv_sec = static_cast<time_t>(timeoutMs / 1000);
        tv.tv_usec = static_cast<suseconds_t>((timeoutMs % 1000) * 1000);

        // Only set SO_SNDTIMEO. The receiver threads intentionally block on
        // recv() and rely on shutdown() to unblock — SO_RCVTIMEO would cause
        // spurious disconnect detection (EAGAIN misread as peer close).
        if (setsockopt(static_cast<int>(sockFd), SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv))
            != 0)
        {
            return -1;
        }
        return 0;
    }

    int shutdownConnection(Handle sockFd)
    {
        return (shutdown(static_cast<int>(sockFd), SHUT_RDWR) == 0) ? 0 : -1;
    }

    // ── Peer Credentials ──────────────────────────────────────────────

    int getPeerUid(Handle sockFd, uint32_t *uid)
    {
        struct ucred cred{};
        socklen_t len = sizeof(cred);
        if (getsockopt(static_cast<int>(sockFd), SOL_SOCKET, SO_PEERCRED, &cred, &len) != 0)
            return -1;
        *uid = static_cast<uint32_t>(cred.uid);
        return 0;
    }

    // ── Shared Memory ───────────────────────────────────────────────

    Handle shmCreate(uint32_t size, const char * /*name*/)
    {
        int fd = static_cast<int>(syscall(SYS_memfd_create, "aether_shm", MFD_CLOEXEC));
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

    void *mapSharedMemory(Handle shmFd, uint32_t size)
    {
        return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                    static_cast<int>(shmFd), 0);
    }

    void unmapSharedMemory(void *base, uint32_t size)
    {
        if (base != nullptr && base != MAP_FAILED)
        {
            munmap(base, size);
        }
    }

    void closeFd(Handle fd)
    {
        if (fd >= 0)
        {
            close(static_cast<int>(fd));
        }
    }

} // namespace aether::ipc::platform
