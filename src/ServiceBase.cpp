#include "ServiceBase.h"
#include "Platform.h"

#include <sys/socket.h>

namespace ms::ipc
{

    // ── Constructor / Destructor ─────────────────────────────────────

    ServiceBase::ServiceBase(const char *serviceName) : m_serviceName(serviceName) {}

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
        m_acceptThread = std::thread([this] { acceptLoop(); });
        return true;
    }

    void ServiceBase::stop()
    {
        if (!m_running.exchange(false))
        {
            return;
        }

        // Phase 1: Unblock accept thread.
        if (m_listenFd >= 0)
        {
            shutdown(m_listenFd, SHUT_RDWR);
            platform::closeFd(m_listenFd);
            m_listenFd = -1;
        }

        if (m_acceptThread.joinable())
        {
            m_acceptThread.join();
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
            client->conn = conn;

            // Zero out local so it doesn't appear to own the fds.
            conn.socketFd = -1;
            conn.shmFd = -1;
            conn.shmBase = nullptr;
            conn.txRing = nullptr;
            conn.rxRing = nullptr;

            client->thread = std::thread([this, ptr = client.get()] { receiverLoop(ptr); });

            std::lock_guard<std::mutex> lock(m_clientsMutex);
            m_clients.push_back(std::move(client));
        }
    }

    // ── Receiver Loop (per client) ───────────────────────────────────

    void ServiceBase::receiverLoop(ClientConn *client)
    {
        while (m_running.load(std::memory_order_acquire))
        {
            if (platform::recvSignal(client->conn.socketFd) != 0)
            {
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

                // Build and send response.
                FrameHeader response{};
                response.version = kProtocolVersion;
                response.flags = FRAME_RESPONSE;
                response.serviceId = header.serviceId;
                response.messageId = header.messageId;
                response.seq = header.seq;
                response.payloadBytes = static_cast<uint32_t>(responsePayload.size());
                response.aux = static_cast<uint32_t>(status);

                writeFrame(client->conn.txRing, response, responsePayload.data(),
                           response.payloadBytes);
                platform::sendSignal(client->conn.socketFd);
            }
        }
    }

    // ── Notification Broadcast ───────────────────────────────────────

    int ServiceBase::sendNotify(uint32_t serviceId, uint32_t messageId, const uint8_t *payload,
                                uint32_t payloadBytes)
    {
        FrameHeader header{};
        header.version = kProtocolVersion;
        header.flags = FRAME_NOTIFY;
        header.serviceId = serviceId;
        header.messageId = messageId;
        header.payloadBytes = payloadBytes;

        std::lock_guard<std::mutex> lock(m_clientsMutex);
        for (auto &c : m_clients)
        {
            if (!c->conn.valid())
            {
                continue;
            }

            int rc = writeFrame(c->conn.txRing, header, payload, payloadBytes);
            if (rc != IPC_SUCCESS)
            {
                return rc;
            }

            if (platform::sendSignal(c->conn.socketFd) != 0)
            {
                return IPC_ERR_DISCONNECTED;
            }
        }

        return IPC_SUCCESS;
    }

} // namespace ms::ipc
