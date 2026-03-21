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

using namespace ms::ipc;
using namespace ms::ipc::platform;

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
