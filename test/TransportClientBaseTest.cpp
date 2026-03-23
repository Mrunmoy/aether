/*
 * TransportClientBaseTest.cpp — Google Test suite for TransportClientBase.
 *
 * Validates the transport-agnostic client using MockTransport:
 * connect/disconnect lifecycle, synchronous call(), timeout handling,
 * notification dispatch, and shutdown behavior.
 */

#include <gtest/gtest.h>

#include "MockTransport.h"
#include "TransportClientBase.h"

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

using namespace aether::ipc;

static void settle()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

// ── Helpers ──────────────────────────────────────────────────────────

// Subclass that captures notifications for testing.
class TestTransportClient : public TransportClientBase
{
public:
    using TransportClientBase::TransportClientBase;

    struct Notification
    {
        uint32_t serviceId;
        uint32_t messageId;
        std::vector<uint8_t> payload;
    };

    std::mutex notifyMutex;
    std::vector<Notification> notifications;

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        std::lock_guard<std::mutex> lock(notifyMutex);
        notifications.push_back({serviceId, messageId, payload});
    }
};

// ── Tests ────────────────────────────────────────────────────────────

TEST(TransportClientBaseTest, ConnectWithMockTransport)
{
    TestTransportClient client("test");

    auto transport = std::make_unique<MockTransport>();
    EXPECT_TRUE(client.connect(std::move(transport)));
    EXPECT_TRUE(client.isConnected());

    client.disconnect();
    EXPECT_FALSE(client.isConnected());
}

TEST(TransportClientBaseTest, ConnectNullTransportFails)
{
    TestTransportClient client("test");
    EXPECT_FALSE(client.connect(nullptr));
    EXPECT_FALSE(client.isConnected());
}

TEST(TransportClientBaseTest, ConnectShutdownTransportFails)
{
    TestTransportClient client("test");

    auto transport = std::make_unique<MockTransport>();
    transport->shutdown();
    EXPECT_FALSE(client.connect(std::move(transport)));
    EXPECT_FALSE(client.isConnected());
}

TEST(TransportClientBaseTest, DoubleConnectFails)
{
    TestTransportClient client("test");

    auto t1 = std::make_unique<MockTransport>();
    EXPECT_TRUE(client.connect(std::move(t1)));

    auto t2 = std::make_unique<MockTransport>();
    EXPECT_FALSE(client.connect(std::move(t2)));

    client.disconnect();
}

