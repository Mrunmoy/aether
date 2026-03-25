#include "ServiceBase.h"
#include "Platform.h"

#include <algorithm>
namespace aether::ipc
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
        if (m_running.load(std::memory_order_acquire))
        {
            return false; // already started
        }
#if defined(_WIN32)
        if (m_loop != nullptr)
        {
            // Windows transport currently supports the threaded accept/receiver
            // model only. Reject RunLoop mode until the platform layer exposes
            // a waitable readiness handle for named pipes.
            return false;
        }
#endif

        m_listenFd = platform::serverSocket(m_serviceName.c_str());
        if (!platform::isValidHandle(m_listenFd))
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

#if !defined(_WIN32)
        if (m_loop)
        {
            // Remove listen source.
            if (platform::isValidHandle(m_listenFd))
            {
                m_loop->removeSource(m_listenFd);
                platform::closeFd(m_listenFd);
                m_listenFd = platform::kInvalidHandle;
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
#endif
        {
            // Phase 1: Unblock accept thread. shutdown() alone may not
            // reliably unblock accept4() on all socket types, so close the
            // fd as well — this guarantees accept4() returns immediately.
            if (platform::isValidHandle(m_listenFd))
            {
                platform::shutdownConnection(m_listenFd);
                platform::closeFd(m_listenFd);
                m_listenFd = platform::kInvalidHandle;
            }

            if (m_acceptThread.joinable())
            {
                m_acceptThread.join();
            }

            // Phase 2: Unblock all receiver threads, then join and cleanup.
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            for (auto &c : m_clients)
            {
                if (platform::isValidHandle(c->conn.socketFd))
                {
                    platform::shutdownConnection(c->conn.socketFd);
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

    void ServiceBase::setMaxClients(uint32_t max) { m_maxClients.store(max, std::memory_order_relaxed); }

    void ServiceBase::setAllowedPeerUid(uint32_t uid)
    {
        m_allowedPeerUid.store(uid, std::memory_order_release);
        m_peerUidFilterEnabled.store(true, std::memory_order_release);
    }

    void ServiceBase::clearPeerUidFilter()
    {
        m_peerUidFilterEnabled.store(false, std::memory_order_release);
    }

    // ── Peer Credential Check ────────────────────────────────────────

    bool ServiceBase::checkPeerUid(platform::Handle socketFd)
    {
#if !defined(_WIN32) && !defined(__APPLE__)
        if (m_peerUidFilterEnabled.load(std::memory_order_acquire))
        {
            uint32_t peerUid = 0;
            if (platform::getPeerUid(socketFd, &peerUid) != 0)
                return false;
            uint32_t allowed = m_allowedPeerUid.load(std::memory_order_acquire);
            if (peerUid != allowed)
                return false;
        }
#else
        (void)socketFd;
#endif
        return true;
    }

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

            if (!checkPeerUid(conn.socketFd))
            {
                conn.close();
                continue;
            }

            // Reap dead clients and check max clients limit before committing.
            std::vector<std::shared_ptr<ClientConn>> deadClients;
            bool overLimit = false;
            {
                std::lock_guard<std::mutex> lock(m_clientsMutex);

                // Partition: live clients first, dead clients last.
                auto it = std::stable_partition(
                    m_clients.begin(), m_clients.end(),
                    [](const auto &c) { return !c->dead.load(std::memory_order_acquire); });
                for (auto move_it = it; move_it != m_clients.end(); ++move_it)
                    deadClients.push_back(std::move(*move_it));
                m_clients.erase(it, m_clients.end());

                if (const auto maxC = m_maxClients.load(std::memory_order_relaxed); maxC > 0 && m_clients.size() >= maxC)
                {
                    overLimit = true;
                }
                else
                {
                    auto client = std::make_shared<ClientConn>();
                    client->conn = std::move(conn);
                    client->thread =
                        std::thread([this, ptr = client.get()] { receiverLoop(ptr); });
                    m_clients.push_back(std::move(client));
                }
            }

            // Join dead client threads outside the lock.
            for (auto &c : deadClients)
            {
                if (c->thread.joinable())
                    c->thread.join();
                c->conn.close();
            }

            if (overLimit)
                conn.close();
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

        if (!checkPeerUid(conn.socketFd))
        {
            conn.close();
            return;
        }

        // Reap dead clients and check max clients limit.
        {
            std::lock_guard<std::mutex> lock(m_clientsMutex);

            // Remove dead entries (already closed by removeClient).
            m_clients.erase(
                std::remove_if(m_clients.begin(), m_clients.end(),
                               [](const auto &c)
                               { return c->dead.load(std::memory_order_acquire); }),
                m_clients.end());

            if (const auto maxC = m_maxClients.load(std::memory_order_relaxed); maxC > 0 && m_clients.size() >= maxC)
            {
                conn.close();
                return;
            }

            auto client = std::make_shared<ClientConn>();
            client->conn = std::move(conn);

            ClientConn *ptr = client.get();
            m_loop->addSource(ptr->conn.socketFd,
                              [this, ptr] { onClientReady(ptr); },
                              [this, ptr] { removeClient(ptr); });

            m_clients.push_back(std::move(client));
        }
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

            bool sendFailed = false;
            {
                std::lock_guard<std::mutex> slock(client->sendMutex);
                int rc = writeFrame(client->conn.txRing, response, responsePayload.data(),
                                    response.payloadBytes);
                if (rc != IPC_SUCCESS || platform::sendSignal(client->conn.socketFd) != 0)
                {
                    sendFailed = true;
                }
            }

            if (sendFailed)
            {
                removeClient(client);
                return;
            }
        }
    }

    void ServiceBase::removeClient(ClientConn *client)
    {
        // Guard: might be called from both the read and error callbacks.
        if (client->dead.load(std::memory_order_acquire))
            return;

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
        bool disconnectClient = false;
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

                if (header.version != kProtocolVersion || !(header.flags & FRAME_REQUEST))
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
                    int rc = writeFrame(client->conn.txRing, response, responsePayload.data(),
                                        response.payloadBytes);
                    if (rc != IPC_SUCCESS || platform::sendSignal(client->conn.socketFd) != 0)
                    {
                        client->dead.store(true, std::memory_order_release);
                        if (platform::isValidHandle(client->conn.socketFd))
                        {
                            platform::shutdownConnection(client->conn.socketFd);
                        }
                        disconnectClient = true;
                    }
                }

                if (disconnectClient)
                {
                    break;
                }
            }

            if (disconnectClient)
            {
                break;
            }
        }
    }

    // ── Notification Broadcast ───────────────────────────────────────

    int ServiceBase::sendNotify(uint32_t serviceId, uint32_t messageId, const uint8_t *payload,
                                uint32_t payloadBytes)
    {
        // Phase 1: Snapshot shared_ptrs under m_clientsMutex, then release.
        // Copying shared_ptrs extends ClientConn lifetime — even if stop()
        // clears m_clients concurrently, the objects survive until we drop
        // the snapshot. This lets acceptLoop add new clients without waiting
        // for the entire broadcast to finish.
        std::vector<std::shared_ptr<ClientConn>> snapshot;
        uint32_t notifySeq = 0;
        {
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            if (m_clients.empty())
            {
                return IPC_SUCCESS;
            }
            snapshot = m_clients;
            // Assign sequence inside lock so delivery order matches sequence
            // order when multiple threads call sendNotify concurrently.
            notifySeq = m_notifySeq.fetch_add(1, std::memory_order_relaxed) + 1;
        }

        FrameHeader header{};
        header.version = kProtocolVersion;
        header.flags = FRAME_NOTIFY;
        header.serviceId = serviceId;
        header.messageId = messageId;
        header.payloadBytes = payloadBytes;
        header.aux = notifySeq;

        int result = IPC_SUCCESS;

        // Phase 2: Iterate outside m_clientsMutex. For each client, acquire its
        // sendMutex, check dead, write frame, send signal.
        // The shared_ptr snapshot guarantees the ClientConn stays alive even
        // if stop() clears m_clients concurrently.
        for (auto &c : snapshot)
        {
            if (!m_running.load(std::memory_order_acquire))
            {
                break;
            }
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
                    // Ring full means this client is not consuming data.
                    // Mark it dead and shut down its socket so receiverLoop
                    // unblocks from recvSignal and the thread can be joined.
                    c->dead.store(true, std::memory_order_release);
                    if (platform::isValidHandle(c->conn.socketFd))
                        platform::shutdownConnection(c->conn.socketFd);
                    if (result == IPC_SUCCESS)
                        result = IPC_ERR_DISCONNECTED;
                    continue;
                }

                if (platform::sendSignal(c->conn.socketFd) != 0)
                {
                    c->dead.store(true, std::memory_order_release);
                    if (result == IPC_SUCCESS)
                        result = IPC_ERR_DISCONNECTED;
                    continue;
                }
            }
        }

        // Phase 3: Try to reap dead clients. Use try_lock to avoid a
        // deadlock if stop() is holding m_clientsMutex while waiting for
        // this thread to join (e.g., when sendNotify is called from a
        // receiver thread's onRequest handler). If we can't get the lock,
        // skip reaping — acceptLoop or the next sendNotify will clean up.
        std::vector<std::shared_ptr<ClientConn>> deadClients;
        {
            std::unique_lock<std::mutex> lock(m_clientsMutex, std::try_to_lock);
            if (lock.owns_lock())
            {
                auto it = std::stable_partition(
                    m_clients.begin(), m_clients.end(),
                    [](const auto &c) { return !c->dead.load(std::memory_order_acquire); });
                for (auto move_it = it; move_it != m_clients.end(); ++move_it)
                {
                    deadClients.push_back(std::move(*move_it));
                }
                m_clients.erase(it, m_clients.end());
            }
        }

        // Phase 4: Join dead client threads and close connections outside all locks.
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

} // namespace aether::ipc
