#include "ClientBase.h"
#include "Platform.h"

#include <sys/socket.h>

namespace ms::ipc
{

    // ── Constructor / Destructor ─────────────────────────────────────

    ClientBase::ClientBase(const char *serviceName, ms::RunLoop *loop)
        : m_serviceName(serviceName), m_loop(loop)
    {
    }

    ClientBase::~ClientBase() { disconnect(); }

    // ── Lifecycle ────────────────────────────────────────────────────

    bool ClientBase::connect()
    {
        if (m_running.load(std::memory_order_acquire))
        {
            return false;
        }

        m_conn = connectToServer(m_serviceName.c_str());
        if (!m_conn.valid())
        {
            return false;
        }

        m_running.store(true, std::memory_order_release);

        if (m_loop)
        {
            m_loop->addSource(m_conn.socketFd, [this] { onDataReady(); });
        }
        else
        {
            m_receiverThread = std::thread([this] { receiverLoop(); });
        }
        return true;
    }

    void ClientBase::disconnect()
    {
        bool wasRunning = m_running.exchange(false);

        if (m_loop)
        {
            if (wasRunning)
            {
                m_loop->removeSource(m_conn.socketFd);
            }
            // Wait for any in-flight handler to finish.
            std::lock_guard<std::mutex> hlock(m_handlerMutex);
        }
        else
        {
            if (wasRunning && m_conn.socketFd >= 0)
            {
                // Unblock receiver thread by shutting down the socket.
                shutdown(m_conn.socketFd, SHUT_RDWR);
            }

            // Always join: the thread may have already exited (server disconnect),
            // or we just unblocked it via shutdown above.
            if (m_receiverThread.joinable())
            {
                m_receiverThread.join();
            }
        }

        // Fail any pending calls not yet resolved (e.g., server disconnected
        // mid-call and the receiver thread already cleared some but not all).
        {
            std::lock_guard<std::mutex> lock(m_pendingMutex);
            for (auto &[_, pending] : m_pending)
            {
                pending->status = IPC_ERR_DISCONNECTED;
                pending->done = true;
                pending->cv.notify_one();
            }
            m_pending.clear();
        }

        {
            std::lock_guard<std::mutex> slock(m_sendMutex);
            m_conn.close();
        }
    }

    bool ClientBase::isConnected() const { return m_running.load(std::memory_order_acquire); }

    // ── Synchronous RPC ─────────────────────────────────────────────

    int ClientBase::call(uint32_t serviceId, uint32_t messageId,
                         const std::vector<uint8_t> &request, std::vector<uint8_t> *response,
                         uint32_t timeoutMs)
    {
        if (!m_running.load(std::memory_order_acquire))
        {
            return IPC_ERR_DISCONNECTED;
        }

        uint32_t seq = m_nextSeq.fetch_add(1, std::memory_order_relaxed);

        // Build request frame.
        FrameHeader header{};
        header.version = kProtocolVersion;
        header.flags = FRAME_REQUEST;
        header.serviceId = serviceId;
        header.messageId = messageId;
        header.seq = seq;
        header.payloadBytes = static_cast<uint32_t>(request.size());

        // Register pending call before writing, so the receiver thread can
        // always find a consumer for any committed frame (the server may
        // drain a newly committed frame before sendSignal if another
        // wakeup is already in flight).
        auto pending = std::make_shared<PendingCall>();
        {
            std::lock_guard<std::mutex> plock(m_pendingMutex);
            m_pending[seq] = pending;
        }

        {
            // Hold sendMutex to serialize txRing writes (SPSC invariant).
            std::lock_guard<std::mutex> slock(m_sendMutex);
            if (!m_running.load(std::memory_order_acquire) || !m_conn.valid())
            {
                std::lock_guard<std::mutex> plock(m_pendingMutex);
                m_pending.erase(seq);
                return IPC_ERR_DISCONNECTED;
            }

            int rc = writeFrame(m_conn.txRing, header, request.data(), header.payloadBytes);
            if (rc != IPC_SUCCESS)
            {
                std::lock_guard<std::mutex> plock(m_pendingMutex);
                m_pending.erase(seq);
                return rc;
            }

            if (platform::sendSignal(m_conn.socketFd) != 0)
            {
                std::lock_guard<std::mutex> plock(m_pendingMutex);
                m_pending.erase(seq);
                return IPC_ERR_DISCONNECTED;
            }
        }

        // Wait for response.
        {
            std::unique_lock<std::mutex> lock(m_pendingMutex);
            if (!pending->cv.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                      [&] { return pending->done; }))
            {
                m_pending.erase(seq);
                return IPC_ERR_TIMEOUT;
            }
        }

