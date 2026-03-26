#include <gtest/gtest.h>
#include "ServiceBase.h"
#include "Connection.h"
#include "FrameIO.h"
#include "Platform.h"
#include "RunLoop.h"

#include <chrono>
#include <cstring>
#include <thread>
#include <vector>

#if !defined(_WIN32)
#include <unistd.h>
#endif

using namespace aether::ipc;
using namespace aether::ipc::platform;

#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

// ── Test service: echoes requests with messageId==1 ──────────────────

class EchoService : public ServiceBase
{
public:
    using ServiceBase::ServiceBase;

protected:
    int onRequest(uint32_t messageId, const std::vector<uint8_t> &request,
                  std::vector<uint8_t> *response) override
    {
        if (messageId == 1)
        {
            *response = request;
            return IPC_SUCCESS;
        }
        return IPC_ERR_INVALID_METHOD;
    }
};

// ── Test service: exposes sendNotify for testing ─────────────────────

class NotifyTestService : public ServiceBase
{
public:
    using ServiceBase::ServiceBase;

    int testNotify(uint32_t messageId, const uint8_t *payload, uint32_t len)
    {
        return sendNotify(1, messageId, payload, len);
    }

protected:
    int onRequest(uint32_t, const std::vector<uint8_t> &, std::vector<uint8_t> *) override
    {
        return IPC_ERR_INVALID_METHOD;
    }
};

// ── Manual client helpers ────────────────────────────────────────────

static FrameHeader makeRequest(uint32_t messageId, uint32_t seq, uint32_t payloadBytes)
{
    FrameHeader hdr{};
    hdr.version = kProtocolVersion;
    hdr.flags = FRAME_REQUEST;
    hdr.serviceId = 1;
    hdr.messageId = messageId;
    hdr.seq = seq;
    hdr.payloadBytes = payloadBytes;
    return hdr;
}

// Send a request and wait for response.
static int sendAndRecv(Connection &conn, uint32_t messageId, uint32_t seq,
                       const uint8_t *reqPayload, uint32_t reqLen, FrameHeader *outHdr,
                       std::vector<uint8_t> *outPayload)
{
    FrameHeader hdr = makeRequest(messageId, seq, reqLen);
    int rc = writeFrame(conn.txRing, hdr, reqPayload, reqLen);
    if (rc != IPC_SUCCESS)
    {
        return rc;
    }

    if (sendSignal(conn.socketFd) != 0)
    {
        return IPC_ERR_DISCONNECTED;
    }

    if (recvSignal(conn.socketFd) != 0)
    {
        return IPC_ERR_DISCONNECTED;
    }

    return readFrameAlloc(conn.rxRing, outHdr, outPayload);
}

// Small delay to let async operations (accept, thread spawn) settle.
static void settle()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

// ═════════════════════════════════════════════════════════════════════
// Start and stop without any connections
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, StartAndStop)
{
    EchoService svc(SVC_NAME);

    ASSERT_TRUE(svc.start());
    EXPECT_TRUE(svc.isRunning());

    svc.stop();
    EXPECT_FALSE(svc.isRunning());
}

// ═════════════════════════════════════════════════════════════════════
// Single echo request/response
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, SingleRequestResponse)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    const uint8_t payload[] = "Hello, Service!";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    int rc = sendAndRecv(client, 1, 42, payload, sizeof(payload), &respHdr, &respPayload);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(respHdr.flags, FRAME_RESPONSE);
    EXPECT_EQ(respHdr.seq, 42u);
    EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));
    ASSERT_EQ(respPayload.size(), sizeof(payload));
    EXPECT_EQ(std::memcmp(respPayload.data(), payload, sizeof(payload)), 0);

    client.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Unknown messageId returns IPC_ERR_INVALID_METHOD in aux
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, InvalidMethodReturnsError)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    uint8_t dummy = 0;
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    int rc = sendAndRecv(client, 99, 1, &dummy, sizeof(dummy), &respHdr, &respPayload);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(respHdr.flags, FRAME_RESPONSE);
    EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_ERR_INVALID_METHOD));

    client.close();
    svc.stop();
}

