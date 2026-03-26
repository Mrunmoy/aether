#pragma once

#include "ITransport.h"

#include <atomic>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <queue>
#include <vector>

namespace aether::ipc
{

    // In-memory ITransport for testing TransportClientBase.
    //
    // - sendFrame() records sent frames for later inspection.
    // - recvFrame() blocks until a frame is injected via injectFrame()
    //   or the transport is shut down.

    class MockTransport : public ITransport
    {
    public:
        struct Frame
        {
            FrameHeader header;
            std::vector<uint8_t> payload;
        };

        int sendFrame(const FrameHeader &header,
                      const uint8_t *payload,
                      uint32_t payloadBytes) override
        {
            {
                std::lock_guard<std::mutex> lock(m_recvMutex);
                if (m_shutdown)
                {
                    return IPC_ERR_DISCONNECTED;
                }
            }

            {
                std::lock_guard<std::mutex> lock(m_sentMutex);
                if (m_nextSendError != 0)
                {
                    int err = m_nextSendError;
                    m_nextSendError = 0;
                    return err;
                }
            }

            Frame f;
            f.header = header;
            if (payload && payloadBytes > 0)
            {
                f.payload.assign(payload, payload + payloadBytes);
            }

            std::lock_guard<std::mutex> lock(m_sentMutex);
            m_sent.push_back(std::move(f));
            return IPC_SUCCESS;
        }

        int recvFrame(FrameHeader *header,
                      std::vector<uint8_t> *payload) override
        {
            std::unique_lock<std::mutex> lock(m_recvMutex);
            m_recvCv.wait(lock, [this]
            {
                return m_shutdown || !m_recvQueue.empty();
            });

            if (m_shutdown)
            {
                return IPC_ERR_DISCONNECTED;
            }

            Frame f = std::move(m_recvQueue.front());
            m_recvQueue.pop();
            *header = f.header;
            *payload = std::move(f.payload);
            return IPC_SUCCESS;
        }

        bool connected() const override
        {
            std::lock_guard<std::mutex> lock(m_recvMutex);
            return !m_shutdown;
        }

        void shutdown() override
        {
            {
                std::lock_guard<std::mutex> lock(m_recvMutex);
                m_shutdown = true;
            }
            m_recvCv.notify_all();
        }

        // ── Test helpers ────────────────────────────────────────────

        // Inject a frame that recvFrame() will return.
        void injectFrame(const FrameHeader &header,
                         const std::vector<uint8_t> &payload)
        {
            std::lock_guard<std::mutex> lock(m_recvMutex);
            m_recvQueue.push(Frame{header, payload});
            m_recvCv.notify_one();
        }

        // Return all frames sent via sendFrame().
        std::vector<Frame> sentFrames() const
        {
            std::lock_guard<std::mutex> lock(m_sentMutex);
            return m_sent;
        }

        // Return and clear all sent frames.
        std::vector<Frame> takeSentFrames()
        {
            std::lock_guard<std::mutex> lock(m_sentMutex);
            std::vector<Frame> out;
            out.swap(m_sent);
            return out;
        }

        void failNextSend(int errorCode)
        {
            std::lock_guard<std::mutex> lock(m_sentMutex);
            m_nextSendError = errorCode;
        }

    private:
        mutable std::mutex m_sentMutex;
        std::vector<Frame> m_sent;
        int m_nextSendError{0};

        mutable std::mutex m_recvMutex;
        std::condition_variable m_recvCv;
        std::queue<Frame> m_recvQueue;
        bool m_shutdown{false};
    };

} // namespace aether::ipc
