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

namespace ms::ipc
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

        std::string m_serviceName;
        ms::RunLoop *m_loop = nullptr;
        int m_listenFd = -1;

        std::atomic<bool> m_running{false};
        std::thread m_acceptThread;

        std::mutex m_clientsMutex;
        std::vector<std::unique_ptr<ClientConn>> m_clients;
    };

} // namespace ms::ipc