TEST(ServiceBaseTest, InvalidVersionRequestIsIgnored)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    FrameHeader badHdr = makeRequest(1, 7, 1);
    badHdr.version = kProtocolVersion + 1;
    uint8_t badPayload = 0x42;
    ASSERT_EQ(writeFrame(client.txRing, badHdr, &badPayload, sizeof(badPayload)), IPC_SUCCESS);
    ASSERT_EQ(sendSignal(client.socketFd), 0);

    settle();

    const uint8_t payload[] = "good";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    int rc = sendAndRecv(client, 1, 8, payload, sizeof(payload), &respHdr, &respPayload);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(respHdr.flags, FRAME_RESPONSE);
    EXPECT_EQ(respHdr.seq, 8u);
    EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));
    EXPECT_EQ(respPayload, std::vector<uint8_t>(payload, payload + sizeof(payload)));

    client.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Multiple requests on the same connection
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, MultipleRequestsOnSameConnection)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    for (uint32_t seq = 1; seq <= 3; ++seq)
    {
        uint8_t payload[4];
        std::memcpy(payload, &seq, sizeof(seq));

        FrameHeader respHdr{};
        std::vector<uint8_t> respPayload;
        int rc = sendAndRecv(client, 1, seq, payload, sizeof(payload), &respHdr, &respPayload);

        ASSERT_EQ(rc, IPC_SUCCESS);
        EXPECT_EQ(respHdr.seq, seq);
        ASSERT_EQ(respPayload.size(), sizeof(payload));

        uint32_t val = 0;
        std::memcpy(&val, respPayload.data(), sizeof(val));
        EXPECT_EQ(val, seq);
    }

    client.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Two clients get independent responses
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, MultipleClients)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    Connection client1 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client1.valid());
    Connection client2 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client2.valid());
    settle();

    const uint8_t p1[] = "client1";
    const uint8_t p2[] = "client2";
    FrameHeader h1{}, h2{};
    std::vector<uint8_t> r1, r2;

    ASSERT_EQ(sendAndRecv(client1, 1, 10, p1, sizeof(p1), &h1, &r1), IPC_SUCCESS);
    ASSERT_EQ(sendAndRecv(client2, 1, 20, p2, sizeof(p2), &h2, &r2), IPC_SUCCESS);

    EXPECT_EQ(h1.seq, 10u);
    EXPECT_EQ(h2.seq, 20u);
    ASSERT_EQ(r1.size(), sizeof(p1));
    ASSERT_EQ(r2.size(), sizeof(p2));
    EXPECT_EQ(std::memcmp(r1.data(), p1, sizeof(p1)), 0);
    EXPECT_EQ(std::memcmp(r2.data(), p2, sizeof(p2)), 0);

    client1.close();
    client2.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Notification broadcast to multiple clients
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, NotifyBroadcast)
{
    NotifyTestService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    Connection client1 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client1.valid());
    Connection client2 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client2.valid());
    settle();

    const uint8_t payload[] = "notification";
    ASSERT_EQ(svc.testNotify(77, payload, sizeof(payload)), IPC_SUCCESS);

    // Both clients should receive the notification.
    auto readNotify = [](Connection &c) -> FrameHeader {
        recvSignal(c.socketFd);
        FrameHeader hdr{};
        std::vector<uint8_t> p;
        readFrameAlloc(c.rxRing, &hdr, &p);
        return hdr;
    };

    FrameHeader n1 = readNotify(client1);
    FrameHeader n2 = readNotify(client2);

    EXPECT_EQ(n1.flags, FRAME_NOTIFY);
    EXPECT_EQ(n1.messageId, 77u);
    EXPECT_EQ(n2.flags, FRAME_NOTIFY);
    EXPECT_EQ(n2.messageId, 77u);

    client1.close();
    client2.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Stop cleans up connections — client detects disconnect
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, StopCleansUpConnections)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    svc.stop();

    // Client should detect disconnect.
    EXPECT_NE(recvSignal(client.socketFd), 0);

    client.close();
}

