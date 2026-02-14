#include "Connection.h"
#include "Platform.h"

#include <new>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace ms::ipc
{

    // ── Shared Memory Layout ────────────────────────────────────────
    //
    //  [ IpcRing: client→server ]  [ IpcRing: server→client ]
    //  |-------- Ring 0 --------|  |-------- Ring 1 --------|
    //
    //  Client: tx = Ring 0, rx = Ring 1
    //  Server: rx = Ring 0, tx = Ring 1

    static constexpr uint32_t kShmSize = 2 * sizeof(IpcRing);

    // ── Connection::close ───────────────────────────────────────────

    void Connection::close()
    {
        if (shmBase != nullptr && shmBase != MAP_FAILED)
        {
            munmap(shmBase, shmSize);
        }
        platform::closeFd(shmFd);
        platform::closeFd(socketFd);

        socketFd = -1;
        shmFd = -1;
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
        if (conn.socketFd < 0)
        {
            return conn;
        }

        // 2. Create shared memory.
        conn.shmFd = platform::shmCreate(kShmSize);
        if (conn.shmFd < 0)
        {
            conn.close();
            return conn;
        }

        // 3. mmap the shared memory.
        conn.shmBase = mmap(nullptr, kShmSize, PROT_READ | PROT_WRITE,
                            MAP_SHARED, conn.shmFd, 0);
        if (conn.shmBase == MAP_FAILED)
        {
            conn.shmBase = nullptr;
            conn.close();
            return conn;
        }
        conn.shmSize = kShmSize;

        // 4. Placement-new the two ring buffers.
        auto *base = static_cast<uint8_t *>(conn.shmBase);
        conn.txRing = new (base) IpcRing();
        conn.rxRing = new (base + sizeof(IpcRing)) IpcRing();

        // 5. Send protocol version + shared memory FD to server.
        uint16_t version = kProtocolVersion;
        if (platform::sendFd(conn.socketFd, conn.shmFd, &version, sizeof(version)) != 0)
        {
            conn.close();
            return conn;
        }

        // 6. Wait for ACK from server.
        if (platform::recvSignal(conn.socketFd) != 0)
        {
            conn.close();
            return conn;
        }

        return conn;
    }

    // ── Server Handshake ────────────────────────────────────────────

    Connection acceptConnection(int listenFd)
    {
        Connection conn;

        // 1. Accept the client connection.
        conn.socketFd = platform::acceptClient(listenFd);
        if (conn.socketFd < 0)
        {
            return conn;
        }

        // 2. Receive protocol version + shared memory FD from client.
        uint16_t version = 0;
        if (platform::recvFd(conn.socketFd, &conn.shmFd, &version, sizeof(version)) <= 0)
        {
            conn.close();
            return conn;
        }

        // 3. Validate protocol version.
        if (version != kProtocolVersion)
        {
            // NACK: close the socket — client's recvSignal will fail.
            conn.close();
            return conn;
        }

        // 4. Determine shared memory size via fstat.
        struct stat st{};
        if (fstat(conn.shmFd, &st) != 0 || st.st_size < static_cast<off_t>(kShmSize))
        {
            conn.close();
            return conn;
        }

        // 5. mmap the shared memory.
        conn.shmBase = mmap(nullptr, kShmSize, PROT_READ | PROT_WRITE,
                            MAP_SHARED, conn.shmFd, 0);
        if (conn.shmBase == MAP_FAILED)
        {
            conn.shmBase = nullptr;
            conn.close();
            return conn;
        }
        conn.shmSize = kShmSize;

        // 6. Set ring buffer pointers (opposite direction from client).
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

} // namespace ms::ipc
