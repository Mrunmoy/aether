#include "ServiceBase.h"
#include "Platform.h"

#include <algorithm>
#include <sys/socket.h>

namespace ms::ipc
{

    // ── Constructor / Destructor ─────────────────────────────────────

    ServiceBase::ServiceBase(const char *serviceName, ms::RunLoop *loop)
        : m_serviceName(serviceName), m_loop(loop)
    {
    }

    ServiceBase::~ServiceBase() { stop(); }

    // ── Lifecycle ────────────────────────────────────────────────────

    bool ServiceBase::start()
    {
        m_listenFd = platform::serverSocket(m_serviceName.c_str());
        if (m_listenFd < 0)
        {
            return false;
        }

        m_running.store(true, std::memory_order_release);

        if (m_loop)
        {
            m_loop->addSource(m_listenFd, [this] { onAcceptReady(); });
        }
        else
        {
            m_acceptThread = std::thread([this] { acceptLoop(); });
        }
        return true;
    }

    void ServiceBase::stop()
    {
        if (!m_running.exchange(false))
        {
            return;
        }

        if (m_loop)
        {
            // Remove listen source.
            if (m_listenFd >= 0)
            {
                m_loop->removeSource(m_listenFd);
                platform::closeFd(m_listenFd);
                m_listenFd = -1;
            }

            // Snapshot client list and remove sources (under lock).
            std::vector<ClientConn *> snapshot;
            {
                std::lock_guard<std::mutex> lock(m_clientsMutex);
                for (auto &c : m_clients)
                {
                    m_loop->removeSource(c->conn.socketFd);
                    snapshot.push_back(c.get());
                }
            }

            // Wait for in-flight handlers and close (without m_clientsMutex).
            for (auto *c : snapshot)
            {
                std::lock_guard<std::mutex> hlock(c->handlerMutex);
                c->conn.close();
            }

            // Clear the list.
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            m_clients.clear();
        }
        else
        {
            // Phase 1: Unblock accept thread, then join before closing fd.
            if (m_listenFd >= 0)
            {
                shutdown(m_listenFd, SHUT_RDWR);
            }

            if (m_acceptThread.joinable())
            {
                m_acceptThread.join();
            }

            // Safe to close now — accept thread has exited.
            if (m_listenFd >= 0)
            {
                platform::closeFd(m_listenFd);
                m_listenFd = -1;
            }

            // Phase 2: Unblock all receiver threads, then join and cleanup.
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            for (auto &c : m_clients)
            {
                if (c->conn.socketFd >= 0)
                {
                    shutdown(c->conn.socketFd, SHUT_RDWR);
                }
            }
            for (auto &c : m_clients)
            {
                if (c->thread.joinable())
                {
                    c->thread.join();
                }
                c->conn.close();
            }
            m_clients.clear();
        }
    }

    bool ServiceBase::isRunning() const { return m_running.load(std::memory_order_acquire); }

    // ── Accept Loop ──────────────────────────────────────────────────

    void ServiceBase::acceptLoop()
    {
        while (m_running.load(std::memory_order_acquire))
        {
            Connection conn = acceptConnection(m_listenFd);
            if (!conn.valid())
            {
                continue;
            }

            auto client = std::make_unique<ClientConn>();
            client->conn = std::move(conn);

            client->thread = std::thread([this, ptr = client.get()] { receiverLoop(ptr); });

            std::lock_guard<std::mutex> lock(m_clientsMutex);
            m_clients.push_back(std::move(client));
        }
    }

    // ── RunLoop Handlers ────────────────────────────────────────────

    void ServiceBase::onAcceptReady()
    {
        Connection conn = acceptConnection(m_listenFd);
        if (!conn.valid())
        {
            return;
        }

        auto client = std::make_unique<ClientConn>();
        client->conn = std::move(conn);

        ClientConn *ptr = client.get();
        m_loop->addSource(ptr->conn.socketFd, [this, ptr] { onClientReady(ptr); });

        std::lock_guard<std::mutex> lock(m_clientsMutex);
        m_clients.push_back(std::move(client));
    }

    void ServiceBase::onClientReady(ClientConn *client)
    {
        std::lock_guard<std::mutex> lock(client->handlerMutex);

        if (!m_running.load(std::memory_order_acquire))
        {
            return;
        }

        // Drain the signal byte (epoll told us it's readable, won't block).
        if (platform::recvSignal(client->conn.socketFd) != 0)
        {
            removeClient(client);
            return;
        }

        // Drain all available frames.
        while (true)
        {
            FrameHeader header{};
            std::vector<uint8_t> payload;
            if (readFrameAlloc(client->conn.rxRing, &header, &payload) != IPC_SUCCESS)
            {
                break;
            }

            if (header.version != kProtocolVersion || !(header.flags & FRAME_REQUEST))
            {
                continue;
            }

            // Dispatch to subclass.
            std::vector<uint8_t> responsePayload;
            int status = onRequest(header.messageId, payload, &responsePayload);

            // Build and send response.
            FrameHeader response{};
            response.version = kProtocolVersion;
            response.flags = FRAME_RESPONSE;
            response.serviceId = header.serviceId;
            response.messageId = header.messageId;
            response.seq = header.seq;
            response.payloadBytes = static_cast<uint32_t>(responsePayload.size());
            response.aux = static_cast<uint32_t>(status);

            {
                std::lock_guard<std::mutex> slock(client->sendMutex);
                writeFrame(client->conn.txRing, response, responsePayload.data(),
                           response.payloadBytes);
                platform::sendSignal(client->conn.socketFd);
            }
        }
    }

