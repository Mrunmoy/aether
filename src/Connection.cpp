#include "Connection.h"
#include "Platform.h"

#include <new>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <string>
#include <type_traits>

// std::hash and GetCurrentProcessId are only needed for the Windows shared memory name.
#if defined(_WIN32)
#include <functional>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#if !defined(_WIN32)
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace aether::ipc
{

    // IpcRing is placement-new'd into shared memory and never explicitly
    // destructed.  Guard against future changes that add non-trivial members.
    static_assert(std::is_trivially_destructible_v<IpcRing>,
                  "IpcRing must be trivially destructible for placement-new in shared memory");

    // ── Shared Memory Layout ────────────────────────────────────────
    //
    //  [ IpcRing: client→server ]  [ IpcRing: server→client ]
    //  |-------- Ring 0 --------|  |-------- Ring 1 --------|
    //
    //  Client: tx = Ring 0, rx = Ring 1
    //  Server: rx = Ring 0, tx = Ring 1

    static constexpr uint32_t kShmSize = 2 * sizeof(IpcRing);
    static constexpr uint32_t kSocketTimeoutMs = 5000;

    static std::string makeSharedMemoryName(const char *serviceName)
    {
#if defined(_WIN32)
        static std::atomic<uint32_t> nextConnectionId{1};
        const char *safeServiceName = (serviceName != nullptr) ? serviceName : "";
        char buf[128];
        std::snprintf(buf, sizeof(buf), "Local\\aether_shm_%u_%u_%08X",
                      static_cast<unsigned>(GetCurrentProcessId()),
                      static_cast<unsigned>(nextConnectionId.fetch_add(1, std::memory_order_relaxed)),
                      static_cast<unsigned>(std::hash<std::string>{}(safeServiceName)));
        return std::string(buf);
#else
        (void)serviceName;
        return std::string();
#endif
    }

    // ── Connection::close ───────────────────────────────────────────

    void Connection::close()
    {
        platform::unmapSharedMemory(shmBase, shmSize);
        platform::closeFd(shmFd);
        platform::closeFd(socketFd);

        socketFd = platform::kInvalidHandle;
        shmFd = platform::kInvalidHandle;
        shmBase = nullptr;
        shmSize = 0;
        txRing = nullptr;
        rxRing = nullptr;
    }

    // ── Client Handshake ────────────────────────────────────────────

    Connection connectToServer(const char *name)
    {
        Connection conn;

        // 1. Connect to server.
        conn.socketFd = platform::clientSocket(name);
        if (!platform::isValidHandle(conn.socketFd))
        {
            return conn;
        }

        // 2. Set socket timeouts.
        if (platform::setSocketTimeouts(conn.socketFd, kSocketTimeoutMs) != 0)
        {
            conn.close();
            return conn;
        }

        // 3. Create shared memory.
        std::string shmName = makeSharedMemoryName(name);
        const char *platformShmName = shmName.empty() ? nullptr : shmName.c_str();
        conn.shmFd = platform::shmCreate(kShmSize, platformShmName);
        if (!platform::isValidHandle(conn.shmFd))
        {
            conn.close();
            return conn;
        }

        // 4. Map the shared memory.
        conn.shmBase = platform::mapSharedMemory(conn.shmFd, kShmSize);
        if (conn.shmBase == nullptr
#if !defined(_WIN32)
            || conn.shmBase == MAP_FAILED
#endif
        )
        {
            conn.shmBase = nullptr;
            conn.close();
            return conn;
        }
        conn.shmSize = kShmSize;

        // 5. Placement-new the two ring buffers.
        auto *base = static_cast<uint8_t *>(conn.shmBase);
        conn.txRing = new (base) IpcRing();
        conn.rxRing = new (base + sizeof(IpcRing)) IpcRing();

        // 6. Send protocol version + shared memory name to server.
        platform::SharedMemoryHandshake hs{};
        hs.version = kProtocolVersion;
        if (platformShmName != nullptr)
        {
            std::snprintf(hs.shmName, sizeof(hs.shmName), "%s", platformShmName);
        }
        if (platform::sendFd(conn.socketFd, conn.shmFd, &hs, sizeof(hs)) != 0)
        {
            conn.close();
            return conn;
        }

        // 7. Wait for ACK from server.
        if (platform::recvSignal(conn.socketFd) != 0)
        {
            conn.close();
            return conn;
        }

        return conn;
    }

    // ── Server Handshake ────────────────────────────────────────────

    Connection acceptConnection(platform::Handle listenFd)
    {
        Connection conn;

        // 1. Accept the client connection.
        conn.socketFd = platform::acceptClient(listenFd);
        if (!platform::isValidHandle(conn.socketFd))
        {
            return conn;
        }

        // 2. Set socket timeouts.
        if (platform::setSocketTimeouts(conn.socketFd, kSocketTimeoutMs) != 0)
        {
            conn.close();
            return conn;
        }

        // 3. Receive protocol version + shared memory name from client.
        platform::SharedMemoryHandshake hs{};
        if (platform::recvFd(conn.socketFd, &conn.shmFd, &hs, sizeof(hs)) <= 0
            || !platform::isValidHandle(conn.shmFd))
        {
            conn.close();
            return conn;
        }

        // 4. Validate protocol version.
        if (hs.version != kProtocolVersion)
        {
            conn.close();
            return conn;
        }

        // 4b. Validate shared memory size before mapping.
#if !defined(_WIN32)
        {
            struct stat st{};
            if (fstat(static_cast<int>(conn.shmFd), &st) != 0
                || st.st_size < static_cast<off_t>(kShmSize))
            {
                conn.close();
                return conn;
            }
        }
#endif

        // 5. On Windows, force NUL-termination of the received name.
#if defined(_WIN32)
        hs.shmName[sizeof(hs.shmName) - 1] = '\0';
        if (hs.shmName[0] != '\0'
            && std::strncmp(hs.shmName, "Local\\aether_shm_", 17) != 0)
        {
            conn.close();
            return conn;
        }
#endif

        // 6. Map the shared memory.
        conn.shmBase = platform::mapSharedMemory(conn.shmFd, kShmSize);
        if (conn.shmBase == nullptr
#if !defined(_WIN32)
            || conn.shmBase == MAP_FAILED
#endif
        )
        {
            conn.shmBase = nullptr;
            conn.close();
            return conn;
        }
        conn.shmSize = kShmSize;

        // 7. Set ring buffer pointers (opposite direction from client).
        auto *base = static_cast<uint8_t *>(conn.shmBase);
        conn.rxRing = reinterpret_cast<IpcRing *>(base);
        conn.txRing = reinterpret_cast<IpcRing *>(base + sizeof(IpcRing));

        // 7. Send ACK to client.
        if (platform::sendSignal(conn.socketFd) != 0)
        {
            conn.close();
            return conn;
        }

        return conn;
    }

} // namespace aether::ipc
