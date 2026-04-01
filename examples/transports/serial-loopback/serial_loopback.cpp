// serial_loopback.cpp — PTY-based serial loopback demo
//
// Creates a pseudo-terminal pair, runs an echo server on the slave side,
// and uses SerialTransport + TransportClientBase on the master side to
// send RPC calls that are echoed back.

#include "SerialTransport.h"
#include <TransportClientBase.h>
#include <Types.h>

#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#if defined(__APPLE__)
#include <util.h>
#else
#include <pty.h>
#endif
#include <termios.h>
#include <unistd.h>

using namespace aether::ipc;

// ── Echo server (runs on the slave PTY fd) ──────────────────────────
// Reads frames and echoes them back with FRAME_RESPONSE flag.

static void echoServer(int fd)
{
    SerialTransport transport(fd);

    // Perform handshake before entering echo loop.
    if (transport.waitForHandshake() != IPC_SUCCESS)
    {
        std::fprintf(stderr, "Server handshake failed\n");
        return;
    }

    while (transport.connected())
    {
        FrameHeader hdr{};
        std::vector<uint8_t> payload;

        int rc = transport.recvFrame(&hdr, &payload);
        if (rc != IPC_SUCCESS)
            break;

        // Turn the request into a response
        hdr.flags = FRAME_RESPONSE;

        rc = transport.sendFrame(hdr, payload.data(),
                                 static_cast<uint32_t>(payload.size()));
        if (rc != IPC_SUCCESS)
            break;
    }
}

// ── Configure raw mode on a PTY fd ─────────────────────────────────

static void configureRaw(int fd)
{
    struct termios tio{};
    if (tcgetattr(fd, &tio) == 0)
    {
        cfmakeraw(&tio);
        tio.c_cc[VMIN] = 1;
        tio.c_cc[VTIME] = 0;
        tcsetattr(fd, TCSANOW, &tio);
    }
}

// ── Main ────────────────────────────────────────────────────────────

int main()
{
    std::printf("=== Serial Loopback Example ===\n\n");

    // 1. Create PTY pair
    int master = -1, slave = -1;
    if (openpty(&master, &slave, nullptr, nullptr, nullptr) != 0)
    {
        std::perror("openpty");
        return 1;
    }

    std::printf("PTY pair created: master fd=%d, slave fd=%d\n", master, slave);

    // Configure both ends for raw mode (no line buffering, no echo)
    configureRaw(master);
    configureRaw(slave);

    // 2. Start echo server on the slave side
    std::thread serverThread(echoServer, slave);

    // 3. Create transport on the master side
    auto transport = std::make_unique<SerialTransport>(master);

    // 4. Perform handshake to negotiate max payload
    int hsRc = transport->handshake();
    if (hsRc != IPC_SUCCESS)
    {
        std::fprintf(stderr, "Client handshake failed: %d\n", hsRc);
        ::close(slave);
        serverThread.join();
        return 1;
    }

    std::printf("Handshake complete (maxPayload=%u).\n",
                transport->negotiatedMaxPayload());

    // 5. Create TransportClientBase and connect
    TransportClientBase client("serial-echo");
    if (!client.connect(std::move(transport)))
    {
        std::fprintf(stderr, "Failed to connect TransportClientBase\n");
        // Clean up: shut down the server thread before exiting.
        ::close(slave);
        serverThread.join();
        return 1;
    }

    std::printf("Client connected.\n\n");

    // 6. Send a few RPC calls
    const char *messages[] = {
        "Hello, serial world!",
        "aether IPC over UART",
        "PTY loopback test",
    };

    for (int i = 0; i < 3; ++i)
    {
        const char *msg = messages[i];
        std::vector<uint8_t> request(
            reinterpret_cast<const uint8_t *>(msg),
            reinterpret_cast<const uint8_t *>(msg) + std::strlen(msg));

        std::vector<uint8_t> response;

        std::printf("  TX [svc=1 msg=%d]: \"%s\"\n", i + 1, msg);

        int rc = client.call(
            /*serviceId=*/1,
            /*messageId=*/static_cast<uint32_t>(i + 1),
            request, &response, /*timeoutMs=*/2000);

        if (rc == IPC_SUCCESS)
        {
            std::string resp(response.begin(), response.end());
            std::printf("  RX [seq ok]:      \"%s\"\n\n", resp.c_str());
        }
        else
        {
            std::fprintf(stderr, "  RX error: %d\n\n", rc);
        }
    }

    // 7. Clean disconnect
    std::printf("Disconnecting...\n");
    client.disconnect();
    serverThread.join();

    std::printf("Done.\n");
    return 0;
}
