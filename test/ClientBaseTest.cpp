#include <gtest/gtest.h>
#include "ClientBase.h"
#include "ServiceBase.h"
#include "RunLoop.h"

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

// ═════════════════════════════════════════════════════════════════════
// isConnected() returns false after server stops (receiver loop detects
// the disconnect and sets m_running = false)
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, IsConnectedFalseAfterServerStop)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    EXPECT_TRUE(client.isConnected());

    svc.stop();

    // Give receiver thread time to detect the disconnect.
    settle();

    // Receiver loop should have set m_running = false.
    EXPECT_FALSE(client.isConnected());

    client.disconnect();
}

// ═════════════════════════════════════════════════════════════════════
// sendNotify skips dead clients — live client still receives the
// notification even when a previously-connected client has gone away
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, NotifySkipsDeadClient)
{
    NotifyTestService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    // First client connects, then disconnects.
    {
        TestClient dead(SVC_NAME);
        ASSERT_TRUE(dead.connect());
        settle();
        dead.disconnect();
    }
    settle();

    // Second client stays connected.
    TestClient alive(SVC_NAME);
    ASSERT_TRUE(alive.connect());
    settle();

    // Broadcast: sendNotify should succeed and the live client should receive.
    const uint8_t payload[] = "hello";
    ASSERT_EQ(svc.testNotify(42, payload, sizeof(payload)), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(alive.notifyMutex);
        ASSERT_TRUE(alive.notifyCv.wait_for(lock, std::chrono::milliseconds(500),
                                            [&] { return !alive.notifications.empty(); }));
    }
    ASSERT_EQ(alive.notifications.size(), 1u);
    EXPECT_EQ(alive.notifications[0].messageId, 42u);

    alive.disconnect();
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
// Client on RunLoop: echo call and response
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, RunLoop_CallAndResponse)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ms::RunLoop loop;
    loop.init("CliRL");

    ClientBase client(SVC_NAME, &loop);
    ASSERT_TRUE(client.connect());

    RunLoopGuard guard(loop);
    settle();

    // call() from a non-RunLoop thread (this thread).
    const std::vector<uint8_t> request = {'H', 'e', 'l', 'l', 'o'};
    std::vector<uint8_t> response;
    int rc = client.call(1, 1, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(response, request);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Client on RunLoop: notification delivered
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, RunLoop_Notification)
{
    NotifyTestService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ms::RunLoop loop;
    loop.init("CliRLNotify");

    TestClient client(SVC_NAME, &loop);
    ASSERT_TRUE(client.connect());

    RunLoopGuard guard(loop);
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
// Client on RunLoop: clean disconnect
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, RunLoop_Disconnect)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ms::RunLoop loop;
    loop.init("CliRLDisc");

    ClientBase client(SVC_NAME, &loop);
    ASSERT_TRUE(client.connect());

    RunLoopGuard guard(loop);
    settle();

    EXPECT_TRUE(client.isConnected());

    client.disconnect();
    EXPECT_FALSE(client.isConnected());

    // Call after disconnect should fail.
    const std::vector<uint8_t> request = {1};
    std::vector<uint8_t> response;
    int rc = client.call(1, 1, request, &response);
    EXPECT_EQ(rc, IPC_ERR_DISCONNECTED);

    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Both service and client on the SAME RunLoop
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, RunLoop_BothOnSameRunLoop)
{
    ms::RunLoop loop;
    loop.init("BothRL");

    EchoService svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME, &loop);
    ASSERT_TRUE(client.connect());
    settle();

    // call() from this thread (not the RunLoop thread).
    const std::vector<uint8_t> request = {'B', 'o', 't', 'h'};
    std::vector<uint8_t> response;
    int rc = client.call(1, 1, request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    EXPECT_EQ(response, request);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Concurrent calls from the same client (tests m_sendMutex)
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, ConcurrentCallsFromSameClient)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    constexpr int kThreads = 2;
    constexpr int kCallsPerThread = 4;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    std::atomic<int> failCount{0};

    for (int t = 0; t < kThreads; ++t)
    {
        threads.emplace_back([&, t] {
            for (int c = 0; c < kCallsPerThread; ++c)
            {
                uint32_t tag = static_cast<uint32_t>(t * kCallsPerThread + c);
                std::vector<uint8_t> request(4);
                std::memcpy(request.data(), &tag, sizeof(tag));

                std::vector<uint8_t> response;
                int rc = client.call(1, 1, request, &response, 10000);

                if (rc == IPC_SUCCESS && response == request)
                    successCount.fetch_add(1);
                else
                    failCount.fetch_add(1);
            }
        });
    }

    for (auto &th : threads)
        th.join();

    EXPECT_EQ(successCount.load(), kThreads * kCallsPerThread);
    EXPECT_EQ(failCount.load(), 0);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Connect to a nonexistent service returns false
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, ConnectToNonexistentService)
{
    ClientBase client("nonexistent_service_xyz");
    EXPECT_FALSE(client.connect());
    EXPECT_FALSE(client.isConnected());
}

// ═════════════════════════════════════════════════════════════════════
// Double disconnect — no crash or deadlock
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, DoubleDisconnect)
{
    EchoService svc(SVC_NAME);
    ASSERT_TRUE(svc.start());

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    client.disconnect();
    EXPECT_FALSE(client.isConnected());

    // Second disconnect should be harmless.
    client.disconnect();
    EXPECT_FALSE(client.isConnected());

    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// Server stop during call — call returns error, does not hang
// ═════════════════════════════════════════════════════════════════════

TEST(ClientBaseTest, ServerStopDuringCall)
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

    // Give the call time to register and block in onRequest.
    settle();

    // Stop the server in a background thread (stop() will block until
    // the receiver thread's onRequest returns).
    std::thread stopThread([&] {
        svc.unblock(); // let onRequest finish so stop() can join
        svc.stop();
    });

    callThread.join();
    stopThread.join();

    // The call should have completed — either successfully (if the response
    // was sent before the connection was torn down) or with an error.
    // The key assertion: neither thread hung forever.
    EXPECT_TRUE(callResult == IPC_SUCCESS || callResult == IPC_ERR_DISCONNECTED ||
                callResult == IPC_ERR_TIMEOUT)
        << "callResult = " << callResult;

    client.disconnect();
}