// ═════════════════════════════════════════════════════════════════════
// Client disconnect does not crash the service
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, ClientDisconnectDoesNotCrash)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    // Connect and immediately disconnect.
    {
        Connection client1 = connectToServer(SVC_NAME);
        ASSERT_TRUE(client1.valid());
        settle();
        client1.close();
    }

    settle();

    // Service should still work for a new client.
    Connection client2 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client2.valid());
    settle();

    const uint8_t payload[] = "still alive";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    int rc = sendAndRecv(client2, 1, 1, payload, sizeof(payload), &respHdr, &respPayload);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));
    ASSERT_EQ(respPayload.size(), sizeof(payload));
    EXPECT_EQ(std::memcmp(respPayload.data(), payload, sizeof(payload)), 0);

    client2.close();
    svc.stop();
}

// ═══════════════════════════════════════════════════════════════════════
// RunLoop-mode tests
// ═══════════════════════════════════════════════════════════════════════

#if !defined(_WIN32)

// Helper: run loop in background, auto-stop on scope exit.
struct RunLoopGuard
{
    ms::RunLoop &loop;
    std::thread thread;

    explicit RunLoopGuard(ms::RunLoop &l) : loop(l), thread([&l] { l.run(); }) {}

    ~RunLoopGuard()
    {
        loop.stop();
        if (thread.joinable())
            thread.join();
    }
};

// ═════════════════════════════════════════════════════════════════════
// Service on RunLoop: accept and respond to echo request
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, RunLoop_AcceptAndRespond)
{
    ms::RunLoop loop;
    loop.init("SvcRL");

    EchoService svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    const uint8_t payload[] = "Hello, RunLoop!";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    int rc = sendAndRecv(client, 1, 42, payload, sizeof(payload), &respHdr, &respPayload);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(respHdr.flags, FRAME_RESPONSE);
    EXPECT_EQ(respHdr.seq, 42u);
    EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));
    ASSERT_EQ(respPayload.size(), sizeof(payload));
    EXPECT_EQ(std::memcmp(respPayload.data(), payload, sizeof(payload)), 0);

    client.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Service on RunLoop: multiple clients get independent responses
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, RunLoop_MultipleClients)
{
    ms::RunLoop loop;
    loop.init("SvcRLMulti");

    EchoService svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    Connection client1 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client1.valid());
    Connection client2 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client2.valid());
    settle();

    const uint8_t p1[] = "client1";
    const uint8_t p2[] = "client2";
    FrameHeader h1{}, h2{};
    std::vector<uint8_t> r1, r2;

    ASSERT_EQ(sendAndRecv(client1, 1, 10, p1, sizeof(p1), &h1, &r1), IPC_SUCCESS);
    ASSERT_EQ(sendAndRecv(client2, 1, 20, p2, sizeof(p2), &h2, &r2), IPC_SUCCESS);

    EXPECT_EQ(h1.seq, 10u);
    EXPECT_EQ(h2.seq, 20u);
    ASSERT_EQ(r1.size(), sizeof(p1));
    ASSERT_EQ(r2.size(), sizeof(p2));
    EXPECT_EQ(std::memcmp(r1.data(), p1, sizeof(p1)), 0);
    EXPECT_EQ(std::memcmp(r2.data(), p2, sizeof(p2)), 0);

    client1.close();
    client2.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Service on RunLoop: client disconnect does not crash
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, RunLoop_ClientDisconnect)
{
    ms::RunLoop loop;
    loop.init("SvcRLDisc");

    EchoService svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    // Connect and immediately disconnect.
    {
        Connection client1 = connectToServer(SVC_NAME);
        ASSERT_TRUE(client1.valid());
        settle();
        client1.close();
    }

    settle();

    // Service should still work for a new client.
    Connection client2 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client2.valid());
    settle();

    const uint8_t payload[] = "still alive";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    int rc = sendAndRecv(client2, 1, 1, payload, sizeof(payload), &respHdr, &respPayload);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));
    ASSERT_EQ(respPayload.size(), sizeof(payload));
    EXPECT_EQ(std::memcmp(respPayload.data(), payload, sizeof(payload)), 0);

    client2.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Service on RunLoop: stop cleans up — client detects disconnect
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, RunLoop_StopCleansUp)
{
    ms::RunLoop loop;
    loop.init("SvcRLStop");

    EchoService svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    svc.stop();

    // Client should detect disconnect.
    EXPECT_NE(recvSignal(client.socketFd), 0);

    client.close();
}