        // Collect result.
        int status = pending->status;
        if (response != nullptr)
        {
            *response = std::move(pending->response);
        }

        {
            std::lock_guard<std::mutex> lock(m_pendingMutex);
            m_pending.erase(seq);
        }

        return status;
    }

    // ── Receiver Loop ───────────────────────────────────────────────

    void ClientBase::receiverLoop()
    {
        while (m_running.load(std::memory_order_acquire))
        {
            if (platform::recvSignal(m_conn.socketFd) != 0)
            {
                break;
            }

            // Drain all available frames.
            while (true)
            {
                FrameHeader header{};
                std::vector<uint8_t> payload;
                if (readFrameAlloc(m_conn.rxRing, &header, &payload) != IPC_SUCCESS)
                {
                    break;
                }

                if (header.version != kProtocolVersion)
                {
                    continue;
                }

                if (header.flags & FRAME_RESPONSE)
                {
                    std::lock_guard<std::mutex> plock(m_pendingMutex);
                    auto it = m_pending.find(header.seq);
                    if (it != m_pending.end())
                    {
                        it->second->status = static_cast<int>(header.aux);
                        it->second->response = std::move(payload);
                        it->second->done = true;
                        it->second->cv.notify_one();
                    }
                }
                else if (header.flags & FRAME_NOTIFY)
                {
                    onNotification(header.serviceId, header.messageId, payload);
                }
            }
        }

        // Server disconnected or stop() was called: update state and fail pending calls.
        m_running.store(false, std::memory_order_release);

        std::lock_guard<std::mutex> plock(m_pendingMutex);
        for (auto &[_, pending] : m_pending)
        {
            pending->status = IPC_ERR_DISCONNECTED;
            pending->done = true;
            pending->cv.notify_one();
        }
        m_pending.clear();
    }

    // ── RunLoop Handler ────────────────────────────────────────────

    void ClientBase::onDataReady()
    {
        std::lock_guard<std::mutex> hlock(m_handlerMutex);

        if (!m_running.load(std::memory_order_acquire))
        {
            return;
        }

        // Drain the signal byte (epoll told us it's readable, won't block).
        if (platform::recvSignal(m_conn.socketFd) != 0)
        {
            m_loop->removeSource(m_conn.socketFd);
            m_running.store(false, std::memory_order_release);

            // Fail pending calls.
            std::lock_guard<std::mutex> plock(m_pendingMutex);
            for (auto &[_, pending] : m_pending)
            {
                pending->status = IPC_ERR_DISCONNECTED;
                pending->done = true;
                pending->cv.notify_one();
            }
            m_pending.clear();
            return;
        }

        // Drain all available frames.
        while (true)
        {
            FrameHeader header{};
            std::vector<uint8_t> payload;
            if (readFrameAlloc(m_conn.rxRing, &header, &payload) != IPC_SUCCESS)
            {
                break;
            }

            if (header.version != kProtocolVersion)
            {
                continue;
            }

            if (header.flags & FRAME_RESPONSE)
            {
                std::lock_guard<std::mutex> plock(m_pendingMutex);
                auto it = m_pending.find(header.seq);
                if (it != m_pending.end())
                {
                    it->second->status = static_cast<int>(header.aux);
                    it->second->response = std::move(payload);
                    it->second->done = true;
                    it->second->cv.notify_one();
                }
            }
            else if (header.flags & FRAME_NOTIFY)
            {
                onNotification(header.serviceId, header.messageId, payload);
            }
        }
    }

    // ── Notification (default no-op) ────────────────────────────────

    void ClientBase::onNotification(uint32_t /*serviceId*/, uint32_t /*messageId*/,
                                    const std::vector<uint8_t> & /*payload*/)
    {
    }

} // namespace ms::ipc
