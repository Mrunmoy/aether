#pragma once

#include "ITransport.h"
#include "Types.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace aether::ipc
{

    // Transport-agnostic client base class.
    //
    // Provides the same synchronous call() / onNotification() model as
    // ClientBase, but works with any ITransport implementation rather
    // than being tied to shared-memory ring buffers.
    //
    // IMPORTANT — Destructor contract:
    //   The base destructor calls disconnect() to join the receiver thread.
    //   However, the receiver thread may invoke onNotification() which is
    //   virtual.  By the time ~TransportClientBase() runs, the derived
    //   destructor has already executed and derived members are destroyed.
    //   Therefore, **derived classes MUST call disconnect() in their own
    //   destructor** to ensure the receiver thread is stopped before any
    //   derived members are destroyed.
    //
    // Usage:
    //   1. Construct with a service name.
    //   2. Create and connect an ITransport (handshake must be complete).
    //   3. Call connect(std::move(transport)).
    //   4. Use call() for synchronous RPC; override onNotification()
    //      to handle server-pushed notifications.
    //   5. Call disconnect() when done.

    class TransportClientBase
    {
    public:
        explicit TransportClientBase(const char *serviceName);
        virtual ~TransportClientBase();

        TransportClientBase(const TransportClientBase &) = delete;
        TransportClientBase &operator=(const TransportClientBase &) = delete;

        // ── Lifecycle ────────────────────────────────────────────────

        // Takes ownership of an already-connected transport.
        // The transport must have completed any handshake before this call.
        bool connect(std::unique_ptr<ITransport> transport);
        void disconnect();
        bool isConnected() const;

        // ── Synchronous RPC ─────────────────────────────────────────

        int call(uint32_t serviceId, uint32_t messageId,
                 const std::vector<uint8_t> &request,
                 std::vector<uint8_t> *response,
                 uint32_t timeoutMs = 2000);

    protected:
        // ── Notification callback ───────────────────────────────────
        // Called on the receiver thread for each incoming FRAME_NOTIFY.
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
        uint32_t nextUniqueSeq(); // caller must hold m_pendingMutex

        std::string m_serviceName;
        std::unique_ptr<ITransport> m_transport;
        std::atomic<bool> m_running{false};
        std::atomic<uint32_t> m_nextSeq{1};
        std::thread m_receiverThread;
        std::mutex m_lifecycleMutex; // serializes connect()/disconnect() transitions
        std::mutex m_sendMutex;
        std::mutex m_pendingMutex;
        std::unordered_map<uint32_t, std::shared_ptr<PendingCall>> m_pending;
    };

} // namespace aether::ipc