TEST(ServiceBaseTest, RunLoop_StopAfterClientMarkedDead)
{
    ms::RunLoop loop;
    loop.init("SvcRLStopDead");

    EchoService svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    client.close();
    settle();

    svc.stop();
    EXPECT_FALSE(svc.isRunning());
}

#else

TEST(ServiceBaseTest, RunLoopModeNotSupportedOnWindows)
{
    ms::RunLoop loop;
    loop.init("SvcRLUnsupported");

    EchoService svc(SVC_NAME, &loop);
    EXPECT_FALSE(svc.start());
    EXPECT_FALSE(svc.isRunning());
}

#endif

// ═════════════════════════════════════════════════════════════════════
// Notify to abruptly dead client does not SIGPIPE/crash
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, NotifyToAbruptlyDeadClient_NoSIGPIPE)
{
    NotifyTestService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    // Connect a client, let the server register it, then close abruptly
    // (no graceful shutdown — simulates a crashed process).
    {
        Connection client = connectToServer(SVC_NAME);
        ASSERT_TRUE(client.valid());
        settle();
        client.close();
    }
    settle();

    // Notify should not crash (no SIGPIPE) and should return successfully.
    const uint8_t payload[] = "ping";
    int rc = svc.testNotify(1, payload, sizeof(payload));
    EXPECT_EQ(rc, IPC_SUCCESS);

    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Notify reaps dead clients — second call has no stale entries
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, NotifyReapsDeadClients)
{
    NotifyTestService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    // Connect two clients.
    Connection client1 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client1.valid());
    Connection client2 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client2.valid());
    settle();

    // Kill the first client abruptly.
    client1.close();
    settle();

    // First notify: reaps the dead client. Should not crash.
    const uint8_t payload[] = "reap";
    int rc1 = svc.testNotify(10, payload, sizeof(payload));
    EXPECT_EQ(rc1, IPC_SUCCESS);

    // Second notify: dead client is already reaped. Should succeed cleanly.
    int rc2 = svc.testNotify(11, payload, sizeof(payload));
    EXPECT_EQ(rc2, IPC_SUCCESS);

    client2.close();
    svc.stop();
}

TEST(ServiceBaseTest, ResponseBackpressureDisconnectsClient)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    bool sawDisconnect = false;
    const uint8_t payload[] = {'x'};

    for (uint32_t seq = 1; seq <= 20000; ++seq)
    {
        FrameHeader hdr = makeRequest(1, seq, sizeof(payload));
        if (writeFrame(client.txRing, hdr, payload, sizeof(payload)) != IPC_SUCCESS
            || sendSignal(client.socketFd) != 0)
        {
            sawDisconnect = true;
            break;
        }

        if (recvSignal(client.socketFd) != 0)
        {
            sawDisconnect = true;
            break;
        }
    }

    ASSERT_TRUE(sawDisconnect) << "service did not disconnect the stuck client";

    Connection healthyClient = connectToServer(SVC_NAME);
    ASSERT_TRUE(healthyClient.valid());
    settle();

    const uint8_t healthyPayload[] = "healthy";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    ASSERT_EQ(sendAndRecv(healthyClient, 1, 999, healthyPayload, sizeof(healthyPayload),
                          &respHdr, &respPayload),
              IPC_SUCCESS);
    EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));
    EXPECT_EQ(respPayload,
              std::vector<uint8_t>(healthyPayload, healthyPayload + sizeof(healthyPayload)));

    client.close();
    healthyClient.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Rapid connect/disconnect cycles — service stays healthy
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, ConnectDisconnectCycle)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    for (int i = 0; i < 20; ++i)
    {
        Connection client = connectToServer(SVC_NAME);
        ASSERT_TRUE(client.valid()) << "iteration " << i;
        settle();

        const uint8_t payload[] = "cycle";
        FrameHeader respHdr{};
        std::vector<uint8_t> respPayload;
        int rc = sendAndRecv(client, 1, static_cast<uint32_t>(i), payload, sizeof(payload),
                             &respHdr, &respPayload);

        ASSERT_EQ(rc, IPC_SUCCESS) << "iteration " << i;
        EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));
        ASSERT_EQ(respPayload.size(), sizeof(payload));
        EXPECT_EQ(std::memcmp(respPayload.data(), payload, sizeof(payload)), 0);

        client.close();
        settle();
    }

    EXPECT_TRUE(svc.isRunning());
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Stop without start — no crash
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, StopWithoutStart)
{
    EchoService svc(SVC_NAME);
    // Never started — stop() should be harmless.
    EXPECT_FALSE(svc.isRunning());
    svc.stop();
    EXPECT_FALSE(svc.isRunning());
}