TEST(TransportClientBaseTest, CallSendsCorrectHeader)
{
    TestTransportClient client("test");
    auto transport = std::make_unique<MockTransport>();
    MockTransport *mock = transport.get();

    ASSERT_TRUE(client.connect(std::move(transport)));
    settle();

    // Inject a response that the receiver will pick up after call() sends.
    // We need a thread to inject the response once we see the sent frame.
    std::thread responder([mock]
    {
        // Wait for the client to send a frame.
        for (int i = 0; i < 100; ++i)
        {
            auto sent = mock->sentFrames();
            if (!sent.empty())
            {
                FrameHeader resp{};
                resp.version = kProtocolVersion;
                resp.flags = FRAME_RESPONSE;
                resp.serviceId = sent[0].header.serviceId;
                resp.messageId = sent[0].header.messageId;
                resp.seq = sent[0].header.seq;
                resp.payloadBytes = 3;
                resp.aux = 0;

                std::vector<uint8_t> payload = {0xAA, 0xBB, 0xCC};
                mock->injectFrame(resp, payload);
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });

    std::vector<uint8_t> request = {1, 2, 3, 4};
    std::vector<uint8_t> response;
    int rc = client.call(42, 7, request, &response, 2000);

    responder.join();

    EXPECT_EQ(IPC_SUCCESS, rc);
    ASSERT_EQ(3u, response.size());
    EXPECT_EQ(0xAA, response[0]);
    EXPECT_EQ(0xBB, response[1]);
    EXPECT_EQ(0xCC, response[2]);

    // Verify sent frame header.
    auto sent = mock->sentFrames();
    ASSERT_EQ(1u, sent.size());
    EXPECT_EQ(kProtocolVersion, sent[0].header.version);
    EXPECT_EQ(FRAME_REQUEST, sent[0].header.flags);
    EXPECT_EQ(42u, sent[0].header.serviceId);
    EXPECT_EQ(7u, sent[0].header.messageId);
    EXPECT_EQ(4u, sent[0].header.payloadBytes);
    ASSERT_EQ(4u, sent[0].payload.size());
    EXPECT_EQ(1, sent[0].payload[0]);

    client.disconnect();
}

TEST(TransportClientBaseTest, CallTimeout)
{
    TestTransportClient client("test");
    auto transport = std::make_unique<MockTransport>();

    ASSERT_TRUE(client.connect(std::move(transport)));
    settle();

    // No response injected — should timeout.
    std::vector<uint8_t> request = {1};
    std::vector<uint8_t> response;
    int rc = client.call(0, 0, request, &response, 100);

    EXPECT_EQ(IPC_ERR_TIMEOUT, rc);

    client.disconnect();
}

TEST(TransportClientBaseTest, CallOnDisconnected)
{
    TestTransportClient client("test");

    std::vector<uint8_t> request = {1};
    std::vector<uint8_t> response;
    int rc = client.call(0, 0, request, &response, 100);

    EXPECT_EQ(IPC_ERR_DISCONNECTED, rc);
}

TEST(TransportClientBaseTest, NotificationDispatch)
{
    TestTransportClient client("test");
    auto transport = std::make_unique<MockTransport>();
    MockTransport *mock = transport.get();

    ASSERT_TRUE(client.connect(std::move(transport)));
    settle();

    // Inject a notification.
    FrameHeader notif{};
    notif.version = kProtocolVersion;
    notif.flags = FRAME_NOTIFY;
    notif.serviceId = 100;
    notif.messageId = 200;
    notif.payloadBytes = 2;

    std::vector<uint8_t> payload = {0x01, 0x02};
    mock->injectFrame(notif, payload);
    settle();

    {
        std::lock_guard<std::mutex> lock(client.notifyMutex);
        ASSERT_EQ(1u, client.notifications.size());
        EXPECT_EQ(100u, client.notifications[0].serviceId);
        EXPECT_EQ(200u, client.notifications[0].messageId);
        ASSERT_EQ(2u, client.notifications[0].payload.size());
        EXPECT_EQ(0x01, client.notifications[0].payload[0]);
        EXPECT_EQ(0x02, client.notifications[0].payload[1]);
    }

    client.disconnect();
}

TEST(TransportClientBaseTest, DisconnectFailsPendingCalls)
{
    TestTransportClient client("test");
    auto transport = std::make_unique<MockTransport>();

    ASSERT_TRUE(client.connect(std::move(transport)));
    settle();

    // Start a call in a background thread (it will block waiting for response).
    std::atomic<int> result{IPC_SUCCESS};
    std::thread caller([&]
    {
        std::vector<uint8_t> request = {1};
        std::vector<uint8_t> response;
        result.store(client.call(0, 0, request, &response, 5000));
    });

    settle();
    client.disconnect();
    caller.join();

    EXPECT_EQ(IPC_ERR_DISCONNECTED, result.load());
}

TEST(TransportClientBaseTest, ShutdownUnblocksReceiverLoop)
{
    TestTransportClient client("test");
    auto transport = std::make_unique<MockTransport>();
    MockTransport *mock = transport.get();

    ASSERT_TRUE(client.connect(std::move(transport)));
    settle();

    // Shutdown the transport directly (simulates peer going away).
    mock->shutdown();
    settle();

    // The receiver loop should have exited and updated m_running.
    EXPECT_FALSE(client.isConnected());
}

TEST(TransportClientBaseTest, ReconnectAfterDisconnect)
{
    TestTransportClient client("test");

    auto t1 = std::make_unique<MockTransport>();
    ASSERT_TRUE(client.connect(std::move(t1)));
    client.disconnect();
    EXPECT_FALSE(client.isConnected());

    auto t2 = std::make_unique<MockTransport>();
    EXPECT_TRUE(client.connect(std::move(t2)));
    EXPECT_TRUE(client.isConnected());

    client.disconnect();
}

TEST(TransportClientBaseTest, CallReturnsServerStatus)
{
    TestTransportClient client("test");
    auto transport = std::make_unique<MockTransport>();
    MockTransport *mock = transport.get();

    ASSERT_TRUE(client.connect(std::move(transport)));
    settle();

    std::thread responder([mock]
    {
        for (int i = 0; i < 100; ++i)
        {
            auto sent = mock->sentFrames();
            if (!sent.empty())
            {
                FrameHeader resp{};
                resp.version = kProtocolVersion;
                resp.flags = FRAME_RESPONSE;
                resp.serviceId = sent[0].header.serviceId;
                resp.messageId = sent[0].header.messageId;
                resp.seq = sent[0].header.seq;
                resp.payloadBytes = 0;
                resp.aux = 42; // application error code

                mock->injectFrame(resp, {});
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });

    std::vector<uint8_t> request = {1};
    std::vector<uint8_t> response;
    int rc = client.call(0, 0, request, &response, 2000);

    responder.join();

    EXPECT_EQ(42, rc);

    client.disconnect();
}

TEST(TransportClientBaseTest, SendFrameFailureReturnsError)
{
    TestTransportClient client("test");
    auto transport = std::make_unique<MockTransport>();
    MockTransport *mock = transport.get();
    ASSERT_TRUE(client.connect(std::move(transport)));
    settle();

    mock->failNextSend(IPC_ERR_RING_FULL);

    std::vector<uint8_t> req = {1}, resp;
    int rc = client.call(0x1234, 1, req, &resp, 100);
    EXPECT_EQ(IPC_ERR_RING_FULL, rc);

    client.disconnect();
}

TEST(TransportClientBaseTest, MultipleCalls)
{
    TestTransportClient client("test");
    auto transport = std::make_unique<MockTransport>();
    MockTransport *mock = transport.get();

    ASSERT_TRUE(client.connect(std::move(transport)));
    settle();

    // Responder thread that echoes back each request payload.
    std::atomic<bool> responderDone{false};
    std::thread responder([mock, &responderDone]
    {
        int lastSeen = 0;
        while (!responderDone.load(std::memory_order_acquire))
        {
            auto sent = mock->sentFrames();
            for (int i = lastSeen; i < static_cast<int>(sent.size()); ++i)
            {
                FrameHeader resp{};
                resp.version = kProtocolVersion;
                resp.flags = FRAME_RESPONSE;
                resp.serviceId = sent[i].header.serviceId;
                resp.messageId = sent[i].header.messageId;
                resp.seq = sent[i].header.seq;
                resp.payloadBytes = static_cast<uint32_t>(sent[i].payload.size());
                resp.aux = 0;

                mock->injectFrame(resp, sent[i].payload);
            }
            lastSeen = static_cast<int>(sent.size());
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    });

    for (int i = 0; i < 20; ++i)
    {
        uint8_t val = static_cast<uint8_t>(i);
        std::vector<uint8_t> request = {val};
        std::vector<uint8_t> response;

        int rc = client.call(0, 1, request, &response, 2000);
        ASSERT_EQ(IPC_SUCCESS, rc) << "Failed on iteration " << i;
        ASSERT_EQ(1u, response.size());
        EXPECT_EQ(val, response[0]);
    }

    responderDone.store(true, std::memory_order_release);
    responder.join();

    client.disconnect();
}
