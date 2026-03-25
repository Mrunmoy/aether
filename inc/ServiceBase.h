#pragma once

#include "Connection.h"
#include "FrameIO.h"
#include "Types.h"

#include <RunLoop.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace aether::ipc
{

    // Server-side base class for IPC services.
    //
    // Generated FooSkeleton inherits from ServiceBase and implements onRequest()
    // as a switch on messageId. The user subclass (FooService) inherits from
    // FooSkeleton and implements the concrete handler methods.
    //
    // Threading model:
    //   - One accept thread waits for new client connections
    //   - One receiver thread per connected client reads request frames,
    //     dispatches via onRequest(), and writes response frames

    class ServiceBase
    {
    public:
        explicit ServiceBase(const char *serviceName, ms::RunLoop *loop = nullptr);
        virtual ~ServiceBase();

        ServiceBase(const ServiceBase &) = delete;
        ServiceBase &operator=(const ServiceBase &) = delete;

        // ── Lifecycle ────────────────────────────────────────────────

        bool start();
        void stop();
        bool isRunning() const;

        // Set maximum number of concurrent client connections.
        // 0 = unlimited (default). Thread-safe; may be called while running.
        void setMaxClients(uint32_t max);

        // ── Peer Credential Filter ───────────────────────────────────
        // Restrict accepted connections to peers with the given UID.
        // Thread-safe (atomic). Linux only (SO_PEERCRED). No-op on other platforms.
        void setAllowedPeerUid(uint32_t uid);  // enable filter
        void clearPeerUidFilter();              // disable filter (allow all)

    protected:
        // ── Virtual dispatch point ───────────────────────────────────
        // Called on the receiver thread for each incoming FRAME_REQUEST.
        // Subclass implements this as a switch on messageId.
        //
        // Returns: IPC_SUCCESS or user-defined error code (stored in response aux).
        virtual int onRequest(uint32_t messageId, const std::vector<uint8_t> &request,
                              std::vector<uint8_t> *response) = 0;

        // ── Notification broadcast ───────────────────────────────────
        // Send a FRAME_NOTIFY to all connected clients.
        // Called by generated notifyXxx() methods in the skeleton.
        int sendNotify(uint32_t serviceId, uint32_t messageId, const uint8_t *payload,
                       uint32_t payloadBytes);

    private:
        struct ClientConn
        {
            Connection conn;
            std::thread thread;
            std::mutex handlerMutex; // guards RunLoop handler execution
            std::mutex sendMutex;    // serializes all server→client txRing writes (SPSC)
            std::atomic<bool> dead{false}; // set when client disconnects
        };

        void acceptLoop();
        void receiverLoop(ClientConn *client);

        void onAcceptReady();
        void onClientReady(ClientConn *client);
        void removeClient(ClientConn *client);

        // Returns true if the peer on socketFd passes the UID filter.
        // Always returns true when no filter is set or on non-Linux platforms.
        bool checkPeerUid(platform::Handle socketFd);

        std::string m_serviceName;
        ms::RunLoop *m_loop = nullptr;
        platform::Handle m_listenFd = platform::kInvalidHandle;

        std::atomic<bool> m_running{false};
        std::thread m_acceptThread;

        std::atomic<uint32_t> m_maxClients{0}; // 0 = unlimited

        std::atomic<bool> m_peerUidFilterEnabled{false};
        std::atomic<uint32_t> m_allowedPeerUid{0};

        std::mutex m_clientsMutex;
        std::vector<std::unique_ptr<ClientConn>> m_clients;
    };

} // namespace aether::ipc
