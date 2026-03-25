#pragma once

#include "Endian.h"
#include "ITransport.h"
#include "Types.h"

#include <atomic>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <poll.h>
#include <unistd.h>
#include <vector>

namespace aether::ipc
{

    // IEEE 802.3 CRC-32 (polynomial 0xEDB88320 reflected).
    // Init 0xFFFFFFFF, final XOR 0xFFFFFFFF.
    inline uint32_t serialCrc32(const uint8_t *data, size_t len)
    {
        uint32_t crc = 0xFFFFFFFFu;
        for (size_t i = 0; i < len; i++)
        {
            crc ^= data[i];
            for (int j = 0; j < 8; j++)
            {
                crc = (crc >> 1) ^ (0xEDB88320u & (-(crc & 1u)));
            }
        }
        return crc ^ 0xFFFFFFFFu;
    }

    // CRC-32 incremental update (for building CRC across segments).
    inline uint32_t serialCrc32Update(uint32_t crc, const uint8_t *data, size_t len)
    {
        for (size_t i = 0; i < len; i++)
        {
            crc ^= data[i];
            for (int j = 0; j < 8; j++)
            {
                crc = (crc >> 1) ^ (0xEDB88320u & (-(crc & 1u)));
            }
        }
        return crc;
    }

    // ITransport implementation over a file descriptor using the aether wire format:
    //   [0xAA][0x55][4-byte LE length][FrameHeader][payload][CRC32]
    //
    // The length field = sizeof(FrameHeader) + sizeof(payload) + sizeof(CRC).
    // CRC covers the length field + header + payload bytes.
    //
    // Uses poll() with a shutdown pipe to make recvFrame() interruptible.

    class SerialTransport : public ITransport
    {
    public:
        explicit SerialTransport(int fd)
            : m_fd(fd)
        {
            if (::pipe(m_shutdownPipe) != 0)
            {
                m_shutdownPipe[0] = -1;
                m_shutdownPipe[1] = -1;
            }
        }

        ~SerialTransport() override
        {
            shutdown();
            if (m_shutdownPipe[0] >= 0)
                ::close(m_shutdownPipe[0]);
            if (m_shutdownPipe[1] >= 0)
                ::close(m_shutdownPipe[1]);
        }

        int sendFrame(const FrameHeader &header,
                      const uint8_t *payload, uint32_t payloadBytes) override
        {
            if (m_fd < 0 || m_shutdown.load(std::memory_order_acquire))
                return IPC_ERR_TRANSPORT;

            // Guard against integer overflow in length calculation.
            if (payloadBytes > 256 * 1024)
                return IPC_ERR_OVERFLOW;

            // Build wire header (little-endian).
            FrameHeader wireHdr = header;
            frameHeaderToWire(&wireHdr);

            // Frame length = header + payload + CRC.
            uint32_t frameLen = static_cast<uint32_t>(sizeof(FrameHeader)) + payloadBytes + 4;
            uint32_t frameLenLE = hostToLe32(frameLen);

            // Sync bytes.
            uint8_t sync[2] = {0xAA, 0x55};

            // CRC over length + header + payload.
            uint32_t crc = 0xFFFFFFFFu;
            crc = serialCrc32Update(crc, reinterpret_cast<const uint8_t *>(&frameLenLE), 4);
            crc = serialCrc32Update(crc, reinterpret_cast<const uint8_t *>(&wireHdr), sizeof(FrameHeader));
            if (payloadBytes > 0 && payload)
            {
                crc = serialCrc32Update(crc, payload, payloadBytes);
            }
            crc ^= 0xFFFFFFFFu;
            uint32_t crcLE = hostToLe32(crc);

            // Write everything.
            if (!writeAll(sync, 2))
                return IPC_ERR_TRANSPORT;
            if (!writeAll(reinterpret_cast<const uint8_t *>(&frameLenLE), 4))
                return IPC_ERR_TRANSPORT;
            if (!writeAll(reinterpret_cast<const uint8_t *>(&wireHdr), sizeof(FrameHeader)))
                return IPC_ERR_TRANSPORT;
            if (payloadBytes > 0 && payload)
            {
                if (!writeAll(payload, payloadBytes))
                    return IPC_ERR_TRANSPORT;
            }
            if (!writeAll(reinterpret_cast<const uint8_t *>(&crcLE), 4))
                return IPC_ERR_TRANSPORT;

            return IPC_SUCCESS;
        }