    void ServiceBase::removeClient(ClientConn *client)
    {
        // Remove the RunLoop source first (while socketFd is still valid).
        m_loop->removeSource(client->conn.socketFd);

        // Mark dead and close conn under sendMutex so we can't race with a concurrent
        // sendNotify that has passed the dead-flag check and is about to write to txRing.
        {
            std::lock_guard<std::mutex> slock(client->sendMutex);
            client->dead.store(true, std::memory_order_release);
            client->conn.close();
        }

        // Do NOT erase from m_clients here: the caller (onClientReady) still holds
        // a raw pointer to this ClientConn and its lock_guard on handlerMutex.
        // Destroying the unique_ptr here would make the lock_guard destructor
        // unlock an already-freed mutex (UB). The entry is reaped on the next
        // sendNotify call or during stop().
    }

    // ── Receiver Loop (per client) ───────────────────────────────────

    void ServiceBase::receiverLoop(ClientConn *client)
    {
        while (m_running.load(std::memory_order_acquire))
        {
            if (platform::recvSignal(client->conn.socketFd) != 0)
            {
                // Mark dead under sendMutex so we can't race with a concurrent
                // sendNotify that has passed the dead-flag check and is about
                // to acquire sendMutex to write to txRing.
                // Do NOT call conn.close() here — the sendNotify two-phase reap
                // or stop() joins this thread first, then closes the connection.
                {
                    std::lock_guard<std::mutex> slock(client->sendMutex);
                    client->dead.store(true, std::memory_order_release);
                }
                break;
            }

            // Drain all available frames.
            while (true)
            {
                FrameHeader header{};
                std::vector<uint8_t> payload;
                if (readFrameAlloc(client->conn.rxRing, &header, &payload) != IPC_SUCCESS)
                {
                    break;
                }

                if (!(header.flags & FRAME_REQUEST))
                {
                    continue;
                }

                // Dispatch to subclass.
                std::vector<uint8_t> responsePayload;
                int status = onRequest(header.messageId, payload, &responsePayload);

                // Build and send response — hold sendMutex to keep SPSC invariant.
                FrameHeader response{};
                response.version = kProtocolVersion;
                response.flags = FRAME_RESPONSE;
                response.serviceId = header.serviceId;
                response.messageId = header.messageId;
                response.seq = header.seq;
                response.payloadBytes = static_cast<uint32_t>(responsePayload.size());
                response.aux = static_cast<uint32_t>(status);

                // Serialize with sendNotify which also writes to txRing.
                {
                    std::lock_guard<std::mutex> slock(client->sendMutex);
                    writeFrame(client->conn.txRing, response, responsePayload.data(),
                               response.payloadBytes);
                    platform::sendSignal(client->conn.socketFd);
                }
            }
        }
    }

    // ── Notification Broadcast ───────────────────────────────────────

    int ServiceBase::sendNotify(uint32_t serviceId, uint32_t messageId, const uint8_t *payload,
                                uint32_t payloadBytes)
    {
        // Fast path: nothing to do if no clients are connected.
        {
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            if (m_clients.empty())
            {
                return IPC_SUCCESS;
            }
        }

        FrameHeader header{};
        header.version = kProtocolVersion;
        header.flags = FRAME_NOTIFY;
        header.serviceId = serviceId;
        header.messageId = messageId;
        header.payloadBytes = payloadBytes;

        int result = IPC_SUCCESS;
        std::vector<std::unique_ptr<ClientConn>> deadClients;

        {
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            for (auto &c : m_clients)
            {
                // Fast path: skip obviously dead clients without acquiring sendMutex.
                if (c->dead.load(std::memory_order_acquire))
                {
                    continue;
                }

                // Hold sendMutex while writing so we serialize with receiverLoop
                // response writes (SPSC: only one producer to txRing at a time).
                // Re-check dead after acquiring the lock to close the TOCTOU window
                // between the fast-path check above and the actual write below.
                {
                    std::lock_guard<std::mutex> slock(c->sendMutex);
                    if (c->dead.load(std::memory_order_acquire))
                    {
                        continue;
                    }

                    int rc = writeFrame(c->conn.txRing, header, payload, payloadBytes);
                    if (rc != IPC_SUCCESS)
                    {
                        result = rc;
                        continue;
                    }

                    if (platform::sendSignal(c->conn.socketFd) != 0)
                    {
                        c->dead.store(true, std::memory_order_release);
                        result = IPC_ERR_DISCONNECTED;
                        continue;
                    }
                }
            }

            // Extract dead clients under lock, then release lock before joining.
            auto it = std::stable_partition(
                m_clients.begin(), m_clients.end(),
                [](const auto &c) { return !c->dead.load(std::memory_order_acquire); });
            for (auto move_it = it; move_it != m_clients.end(); ++move_it)
            {
                deadClients.push_back(std::move(*move_it));
            }
            m_clients.erase(it, m_clients.end());
        }

        // Join threads and close connections outside the lock.
        for (auto &c : deadClients)
        {
            if (c->thread.joinable())
            {
                c->thread.join();
            }
            // conn.close() is idempotent: already called for RunLoop clients
            // (in removeClient), harmless no-op if called again.
            c->conn.close();
        }

        return result;
    }

} // namespace ms::ipc
