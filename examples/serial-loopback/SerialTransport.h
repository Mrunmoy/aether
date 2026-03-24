#pragma once

// SerialTransport — POSIX serial ITransport implementation
//
// Uses the aether-lite wire format:
//   [0xAA][0x55][4-byte LE length][24-byte header][payload][4-byte CRC32]
//
// The "length" field covers header + payload + CRC (i.e., everything after
// the 6-byte sync+length preamble).

#include <ITransport.h>
#include <Endian.h>
#include <Types.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>

namespace aether::ipc
{

// ── IEEE 802.3 CRC32 (reflected polynomial 0xEDB88320) ──────────────

inline uint32_t crc32Serial(const uint8_t *data, size_t len)
{
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; ++i)
    {
        crc ^= data[i];
        for (int b = 0; b < 8; ++b)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }
    return crc ^ 0xFFFFFFFF;
}

// ── SerialTransport ─────────────────────────────────────────────────

class SerialTransport : public ITransport
{
public:
    // Takes ownership of an already-opened fd (serial port or PTY master).
    explicit SerialTransport(int fd)
        : m_fd(fd)
    {
        // Create a self-pipe for shutdown signaling.
        if (::pipe(m_shutdownPipe) != 0)
        {
            m_fd = -1;
            return;
        }
        // Make the read end non-blocking.
        int flags = ::fcntl(m_shutdownPipe[0], F_GETFL);
        ::fcntl(m_shutdownPipe[0], F_SETFL, flags | O_NONBLOCK);
    }

    ~SerialTransport() override
    {
        shutdown();
        if (m_fd >= 0)
            ::close(m_fd);
        if (m_shutdownPipe[0] >= 0)
            ::close(m_shutdownPipe[0]);
        if (m_shutdownPipe[1] >= 0)
            ::close(m_shutdownPipe[1]);
    }

    // ── ITransport interface ────────────────────────────────────────

    int sendFrame(const FrameHeader &header,
                  const uint8_t *payload, uint32_t payloadBytes) override
    {
        if (!connected())
            return IPC_ERR_DISCONNECTED;

        // Build the wire buffer: sync(2) + length(4) + header(24) + payload + crc(4)
        const uint32_t innerLen = 24 + payloadBytes + 4; // header + payload + CRC
        std::vector<uint8_t> buf(2 + 4 + innerLen);
        uint8_t *p = buf.data();

        // Sync bytes
        *p++ = 0xAA;
        *p++ = 0x55;

        // Length (LE)
        uint32_t leLen = hostToLe32(innerLen);
        std::memcpy(p, &leLen, 4);
        p += 4;

        // Header (convert to LE wire format)
        FrameHeader wireHdr = header;
        wireHdr.payloadBytes = payloadBytes; // ensure consistency
        frameHeaderToWire(&wireHdr);
        std::memcpy(p, &wireHdr, 24);
        p += 24;

        // Payload
        if (payloadBytes > 0 && payload)
        {
            std::memcpy(p, payload, payloadBytes);
            p += payloadBytes;
        }

        // CRC32 over header(wire) + payload
        uint32_t crc = crc32Serial(buf.data() + 6, 24 + payloadBytes);
        uint32_t leCrc = hostToLe32(crc);
        std::memcpy(p, &leCrc, 4);

        // Write atomically (single write call for small frames)
        size_t total = buf.size();
        size_t written = 0;
        while (written < total)
        {
            ssize_t n = ::write(m_fd, buf.data() + written, total - written);
            if (n < 0)
            {
                if (errno == EINTR)
                    continue;
                m_connected = false;
                return IPC_ERR_TRANSPORT;
            }
            if (n == 0)
            {
                m_connected = false;
                return IPC_ERR_DISCONNECTED;
            }
            written += static_cast<size_t>(n);
        }

        return IPC_SUCCESS;
    }