// ═════════════════════════════════════════════════════════════════════
// Start twice — second start returns false (or is idempotent)
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, StartTwice)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    EXPECT_TRUE(svc.isRunning());

    // Second start: the socket is already bound, so serverSocket returns -1
    // and start() returns false.
    bool secondStart = svc.start();
    EXPECT_FALSE(secondStart);
    EXPECT_TRUE(svc.isRunning());

    // Verify the service is still healthy after the failed second start.
    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    const uint8_t payload[] = "still-alive";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    int rc = sendAndRecv(client, 1, 0, payload, sizeof(payload), &respHdr, &respPayload);
    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));

    client.close();
    settle();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// MaxClients: default unlimited — can connect many clients
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, MaxClientsDefaultUnlimited)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    std::vector<Connection> clients;
    for (int i = 0; i < 5; ++i)
    {
        Connection c = connectToServer(SVC_NAME);
        ASSERT_TRUE(c.valid()) << "client " << i << " failed to connect";
        clients.push_back(std::move(c));
        settle();
    }

    // Verify each client can communicate.
    for (int i = 0; i < 5; ++i)
    {
        const uint8_t payload[] = "ping";
        FrameHeader respHdr{};
        std::vector<uint8_t> respPayload;
        int rc = sendAndRecv(clients[static_cast<size_t>(i)], 1,
                             static_cast<uint32_t>(i), payload, sizeof(payload),
                             &respHdr, &respPayload);
        ASSERT_EQ(rc, IPC_SUCCESS) << "client " << i;
        EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));
    }

    for (auto &c : clients)
        c.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// MaxClients: rejects excess connections
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, MaxClientsRejectsExcess)
{
    EchoService svc(SVC_NAME);
    svc.setMaxClients(2);
    ASSERT_TRUE(svc.start());

    Connection client1 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client1.valid());
    settle();
    Connection client2 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client2.valid());
    settle();

    // Third connection should be rejected (server closes it during handshake
    // or immediately after accept).
    Connection client3 = connectToServer(SVC_NAME);
    settle();

    if (client3.valid())
    {
        // The server accepted the socket but should close it shortly.
        // Try to use it — expect failure.
        const uint8_t payload[] = "x";
        FrameHeader respHdr{};
        std::vector<uint8_t> respPayload;
        int rc = sendAndRecv(client3, 1, 99, payload, sizeof(payload),
                             &respHdr, &respPayload);
        EXPECT_NE(rc, IPC_SUCCESS) << "3rd client should not get a response";
        client3.close();
    }

    // First two clients should still work.
    const uint8_t p1[] = "one";
    const uint8_t p2[] = "two";
    FrameHeader h1{}, h2{};
    std::vector<uint8_t> r1, r2;

    ASSERT_EQ(sendAndRecv(client1, 1, 1, p1, sizeof(p1), &h1, &r1), IPC_SUCCESS);
    ASSERT_EQ(sendAndRecv(client2, 1, 2, p2, sizeof(p2), &h2, &r2), IPC_SUCCESS);
    EXPECT_EQ(h1.aux, static_cast<uint32_t>(IPC_SUCCESS));
    EXPECT_EQ(h2.aux, static_cast<uint32_t>(IPC_SUCCESS));

    client1.close();
    client2.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// MaxClients: allows reconnect after disconnect
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, MaxClientsAllowsReconnectAfterDisconnect)
{
    EchoService svc(SVC_NAME);
    svc.setMaxClients(1);
    ASSERT_TRUE(svc.start());

    Connection client1 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client1.valid());
    settle();

    // Verify it works.
    const uint8_t payload[] = "hello";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    ASSERT_EQ(sendAndRecv(client1, 1, 1, payload, sizeof(payload),
                          &respHdr, &respPayload),
              IPC_SUCCESS);

    // Disconnect.
    client1.close();
    // Wait for the server to reap the dead client.
    settle();
    settle();

    // New client should succeed.
    Connection client2 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client2.valid());
    settle();

    FrameHeader respHdr2{};
    std::vector<uint8_t> respPayload2;
    ASSERT_EQ(sendAndRecv(client2, 1, 2, payload, sizeof(payload),
                          &respHdr2, &respPayload2),
              IPC_SUCCESS);
    EXPECT_EQ(respHdr2.aux, static_cast<uint32_t>(IPC_SUCCESS));

    client2.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop: MaxClients rejects excess connections
