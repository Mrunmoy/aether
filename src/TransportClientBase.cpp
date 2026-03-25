#include "TransportClientBase.h"

namespace aether::ipc
{

    // ── Constructor / Destructor ─────────────────────────────────────

    TransportClientBase::TransportClientBase(const char *serviceName)
        : m_serviceName(serviceName)
    {
    }

    TransportClientBase::~TransportClientBase()
    {
        disconnect();
    }

    // ── Lifecycle ────────────────────────────────────────────────────

    bool TransportClientBase::connect(std::unique_ptr<ITransport> transport)
    {
        // Atomically claim the "not running" → "connecting" transition.
        // If two threads race on connect(), only one wins the CAS.
        bool expected = false;
        if (!m_running.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        {
            return false; // already connected or connecting
        }

        if (m_receiverThread.joinable())
        {
            m_receiverThread.join();
        }

        if (!transport || !transport->connected())
        {
            m_running.store(false, std::memory_order_release);
            return false;
        }

        // Publish m_transport under m_sendMutex so call() — which reads
        // m_transport under the same lock — never sees a stale pointer.
        // During the brief window between the CAS above and this assignment,
        // call() may observe m_running==true but m_transport==null; its
        // existing null-check under m_sendMutex safely returns DISCONNECTED.
        {
            std::lock_guard<std::mutex> slock(m_sendMutex);
            m_transport = std::move(transport);
        }

        m_receiverThread = std::thread([this] { receiverLoop(); });
        return true;
    }

    void TransportClientBase::disconnect()
    {
        bool wasRunning = m_running.exchange(false);

        // Read m_transport under m_sendMutex to synchronize with
        // connect() which assigns m_transport under the same lock.
        {
            std::lock_guard<std::mutex> slock(m_sendMutex);
            if (wasRunning && m_transport)
            {
                m_transport->shutdown();
            }
        }

        if (m_receiverThread.joinable())
        {
            m_receiverThread.join();
        }

        // Fail any pending calls.
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
            m_transport.reset();
        }
    }

    bool TransportClientBase::isConnected() const
    {
        return m_running.load(std::memory_order_acquire);
    }

    // ── Sequence allocation ─────────────────────────────────────────

    uint32_t TransportClientBase::nextUniqueSeq()
    {
        // Caller must hold m_pendingMutex.
        //
        // This loop terminates because m_pending.size() is bounded by available
        // memory (at most a few thousand concurrent calls), so at most that many
        // iterations before finding a free seq.
        uint32_t seq = m_nextSeq.fetch_add(1, std::memory_order_relaxed);
        if (seq == 0)
            seq = m_nextSeq.fetch_add(1, std::memory_order_relaxed);

        uint32_t start = seq;
        while (m_pending.count(seq))
        {
            seq = m_nextSeq.fetch_add(1, std::memory_order_relaxed);
            if (seq == 0)
                seq = m_nextSeq.fetch_add(1, std::memory_order_relaxed);
            if (seq == start)
                return 0; // wrapped completely — all seqs exhausted
        }
        return seq;
    }

    // ── Synchronous RPC ─────────────────────────────────────────────

    int TransportClientBase::call(uint32_t serviceId, uint32_t messageId,
                                  const std::vector<uint8_t> &request,
                                  std::vector<uint8_t> *response,
                                  uint32_t timeoutMs)
    {
        if (!m_running.load(std::memory_order_acquire))
        {
            return IPC_ERR_DISCONNECTED;
        }

        // Register pending call before sending so the receiver thread can
        // always find a consumer for any response.
        auto pending = std::make_shared<PendingCall>();
        uint32_t seq;
        {
            std::lock_guard<std::mutex> plock(m_pendingMutex);
            seq = nextUniqueSeq();
            if (seq == 0)
            {
                return IPC_ERR_OVERFLOW;
            }
            m_pending[seq] = pending;
        }

        FrameHeader header{};
        header.version = kProtocolVersion;
        header.flags = FRAME_REQUEST;
        header.serviceId = serviceId;
        header.messageId = messageId;
        header.seq = seq;
        header.payloadBytes = static_cast<uint32_t>(request.size());

        {
            std::lock_guard<std::mutex> slock(m_sendMutex);
            if (!m_running.load(std::memory_order_acquire) || !m_transport)
            {
                std::lock_guard<std::mutex> plock(m_pendingMutex);
                m_pending.erase(seq);
                return IPC_ERR_DISCONNECTED;
            }

            int rc = m_transport->sendFrame(header, request.data(), header.payloadBytes);
            if (rc != IPC_SUCCESS)
            {
                std::lock_guard<std::mutex> plock(m_pendingMutex);
                m_pending.erase(seq);
                return rc;
            }
        }

        // Wait for response.
        int status;
        {
            std::unique_lock<std::mutex> lock(m_pendingMutex);
            if (!pending->cv.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                      [&] { return pending->done; }))
            {
                m_pending.erase(seq);
                return IPC_ERR_TIMEOUT;
            }

            status = pending->status;
            if (response != nullptr)
            {
                *response = std::move(pending->response);
            }
            m_pending.erase(seq);
        }

        return status;
    }

    // ── Receiver Loop ───────────────────────────────────────────────

    void TransportClientBase::receiverLoop()
    {
        while (m_running.load(std::memory_order_acquire))
        {
            FrameHeader header{};
            std::vector<uint8_t> payload;

            int rc = m_transport->recvFrame(&header, &payload);
            if (rc != IPC_SUCCESS)
            {
                break;
            }

            if (header.version != kProtocolVersion)
            {
                continue;
            }

            if (header.flags & FRAME_RESPONSE)
            {
                std::shared_ptr<PendingCall> matched;
                {
                    std::lock_guard<std::mutex> plock(m_pendingMutex);
                    auto it = m_pending.find(header.seq);
                    if (it != m_pending.end())
                    {
                        matched = it->second;
                        matched->status = static_cast<int>(header.aux);
                        matched->response = std::move(payload);
                        matched->done = true;
                    }
                }
                if (matched)
                {
                    matched->cv.notify_one();
                }
            }
            else if (header.flags & FRAME_NOTIFY)
            {
                onNotification(header.serviceId, header.messageId, payload);
            }
        }

        // Transport disconnected or disconnect() was called.
        m_running.store(false, std::memory_order_release);

        std::vector<std::shared_ptr<PendingCall>> toNotify;
        {
            std::lock_guard<std::mutex> plock(m_pendingMutex);
            for (auto &[_, pending] : m_pending)
            {
                pending->status = IPC_ERR_DISCONNECTED;
                pending->done = true;
                toNotify.push_back(pending);
            }
            m_pending.clear();
        }
        for (auto &p : toNotify)
        {
            p->cv.notify_one();
        }
    }

    // ── Notification (default no-op) ────────────────────────────────

    void TransportClientBase::onNotification(uint32_t /*serviceId*/, uint32_t /*messageId*/,
                                             const std::vector<uint8_t> & /*payload*/)
    {
    }

} // namespace aether::ipc