    int recvFrame(FrameHeader *header,
                  std::vector<uint8_t> *payload) override
    {
        // State-machine parser
        enum State
        {
            SYNC1,
            SYNC2,
            LENGTH,
            BODY
        };
        State state = SYNC1;

        uint8_t lenBuf[4]{};
        size_t lenPos = 0;
        uint32_t bodyLen = 0;

        std::vector<uint8_t> body;
        size_t bodyPos = 0;

        while (connected())
        {
            uint8_t byte;
            int rc = readByte(&byte);
            if (rc != 0)
                return rc;

            switch (state)
            {
            case SYNC1:
                if (byte == 0xAA)
                    state = SYNC2;
                break;

            case SYNC2:
                if (byte == 0x55)
                {
                    state = LENGTH;
                    lenPos = 0;
                }
                else if (byte == 0xAA)
                {
                    state = SYNC2; // stay, could be another 0xAA
                }
                else
                {
                    state = SYNC1;
                }
                break;

            case LENGTH:
                lenBuf[lenPos++] = byte;
                if (lenPos == 4)
                {
                    std::memcpy(&bodyLen, lenBuf, 4);
                    bodyLen = leToHost32(bodyLen);

                    // Sanity check: must be at least header(24) + crc(4)
                    if (bodyLen < 28 || bodyLen > 1024 * 1024)
                    {
                        state = SYNC1;
                        break;
                    }
                    body.resize(bodyLen);
                    bodyPos = 0;
                    state = BODY;
                }
                break;

            case BODY:
                body[bodyPos++] = byte;
                if (bodyPos == bodyLen)
                {
                    // Parse: first 24 bytes = header, then payload, last 4 = CRC
                    uint32_t payloadBytes = bodyLen - 24 - 4;

                    // Verify CRC over header + payload
                    uint32_t computed = crc32Serial(body.data(), 24 + payloadBytes);
                    uint32_t received;
                    std::memcpy(&received, body.data() + 24 + payloadBytes, 4);
                    received = leToHost32(received);

                    if (computed != received)
                    {
                        // CRC mismatch — resync
                        state = SYNC1;
                        break;
                    }

                    // Deserialize header
                    std::memcpy(header, body.data(), 24);
                    frameHeaderFromWire(header);

                    // Extract payload
                    payload->assign(body.data() + 24,
                                    body.data() + 24 + payloadBytes);

                    return IPC_SUCCESS;
                }
                break;
            }
        }

        return IPC_ERR_DISCONNECTED;
    }

    bool connected() const override
    {
        return m_connected && m_fd >= 0;
    }

    void shutdown() override
    {
        bool expected = true;
        if (m_connected.compare_exchange_strong(expected, false))
        {
            // Signal the shutdown pipe to unblock poll() in readByte().
            uint8_t sig = 1;
            (void)::write(m_shutdownPipe[1], &sig, 1);
        }
    }

private:
    // Read a single byte, using poll() to allow shutdown interruption.
    int readByte(uint8_t *out)
    {
        while (connected())
        {
            struct pollfd fds[2];
            fds[0].fd = m_fd;
            fds[0].events = POLLIN;
            fds[1].fd = m_shutdownPipe[0];
            fds[1].events = POLLIN;

            int ret = ::poll(fds, 2, 1000); // 1s timeout for periodic check
            if (ret < 0)
            {
                if (errno == EINTR)
                    continue;
                m_connected = false;
                return IPC_ERR_TRANSPORT;
            }

            // Shutdown signaled?
            if (fds[1].revents & POLLIN)
                return IPC_ERR_DISCONNECTED;

            if (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                m_connected = false;
                return IPC_ERR_DISCONNECTED;
            }

            if (fds[0].revents & POLLIN)
            {
                ssize_t n = ::read(m_fd, out, 1);
                if (n == 1)
                    return 0;
                if (n == 0)
                {
                    m_connected = false;
                    return IPC_ERR_DISCONNECTED;
                }
                if (errno == EINTR || errno == EAGAIN)
                    continue;
                m_connected = false;
                return IPC_ERR_TRANSPORT;
            }
        }
        return IPC_ERR_DISCONNECTED;
    }

    int m_fd = -1;
    int m_shutdownPipe[2] = {-1, -1};
    std::atomic<bool> m_connected{true};
};

} // namespace aether::ipc