// ═════════════════════════════════════════════════════════════════════

#if !defined(_WIN32)

// ═════════════════════════════════════════════════════════════════════
// RunLoop: dead clients are reaped when a new client connects
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, RunLoop_DeadClientsReapedOnAccept)
{
    ms::RunLoop loop;
    loop.init("SvcRLReap");

    EchoService svc(SVC_NAME, &loop);
    // Set maxClients=1 so we can observe the reap: if the dead entry
    // for client1 is NOT reaped, client2 would be rejected.
    svc.setMaxClients(1);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    // Connect client1 and verify it works.
    Connection client1 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client1.valid());
    settle();

    const uint8_t payload[] = "ping";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    ASSERT_EQ(sendAndRecv(client1, 1, 1, payload, sizeof(payload),
                          &respHdr, &respPayload),
              IPC_SUCCESS);
    EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));

    // Disconnect client1 — triggers removeClient (marks dead).
    client1.close();
    settle();

    // Connect client2 — onAcceptReady should reap client1's dead entry
    // before checking maxClients, so this connection succeeds.
    Connection client2 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client2.valid());
    settle();

    // Verify client2 can communicate.
    FrameHeader respHdr2{};
    std::vector<uint8_t> respPayload2;
    ASSERT_EQ(sendAndRecv(client2, 1, 2, payload, sizeof(payload),
                          &respHdr2, &respPayload2),
              IPC_SUCCESS);
    EXPECT_EQ(respHdr2.aux, static_cast<uint32_t>(IPC_SUCCESS));
    EXPECT_EQ(respPayload2, std::vector<uint8_t>(payload, payload + sizeof(payload)));

    client2.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop: many connect/disconnect cycles — no dead entry buildup
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, RunLoop_ManyConnectDisconnectCycles)
{
    ms::RunLoop loop;
    loop.init("SvcRLCyc");

    EchoService svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    for (int i = 0; i < 10; ++i)
    {
        Connection client = connectToServer(SVC_NAME);
        ASSERT_TRUE(client.valid()) << "iteration " << i;
        settle();

        const uint8_t payload[] = "cycle";
        FrameHeader respHdr{};
        std::vector<uint8_t> respPayload;
        int rc = sendAndRecv(client, 1, static_cast<uint32_t>(i), payload,
                             sizeof(payload), &respHdr, &respPayload);

        ASSERT_EQ(rc, IPC_SUCCESS) << "iteration " << i;
        EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));
        ASSERT_EQ(respPayload.size(), sizeof(payload));
        EXPECT_EQ(std::memcmp(respPayload.data(), payload, sizeof(payload)), 0);

        client.close();
        settle();
    }

    EXPECT_TRUE(svc.isRunning());
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop: MaxClients rejects excess connections
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, RunLoop_MaxClientsRejectsExcess)
{
    ms::RunLoop loop;
    loop.init("SvcRLMax");

    EchoService svc(SVC_NAME, &loop);
    svc.setMaxClients(2);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    Connection client1 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client1.valid());
    settle();
    Connection client2 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client2.valid());
    settle();

    // Third connection should be rejected.
    Connection client3 = connectToServer(SVC_NAME);
    settle();

    if (client3.valid())
    {
        const uint8_t payload[] = "x";
        FrameHeader respHdr{};
        std::vector<uint8_t> respPayload;
        int rc = sendAndRecv(client3, 1, 99, payload, sizeof(payload),
                             &respHdr, &respPayload);
        EXPECT_NE(rc, IPC_SUCCESS) << "3rd client should not get a response";
        client3.close();
    }

    // First two clients should still work.
    const uint8_t p1[] = "one";
    const uint8_t p2[] = "two";
    FrameHeader h1{}, h2{};
    std::vector<uint8_t> r1, r2;

    ASSERT_EQ(sendAndRecv(client1, 1, 1, p1, sizeof(p1), &h1, &r1), IPC_SUCCESS);
    ASSERT_EQ(sendAndRecv(client2, 1, 2, p2, sizeof(p2), &h2, &r2), IPC_SUCCESS);
    EXPECT_EQ(h1.aux, static_cast<uint32_t>(IPC_SUCCESS));
    EXPECT_EQ(h2.aux, static_cast<uint32_t>(IPC_SUCCESS));

    client1.close();
    client2.close();
    svc.stop();
}

