#include <gtest/gtest.h>
#include "ServiceBase.h"
#include "Connection.h"
#include "FrameIO.h"
#include "Platform.h"

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
    auto readNotify = [](Connection &c) -> FrameHeader
    {
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
