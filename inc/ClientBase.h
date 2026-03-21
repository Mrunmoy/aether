#pragma once

#include "Connection.h"
#include "FrameIO.h"
#include "Types.h"

#include <RunLoop.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace ms::ipc
{

    // Client-side base class for IPC services.
    //
    // Generated FooClient inherits from ClientBase and provides typed
    // call wrappers (e.g., fooMethod(req) -> response) and virtual
    // onNotifyXxx() callbacks. Users subclass FooClient to handle
    // notifications.
    //
    // Threading model:
    //   - One receiver thread reads response and notification frames
    //   - call() blocks the caller until the response arrives or timeout
    //   - Sequence numbers correlate requests to responses

    class ClientBase
    {
    public:
        explicit ClientBase(const char *serviceName, ms::RunLoop *loop = nullptr);
        virtual ~ClientBase();

        ClientBase(const ClientBase &) = delete;
        ClientBase &operator=(const ClientBase &) = delete;

        // ── Lifecycle ────────────────────────────────────────────────

        bool connect();
        void disconnect();
        bool isConnected() const;

        // ── Synchronous RPC ─────────────────────────────────────────

        // Send a request and block until the response arrives or timeout.
        // Returns IPC_SUCCESS on success, or an error code:
        //   IPC_ERR_DISCONNECTED — not connected or connection lost
        //   IPC_ERR_TIMEOUT      — no response within timeoutMs
        //   IPC_ERR_RING_FULL    — ring buffer full
        //   positive/negative    — server-returned status (from aux)
        int call(uint32_t serviceId, uint32_t messageId, const std::vector<uint8_t> &request,
                 std::vector<uint8_t> *response, uint32_t timeoutMs = 2000);

    protected:
        // ── Notification callback ───────────────────────────────────
        // Called on the receiver thread for each incoming FRAME_NOTIFY.
        // Generated FooClient overrides this as a switch on messageId.
        // Default implementation does nothing.
        virtual void onNotification(uint32_t serviceId, uint32_t messageId,
                                    const std::vector<uint8_t> &payload);

    private:
        struct PendingCall
        {
            std::condition_variable cv;
            bool done{false};
            int status{IPC_SUCCESS};
            std::vector<uint8_t> response;
        };

        void receiverLoop();
        void onDataReady();

        std::string m_serviceName;
        ms::RunLoop *m_loop = nullptr;
        Connection m_conn;
        std::atomic<bool> m_running{false};
        std::atomic<uint32_t> m_nextSeq{1};
        std::thread m_receiverThread;

        std::mutex m_handlerMutex; // guards RunLoop handler execution
        std::mutex m_sendMutex;   // serializes txRing writes (SPSC invariant)
        std::mutex m_pendingMutex;
        std::unordered_map<uint32_t, std::shared_ptr<PendingCall>> m_pending;
    };

} // namespace ms::ipc