#endif

// ═════════════════════════════════════════════════════════════════════
// Peer UID filter tests
// ═════════════════════════════════════════════════════════════════════

// ═════════════════════════════════════════════════════════════════════
// Notification sequence numbers are monotonically increasing
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, NotifySequenceIsMonotonic)
{
    NotifyTestService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    constexpr int kNotifications = 5;
    const uint8_t payload[] = "seq-test";

    for (int i = 0; i < kNotifications; ++i)
    {
        ASSERT_EQ(svc.testNotify(static_cast<uint32_t>(i + 1), payload, sizeof(payload)),
                  IPC_SUCCESS);
    }

    // Read all notification frames and verify aux contains 1, 2, 3, 4, 5.
    // On Windows, recvSignal drains all queued bytes so we can only rely on
    // one wakeup.  Wait for the first signal, then read all available frames.
    std::vector<uint32_t> seqs;
    ASSERT_EQ(recvSignal(client.socketFd), 0) << "initial signal";

    for (int i = 0; i < kNotifications; ++i)
    {
        FrameHeader hdr{};
        std::vector<uint8_t> p;
        // Frames may arrive slightly after the signal on slow CI; brief spin.
        int rc = IPC_ERR_TIMEOUT;
        for (int attempt = 0; attempt < 50 && rc != IPC_SUCCESS; ++attempt)
        {
            rc = readFrameAlloc(client.rxRing, &hdr, &p);
            if (rc != IPC_SUCCESS)
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        ASSERT_EQ(rc, IPC_SUCCESS) << "frame " << i;
        EXPECT_EQ(hdr.flags, FRAME_NOTIFY);
        seqs.push_back(hdr.aux);
    }

    ASSERT_EQ(seqs.size(), static_cast<size_t>(kNotifications));
    for (int i = 0; i < kNotifications; ++i)
    {
        EXPECT_EQ(seqs[static_cast<size_t>(i)], static_cast<uint32_t>(i + 1))
            << "notification " << i << " has wrong sequence number";
    }

    client.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Notify does not block accept — sendNotify releases m_clientsMutex
// before iterating clients, so acceptLoop can add new connections.
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, NotifyDoesNotBlockAccept)
{
    NotifyTestService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    Connection client1 = connectToServer(SVC_NAME);
    ASSERT_TRUE(client1.valid());
    settle();

    // Start a thread that sends notifications in a loop.
    std::atomic<bool> notifyDone{false};
    std::thread notifier([&] {
        const uint8_t payload[] = "notify";
        for (int i = 0; i < 10; ++i)
        {
            svc.testNotify(static_cast<uint32_t>(i), payload, sizeof(payload));
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        notifyDone.store(true, std::memory_order_release);
    });

    // While notifications are running, connect a second client.
    // This should succeed quickly (< 500ms) if sendNotify doesn't
    // hold m_clientsMutex for the entire broadcast loop.
    auto t0 = std::chrono::steady_clock::now();
    Connection client2 = connectToServer(SVC_NAME);
    auto t1 = std::chrono::steady_clock::now();

    ASSERT_TRUE(client2.valid());
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    EXPECT_LT(elapsed.count(), 500) << "connecting client2 took too long (" << elapsed.count()
                                    << " ms); sendNotify may be blocking accept";

    notifier.join();

    // Drain any pending notifications on client1.
    // (We don't care about content, just ensure no crash.)
    client1.close();
    client2.close();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Notify with concurrent disconnect — no crash, no hang
// ═════════════════════════════════════════════════════════════════════

TEST(ServiceBaseTest, NotifyWithConcurrentDisconnect)
{
    NotifyTestService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    // Connect 3 clients.
    Connection clients[3];
    for (auto &c : clients)
    {
        c = connectToServer(SVC_NAME);
        ASSERT_TRUE(c.valid());
    }
    settle();

    // Start notifications in a background thread.
    std::atomic<bool> notifyDone{false};
    std::thread notifier([&] {
        const uint8_t payload[] = "concurrent";
        for (int i = 0; i < 20; ++i)
        {
            svc.testNotify(static_cast<uint32_t>(i), payload, sizeof(payload));
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
        notifyDone.store(true, std::memory_order_release);
    });

    // Disconnect clients one by one while notifications are in flight.
    for (auto &c : clients)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        c.close();
    }

    notifier.join();

    // Service should still be healthy — no crash, no hang.
    EXPECT_TRUE(svc.isRunning());
    svc.stop();
}

#if !defined(_WIN32) && !defined(__APPLE__)

TEST(ServiceBaseTest, PeerUidFilterRejectsDifferentUid)
{
    EchoService svc(SVC_NAME);

    svc.setAllowedPeerUid(99999);
    ASSERT_TRUE(svc.start());
    settle();

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    const uint8_t payload[] = "rejected";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    int rc = sendAndRecv(client, 1, 1, payload, sizeof(payload), &respHdr, &respPayload);
    EXPECT_NE(rc, IPC_SUCCESS);

    client.close();

    svc.stop();
    EchoService svc2("PeerUidAllow");
    svc2.setAllowedPeerUid(static_cast<uint32_t>(getuid()));
    ASSERT_TRUE(svc2.start());
    settle();

    Connection client2 = connectToServer("PeerUidAllow");
    ASSERT_TRUE(client2.valid());
    settle();

    const uint8_t payload2[] = "allowed";
    FrameHeader respHdr2{};
    std::vector<uint8_t> respPayload2;
    rc = sendAndRecv(client2, 1, 2, payload2, sizeof(payload2), &respHdr2, &respPayload2);
    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(respHdr2.aux, static_cast<uint32_t>(IPC_SUCCESS));

    client2.close();
    svc2.stop();
}

TEST(ServiceBaseTest, PeerUidFilterDefaultAllowsAll)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    Connection client = connectToServer(SVC_NAME);
    ASSERT_TRUE(client.valid());
    settle();

    const uint8_t payload[] = "default-allows";
    FrameHeader respHdr{};
    std::vector<uint8_t> respPayload;
    int rc = sendAndRecv(client, 1, 1, payload, sizeof(payload), &respHdr, &respPayload);
    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(respHdr.aux, static_cast<uint32_t>(IPC_SUCCESS));
    ASSERT_EQ(respPayload.size(), sizeof(payload));
    EXPECT_EQ(std::memcmp(respPayload.data(), payload, sizeof(payload)), 0);

    client.close();
    svc.stop();
}

#endif // !_WIN32 && !__APPLE__
