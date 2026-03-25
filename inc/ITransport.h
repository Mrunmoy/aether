#pragma once

#include "Types.h"
#include <cstdint>
#include <vector>

namespace aether::ipc
{

    // Thread safety
    // -------------
    //   - sendFrame() and recvFrame() may be called from different threads
    //     (one sender thread, one receiver thread).
    //   - sendFrame() is NOT safe to call concurrently from multiple threads
    //     unless the implementation documents otherwise.
    //   - shutdown() is safe to call from any thread; it unblocks recvFrame().
    //
    // Lifecycle
    // ---------
    //   ITransport implementations are assumed to be in a connected state
    //   when provided to consumers. Connection establishment is transport-
    //   specific and happens before passing the ITransport to a client/service.

    class ITransport
    {
    public:
        virtual ~ITransport() = default;
        ITransport() = default;
        ITransport(const ITransport &) = delete;
        ITransport &operator=(const ITransport &) = delete;

        // Send a complete frame (header + payload).
        // The `payloadBytes` parameter is authoritative for the byte count
        // to transmit; implementations copy it into header.payloadBytes on
        // the wire. Callers SHOULD set header.payloadBytes == payloadBytes.
        // Blocks until fully transmitted or error.
        // Returns: IPC_SUCCESS, IPC_ERR_DISCONNECTED, IPC_ERR_TRANSPORT
        virtual int sendFrame(const FrameHeader &header,
                              const uint8_t *payload, uint32_t payloadBytes) = 0;

        // Receive a complete frame.
        // Blocks until a valid frame arrives, error, or shutdown.
        // Returns: IPC_SUCCESS, IPC_ERR_DISCONNECTED, IPC_ERR_CRC, IPC_ERR_TRANSPORT
        virtual int recvFrame(FrameHeader *header,
                              std::vector<uint8_t> *payload) = 0;

        // Is the transport still connected?
        virtual bool connected() const = 0;

        // Initiate shutdown. Unblocks any thread blocked in recvFrame()
        // or sendFrame(). Thread-safe — may be called from any thread.
        // Implementations should ensure that after shutdown() returns,
        // blocked recv/send calls wake up promptly (returning an error).
        virtual void shutdown() = 0;
    };

} // namespace aether::ipc