        int recvFrame(FrameHeader *header, std::vector<uint8_t> *payload) override
        {
            if (m_fd < 0)
                return IPC_ERR_TRANSPORT;

            // State machine parser.
            enum
            {
                SYNC_0,
                SYNC_1,
                READ_LEN,
                READ_BODY
            } state = SYNC_0;

            uint8_t lenBuf[4]{};
            uint32_t lenPos = 0;
            uint32_t frameLen = 0;
            std::vector<uint8_t> body; // header + payload + CRC
            uint32_t bodyPos = 0;

            while (!m_shutdown.load(std::memory_order_acquire))
            {
                uint8_t byte;
                int n = readByte(&byte);
                if (n <= 0)
                    return IPC_ERR_TRANSPORT;

                switch (state)
                {
                case SYNC_0:
                    if (byte == 0xAA)
                        state = SYNC_1;
                    break;

                case SYNC_1:
                    if (byte == 0x55)
                    {
                        state = READ_LEN;
                        lenPos = 0;
                    }
                    else if (byte == 0xAA)
                    {
                        // stay in SYNC_1
                    }
                    else
                    {
                        state = SYNC_0;
                    }
                    break;

                case READ_LEN:
                    lenBuf[lenPos++] = byte;
                    if (lenPos == 4)
                    {
                        std::memcpy(&frameLen, lenBuf, 4);
                        frameLen = leToHost32(frameLen);

                        // Sanity: min = sizeof(FrameHeader) + 4 (crc)
                        constexpr uint32_t kMinFrame = static_cast<uint32_t>(sizeof(FrameHeader)) + 4;
                        if (frameLen < kMinFrame || frameLen > static_cast<uint32_t>(sizeof(FrameHeader)) + 65536 + 4)
                        {
                            state = SYNC_0;
                            break;
                        }

                        body.resize(frameLen);
                        bodyPos = 0;
                        state = READ_BODY;
                    }
                    break;

                case READ_BODY:
                    body[bodyPos++] = byte;
                    if (bodyPos == frameLen)
                    {
                        // Validate CRC: covers length + header + payload.
                        uint32_t payloadSize = frameLen - static_cast<uint32_t>(sizeof(FrameHeader)) - 4;
                        uint32_t crc = 0xFFFFFFFFu;
                        crc = serialCrc32Update(crc, lenBuf, 4);
                        crc = serialCrc32Update(crc, body.data(), frameLen - 4);
                        crc ^= 0xFFFFFFFFu;

                        uint32_t rxCrc;
                        std::memcpy(&rxCrc, body.data() + frameLen - 4, 4);
                        rxCrc = leToHost32(rxCrc);

                        if (crc != rxCrc)
                        {
                            state = SYNC_0;
                            continue; // re-sync
                        }

                        // Deserialize header.
                        std::memcpy(header, body.data(), sizeof(FrameHeader));
                        frameHeaderFromWire(header);

                        // Validate header's payloadBytes against frame length.
                        if (header->payloadBytes != payloadSize)
                        {
                            state = SYNC_0;
                            continue; // re-sync on mismatch
                        }

                        // Copy payload.
                        payload->resize(payloadSize);
                        if (payloadSize > 0)
                        {
                            std::memcpy(payload->data(), body.data() + sizeof(FrameHeader), payloadSize);
                        }

                        return IPC_SUCCESS;
                    }
                    break;
                }
            }

            return IPC_ERR_TRANSPORT;
        }

        bool connected() const override
        {
            return m_fd >= 0 && !m_shutdown.load(std::memory_order_acquire);
        }

        void shutdown() override
        {
            bool expected = false;
            if (m_shutdown.compare_exchange_strong(expected, true))
            {
                // Write to shutdown pipe to unblock poll().
                if (m_shutdownPipe[1] >= 0)
                {
                    uint8_t dummy = 1;
                    ssize_t rc = ::write(m_shutdownPipe[1], &dummy, 1);
                    if (rc < 0 && errno != EINTR)
                    {
                        // Best-effort wakeup only; shutdown state is already visible.
                    }
                }
            }
        }

    private:
        bool writeAll(const uint8_t *buf, size_t len)
        {
            size_t written = 0;
            while (written < len)
            {
                ssize_t n = ::write(m_fd, buf + written, len - written);
                if (n < 0)
                {
                    if (errno == EINTR)
                        continue;
                    return false;
                }
                if (n == 0)
                    return false;
                written += static_cast<size_t>(n);
            }
            return true;
        }

        // Read a single byte with poll() so we can be interrupted.
        int readByte(uint8_t *out)
        {
            struct pollfd fds[2]{};
            fds[0].fd = m_fd;
            fds[0].events = POLLIN;
            fds[1].fd = m_shutdownPipe[0];
            fds[1].events = POLLIN;

            int ret;
            do
            {
                ret = ::poll(fds, 2, 2000); // 2s timeout
            } while (ret < 0 && errno == EINTR);

            if (ret <= 0)
                return -1;
            if (fds[1].revents & POLLIN)
                return -1; // shutdown signalled
            if (!(fds[0].revents & POLLIN))
                return -1;

            ssize_t n;
            do
            {
                n = ::read(m_fd, out, 1);
            } while (n < 0 && errno == EINTR);

            return (n == 1) ? 1 : -1;
        }

        int m_fd;
        int m_shutdownPipe[2]{-1, -1};
        std::atomic<bool> m_shutdown{false};
    };

} // namespace aether::ipc
