#include <gtest/gtest.h>
#include "ClientBase.h"
#include "ServiceBase.h"

#include <chrono>
#include <cstring>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

using namespace ms::ipc;

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

// ── Test service: blocks forever (for timeout testing) ───────────────

class SilentService : public ServiceBase
{
public:
    using ServiceBase::ServiceBase;

    void unblock()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_unblock = true;
        m_cv.notify_all();
    }

protected:
    int onRequest(uint32_t, const std::vector<uint8_t> &, std::vector<uint8_t> *) override
    {
        // Block until unblocked or timeout — client will time out first.
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait_for(lock, std::chrono::seconds(5), [this] { return m_unblock; });
        return IPC_SUCCESS;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_unblock{false};
};

// ── Test client: captures notifications ──────────────────────────────

class TestClient : public ClientBase
{
public:
    using ClientBase::ClientBase;

    struct ReceivedNotify
    {
        uint32_t serviceId;
        uint32_t messageId;
        std::vector<uint8_t> payload;
    };

    std::vector<ReceivedNotify> notifications;
    std::mutex notifyMutex;
    std::condition_variable notifyCv;

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        std::lock_guard<std::mutex> lock(notifyMutex);
        notifications.push_back({serviceId, messageId, payload});
        notifyCv.notify_one();
    }
};

// Small delay to let async operations settle.
static void settle()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

// ═════════════════════════════════════════════════════════════════════
// Connect and disconnect without any calls
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, ConnectAndDisconnect)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    EXPECT_TRUE(client.isConnected());

    client.disconnect();
    EXPECT_FALSE(client.isConnected());

    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Single echo call
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, SingleCall)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    const std::vector<uint8_t> request = {'H', 'e', 'l', 'l', 'o'};
    std::vector<uint8_t> response;
    int rc = client.call(1, 1, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), request.size());
    EXPECT_EQ(response, request);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Invalid method returns error status
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, InvalidMethodReturnsError)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    const std::vector<uint8_t> request = {0};
    std::vector<uint8_t> response;
    int rc = client.call(1, 99, request, &response);

    EXPECT_EQ(rc, IPC_ERR_INVALID_METHOD);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Multiple sequential calls
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, MultipleCalls)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    for (uint32_t i = 1; i <= 3; ++i)
    {
        std::vector<uint8_t> request(4);
        std::memcpy(request.data(), &i, sizeof(i));

        std::vector<uint8_t> response;
        int rc = client.call(1, 1, request, &response);

        ASSERT_EQ(rc, IPC_SUCCESS);
        ASSERT_EQ(response.size(), sizeof(i));

        uint32_t val = 0;
        std::memcpy(&val, response.data(), sizeof(val));
        EXPECT_EQ(val, i);
    }

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Call timeout
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, CallTimeout)
{
    SilentService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    const std::vector<uint8_t> request = {1};
    std::vector<uint8_t> response;
    int rc = client.call(1, 1, request, &response, 100);

    EXPECT_EQ(rc, IPC_ERR_TIMEOUT);

    client.disconnect();
    svc.unblock();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Disconnect fails pending calls
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, DisconnectFailsPendingCalls)
{
    SilentService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    int callResult = IPC_SUCCESS;
    std::thread callThread([&] {
        const std::vector<uint8_t> request = {1};
        std::vector<uint8_t> response;
        callResult = client.call(1, 1, request, &response, 5000);
    });

    // Give the call time to register and block.
    settle();

    // Disconnect should unblock the pending call.
    client.disconnect();
    callThread.join();

    EXPECT_TRUE(callResult == IPC_ERR_DISCONNECTED || callResult == IPC_ERR_TIMEOUT);

    svc.unblock();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Receive notification
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, ReceiveNotification)
{
    NotifyTestService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    TestClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    const uint8_t payload[] = "notification";
    ASSERT_EQ(svc.testNotify(77, payload, sizeof(payload)), IPC_SUCCESS);

    // Wait for the notification to arrive.
    {
        std::unique_lock<std::mutex> lock(client.notifyMutex);
        ASSERT_TRUE(client.notifyCv.wait_for(lock, std::chrono::milliseconds(500),
                                             [&] { return !client.notifications.empty(); }));
    }

    ASSERT_EQ(client.notifications.size(), 1u);
    EXPECT_EQ(client.notifications[0].messageId, 77u);
    ASSERT_EQ(client.notifications[0].payload.size(), sizeof(payload));
    EXPECT_EQ(std::memcmp(client.notifications[0].payload.data(), payload, sizeof(payload)), 0);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Call after disconnect returns error
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, CallAfterDisconnect)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    client.disconnect();

    const std::vector<uint8_t> request = {1};
    std::vector<uint8_t> response;
    int rc = client.call(1, 1, request, &response);

    EXPECT_EQ(rc, IPC_ERR_DISCONNECTED);

    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Server stop disconnects client
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, ServerStopDisconnectsClient)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    svc.stop();

    // Give receiver thread time to detect disconnect.
    settle();

    // Client should detect that the connection is gone.
    // A call should fail (receiver loop has exited).
    const std::vector<uint8_t> request = {1};
    std::vector<uint8_t> response;
    int rc = client.call(1, 1, request, &response, 100);

    EXPECT_NE(rc, IPC_SUCCESS);

    client.disconnect();
}
