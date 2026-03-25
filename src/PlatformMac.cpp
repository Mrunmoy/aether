#include "Platform.h"

#include <atomic>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <fcntl.h>
#include <poll.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace aether::ipc::platform
{

    namespace
    {
        constexpr char kShmPrefix[] = "/aether_shm_";

        // Return a per-user runtime directory for socket files.
        // macOS sets $TMPDIR to /var/folders/<x>/<hash>/T/ which is per-user,
        // mode 0700, immune to symlink attacks in /tmp.
        const std::string &socketDir()
        {
            static const std::string dir = []
            {
                const char *tmpdir = std::getenv("TMPDIR");
                if (tmpdir && tmpdir[0] != '\0')
                {
                    std::string d(tmpdir);
                    if (d.back() != '/')
                        d += '/';
                    return d;
                }
                // Fallback: per-user path under /tmp
                char fallback[128]{};
                std::snprintf(fallback, sizeof(fallback), "/tmp/aether_%u/",
                              static_cast<unsigned>(getuid()));
                ::mkdir(fallback, 0700);
                return std::string(fallback);
            }();
            return dir;
        }

        uint64_t fnv1a64(const char *name)
        {
            constexpr uint64_t kOffset = 14695981039346656037ULL;
            constexpr uint64_t kPrime = 1099511628211ULL;
            uint64_t hash = kOffset;
            for (const unsigned char *p = reinterpret_cast<const unsigned char *>(name); *p != 0;
                 ++p)
            {
                hash ^= *p;
                hash *= kPrime;
            }
            return hash;
        }

        bool setCloExec(int fd)
        {
            int flags = fcntl(fd, F_GETFD);
            if (flags < 0)
            {
                return false;
            }
            return fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == 0;
        }

        bool setNoSigPipe(int fd)
        {
            int enable = 1;
            return setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(enable)) == 0;
        }

        std::string buildSocketPath(const char *name)
        {
            const char *safeName = (name != nullptr) ? name : "";
            char buf[sizeof(sockaddr_un::sun_path)]{};
            std::snprintf(buf, sizeof(buf), "%saether_%016llX.sock",
                          socketDir().c_str(),
                          static_cast<unsigned long long>(fnv1a64(safeName)));
            return std::string(buf);
        }

        sockaddr_un buildAddr(const char *name)
        {
            sockaddr_un addr{};
            addr.sun_family = AF_UNIX;

            std::string path = buildSocketPath(name);
            std::snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", path.c_str());
            return addr;
        }

        bool isManagedSocketPath(const sockaddr_un &addr)
        {
            const std::string &dir = socketDir();
            return std::strncmp(addr.sun_path, dir.c_str(), dir.size()) == 0;
        }
    } // namespace

    Handle serverSocket(const char *name)
    {
        int fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0)
        {
            return -1;
        }
        if (!setCloExec(fd) || !setNoSigPipe(fd))
        {
            close(fd);
            return -1;
        }

        sockaddr_un addr = buildAddr(name);
        unlink(addr.sun_path);

        if (bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0)
        {
            close(fd);
            return -1;
        }

        if (listen(fd, 16) != 0)
        {
            unlink(addr.sun_path);
            close(fd);
            return -1;
        }

        return fd;
    }

    Handle clientSocket(const char *name)
    {
        int fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0)
        {
            return -1;
        }
        if (!setCloExec(fd) || !setNoSigPipe(fd))
        {
            close(fd);
            return -1;
        }

        sockaddr_un addr = buildAddr(name);
        if (connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0)
        {
            close(fd);
            return -1;
        }

        return fd;
    }

    Handle acceptClient(Handle listenFd)
    {
        // Use poll() so accept() doesn't block indefinitely — on macOS,
        // shutdown(SHUT_RDWR) on a listening socket returns ENOTCONN and
        // close() from another thread may not reliably unblock accept().
        // A 500ms timeout lets the caller's loop re-check its stop flag.
        pollfd pfd{};
        pfd.fd = listenFd;
        pfd.events = POLLIN;

        int pr;
        do {
            pr = poll(&pfd, 1, 500);
        } while (pr < 0 && errno == EINTR);

        if (pr <= 0)
        {
            return -1;
        }

        int fd = accept(listenFd, nullptr, nullptr);
        if (fd < 0)
        {
            return -1;
        }
        if (!setCloExec(fd) || !setNoSigPipe(fd))
        {
            close(fd);
            return -1;
        }
        return fd;
    }

    int sendFd(Handle sockFd, Handle fdToSend, const void *data, uint32_t dataLen)
    {
        msghdr msg{};

        uint8_t dummy = 0;
        iovec iov{};
        iov.iov_base = (dataLen > 0) ? const_cast<void *>(data) : &dummy;
        iov.iov_len = (dataLen > 0) ? dataLen : 1;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

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

        ssize_t n;
        do {
            n = sendmsg(sockFd, &msg, 0);
        } while (n < 0 && errno == EINTR);
        return (n == static_cast<ssize_t>(iov.iov_len)) ? 0 : -1;
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

        static constexpr int kMaxFds = 4;
        char control[CMSG_SPACE(kMaxFds * sizeof(int))];
        std::memset(control, 0, sizeof(control));
        msg.msg_control = control;
        msg.msg_controllen = sizeof(control);

        ssize_t n;
        do {
            n = recvmsg(sockFd, &msg, MSG_WAITALL);
        } while (n < 0 && errno == EINTR);
        if (n <= 0)
        {
            return -1;
        }

        if (msg.msg_flags & MSG_CTRUNC)
        {
            for (cmsghdr *cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg))
            {
                if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS)
                {
                    int nfds = static_cast<int>((cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(int));
                    int *fds = reinterpret_cast<int *>(CMSG_DATA(cmsg));
                    for (int i = 0; i < nfds; ++i)
                    {
                        close(fds[i]);
                    }
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
                        *receivedFd = fds[i];
                        firstFdTaken = true;
                    }
                    else
                    {
                        close(fds[i]);
                    }
                }
            }
        }

        return (firstFdTaken && n == static_cast<ssize_t>(iov.iov_len)) ? static_cast<int>(n) : -1;
    }

    int sendSignal(Handle sockFd)
    {
        uint8_t byte = 1;
        ssize_t n;
        do {
            n = send(sockFd, &byte, 1, MSG_DONTWAIT);
        } while (n < 0 && errno == EINTR);
        if (n == 1)
        {
            return 0;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return 0;
        }
        return -1;
    }

    int recvSignal(Handle sockFd)
    {
        uint8_t byte = 0;
        ssize_t n;
        do {
            n = recv(sockFd, &byte, 1, MSG_WAITALL);
        } while (n < 0 && errno == EINTR);
        return (n == 1) ? 0 : -1;
    }

    int setSocketTimeouts(Handle sockFd, uint32_t timeoutMs)
    {
        timeval tv{};
        tv.tv_sec = static_cast<time_t>(timeoutMs / 1000);
        tv.tv_usec = static_cast<suseconds_t>((timeoutMs % 1000) * 1000);

        if (setsockopt(sockFd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) != 0)
        {
            return -1;
        }
        return 0;
    }

    int shutdownConnection(Handle sockFd)
    {
        return (shutdown(sockFd, SHUT_RDWR) == 0) ? 0 : -1;
    }

    Handle shmCreate(uint32_t size, const char * /*name*/)
    {
        static std::atomic<uint64_t> counter{0};

        std::string shmName = kShmPrefix;
        shmName += std::to_string(getpid());
        shmName += "_";
        shmName += std::to_string(counter.fetch_add(1, std::memory_order_relaxed));

        int fd = shm_open(shmName.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd < 0)
        {
            return -1;
        }

        shm_unlink(shmName.c_str());

        if (!setCloExec(fd))
        {
            close(fd);
            return -1;
        }

        if (ftruncate(fd, size) != 0)
        {
            close(fd);
            return -1;
        }

        return fd;
    }

    void *mapSharedMemory(Handle shmFd, uint32_t size)
    {
        return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
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
        if (fd < 0)
        {
            return;
        }

        sockaddr_un addr{};
        socklen_t len = sizeof(addr);
        int acceptConn = 0;
        socklen_t acceptConnLen = sizeof(acceptConn);
        bool isListener = getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &acceptConn, &acceptConnLen) == 0
                          && acceptConn != 0;
        if (isListener && getsockname(fd, reinterpret_cast<sockaddr *>(&addr), &len) == 0
            && addr.sun_family == AF_UNIX && len > offsetof(sockaddr_un, sun_path)
            && addr.sun_path[0] != '\0' && isManagedSocketPath(addr))
        {
            unlink(addr.sun_path);
        }

        close(fd);
    }

} // namespace aether::ipc::platform
