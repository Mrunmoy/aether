#include <gtest/gtest.h>
#include "Connection.h"
#include "Platform.h"

#include <atomic>
#include <cstring>
#include <thread>

using namespace aether::ipc;
using namespace aether::ipc::platform;

// Unique service name per test to avoid socket collisions.
#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

// Helper: run server accept in a background thread.
struct AcceptThread
{
    Handle listenFd;
    Connection conn;
    std::thread thread;

    explicit AcceptThread(Handle fd) : listenFd(fd)
    {
        thread = std::thread([this] { conn = acceptConnection(listenFd); });
    }

    void join()
    {
        if (thread.joinable())
            thread.join();
    }

    ~AcceptThread() { join(); }
};

// ═════════════════════════════════════════════════════════════════════
// Basic handshake — server + client connect, both get valid connections
// ═════════════════════════════════════════════════════════════════════

TEST(ConnectionTest, BasicHandshake)
{
    Handle srv = serverSocket(SVC_NAME);
    ASSERT_TRUE(isValidHandle(srv));

    AcceptThread acceptor(srv);

    Connection client = connectToServer(SVC_NAME);
    acceptor.join();

    ASSERT_TRUE(client.valid());
    ASSERT_TRUE(acceptor.conn.valid());

    EXPECT_NE(client.txRing, nullptr);
    EXPECT_NE(client.rxRing, nullptr);
    EXPECT_NE(acceptor.conn.txRing, nullptr);
    EXPECT_NE(acceptor.conn.rxRing, nullptr);

    // Client's tx should be server's rx (same underlying memory).
    // Client's rx should be server's tx.
    // We verify this by writing data on one side and reading on the other.

    client.close();
    acceptor.conn.close();
    closeFd(srv);
}

// ═════════════════════════════════════════════════════════════════════
// Data flow — write through client tx, read from server rx (and back)
// ═════════════════════════════════════════════════════════════════════

TEST(ConnectionTest, RingBufferDataFlow)
{
    Handle srv = serverSocket(SVC_NAME);
    ASSERT_TRUE(isValidHandle(srv));

    AcceptThread acceptor(srv);
    Connection client = connectToServer(SVC_NAME);
    acceptor.join();

    ASSERT_TRUE(client.valid());
    ASSERT_TRUE(acceptor.conn.valid());

    // Client → Server direction
    {
        const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
        ASSERT_TRUE(client.txRing->write(data, 4));

        uint8_t buf[4] = {};
        ASSERT_TRUE(acceptor.conn.rxRing->read(buf, 4));
        EXPECT_EQ(std::memcmp(data, buf, 4), 0);
    }

    // Server → Client direction
    {
        const uint8_t data[] = {0xCA, 0xFE, 0xBA, 0xBE};
        ASSERT_TRUE(acceptor.conn.txRing->write(data, 4));

        uint8_t buf[4] = {};
        ASSERT_TRUE(client.rxRing->read(buf, 4));
        EXPECT_EQ(std::memcmp(data, buf, 4), 0);
    }

    client.close();
    acceptor.conn.close();
    closeFd(srv);
}

// ═════════════════════════════════════════════════════════════════════
// Version mismatch — client sends wrong version, server rejects
// ═════════════════════════════════════════════════════════════════════

TEST(ConnectionTest, VersionMismatch)
{
    Handle srv = serverSocket(SVC_NAME);
    ASSERT_TRUE(isValidHandle(srv));

    // Server accept thread
    Connection serverConn;
    std::thread serverThread([&] { serverConn = acceptConnection(srv); });

    // Manual client handshake with wrong version.
    Handle clientFd = clientSocket(SVC_NAME);
    ASSERT_TRUE(isValidHandle(clientFd));

    Handle shmFd = shmCreate(2 * sizeof(IpcRing));
    ASSERT_TRUE(isValidHandle(shmFd));

    SharedMemoryHandshake hs{};
    hs.version = 999;
    EXPECT_EQ(sendFd(clientFd, shmFd, &hs, sizeof(hs)), 0);

    // Server should reject — recvSignal will fail because server closes socket.
    // Give server thread time to process.
    serverThread.join();

    EXPECT_FALSE(serverConn.valid());

    closeFd(shmFd);
    closeFd(clientFd);
    closeFd(srv);
}

// ═════════════════════════════════════════════════════════════════════
// Multiple clients — each gets independent ring buffers
// ═════════════════════════════════════════════════════════════════════

TEST(ConnectionTest, MultipleClients)
{
    Handle srv = serverSocket(SVC_NAME);
    ASSERT_TRUE(isValidHandle(srv));

    constexpr int N = 3;
    Connection serverConns[N];
    Connection clientConns[N];

    for (int i = 0; i < N; ++i)
    {
        std::thread serverThread([&, i] { serverConns[i] = acceptConnection(srv); });

        clientConns[i] = connectToServer(SVC_NAME);
        serverThread.join();

        ASSERT_TRUE(clientConns[i].valid()) << "Client " << i << " failed";
        ASSERT_TRUE(serverConns[i].valid()) << "Server " << i << " failed";
    }

    // Write unique data on each client, verify each server connection
    // receives its own data (isolation).
    for (int i = 0; i < N; ++i)
    {
        uint8_t val = static_cast<uint8_t>(i + 1);
        ASSERT_TRUE(clientConns[i].txRing->write(&val, 1));

        uint8_t readVal = 0;
        ASSERT_TRUE(serverConns[i].rxRing->read(&readVal, 1));
        EXPECT_EQ(readVal, val) << "Client " << i << " data mismatch";
    }

    for (int i = 0; i < N; ++i)
    {
        clientConns[i].close();
        serverConns[i].close();
    }
    closeFd(srv);
}

// ═════════════════════════════════════════════════════════════════════
// Connection close — verify cleanup
// ═════════════════════════════════════════════════════════════════════

TEST(ConnectionTest, ConnectionClose)
{
    Handle srv = serverSocket(SVC_NAME);
    ASSERT_TRUE(isValidHandle(srv));

    AcceptThread acceptor(srv);
    Connection client = connectToServer(SVC_NAME);
    acceptor.join();

    ASSERT_TRUE(client.valid());

    client.close();

    EXPECT_FALSE(client.valid());
    EXPECT_EQ(client.socketFd, kInvalidHandle);
    EXPECT_EQ(client.shmFd, kInvalidHandle);
    EXPECT_EQ(client.shmBase, nullptr);
    EXPECT_EQ(client.txRing, nullptr);
    EXPECT_EQ(client.rxRing, nullptr);

    acceptor.conn.close();
    closeFd(srv);
}

// ═════════════════════════════════════════════════════════════════════
// Double close — calling close() twice must not crash
// ═════════════════════════════════════════════════════════════════════

TEST(ConnectionTest, DoubleClose)
{
    Handle srv = serverSocket(SVC_NAME);
    ASSERT_TRUE(isValidHandle(srv));

    AcceptThread acceptor(srv);
    Connection client = connectToServer(SVC_NAME);
    acceptor.join();

    ASSERT_TRUE(client.valid());

    client.close();
    EXPECT_FALSE(client.valid());

    // Second close — must not crash or double-free.
    client.close();
    EXPECT_FALSE(client.valid());
    EXPECT_EQ(client.socketFd, kInvalidHandle);
    EXPECT_EQ(client.shmFd, kInvalidHandle);
    EXPECT_EQ(client.shmBase, nullptr);
    EXPECT_EQ(client.shmSize, 0u);
    EXPECT_EQ(client.txRing, nullptr);
    EXPECT_EQ(client.rxRing, nullptr);

    acceptor.conn.close();
    closeFd(srv);
}

// ═════════════════════════════════════════════════════════════════════
// Move assignment — destination valid, source invalidated
// ═════════════════════════════════════════════════════════════════════

TEST(ConnectionTest, MoveAssignment)
{
    Handle srv = serverSocket(SVC_NAME);
    ASSERT_TRUE(isValidHandle(srv));

    AcceptThread acceptor(srv);
    Connection client = connectToServer(SVC_NAME);
    acceptor.join();

    ASSERT_TRUE(client.valid());

    Connection dest;
    dest = std::move(client);

    // Destination should be valid.
    EXPECT_TRUE(dest.valid());
    EXPECT_TRUE(isValidHandle(dest.socketFd));
    EXPECT_NE(dest.shmBase, nullptr);
    EXPECT_NE(dest.txRing, nullptr);
    EXPECT_NE(dest.rxRing, nullptr);

    // Source should be invalidated.
    EXPECT_FALSE(client.valid());
    EXPECT_EQ(client.socketFd, kInvalidHandle);
    EXPECT_EQ(client.shmBase, nullptr);
    EXPECT_EQ(client.txRing, nullptr);
    EXPECT_EQ(client.rxRing, nullptr);

    dest.close();
    acceptor.conn.close();
    closeFd(srv);
}

// ═════════════════════════════════════════════════════════════════════
// Move constructor — destination valid, source invalidated
// ═════════════════════════════════════════════════════════════════════

TEST(ConnectionTest, MoveConstructor)
{
    Handle srv = serverSocket(SVC_NAME);
    ASSERT_TRUE(isValidHandle(srv));

    AcceptThread acceptor(srv);
    Connection client = connectToServer(SVC_NAME);
    acceptor.join();

    ASSERT_TRUE(client.valid());

    Connection dest(std::move(client));

    // Destination should be valid.
    EXPECT_TRUE(dest.valid());
    EXPECT_TRUE(isValidHandle(dest.socketFd));
    EXPECT_NE(dest.shmBase, nullptr);
    EXPECT_NE(dest.txRing, nullptr);
    EXPECT_NE(dest.rxRing, nullptr);

    // Source should be invalidated.
    EXPECT_FALSE(client.valid());
    EXPECT_EQ(client.socketFd, kInvalidHandle);
    EXPECT_EQ(client.shmBase, nullptr);
    EXPECT_EQ(client.txRing, nullptr);
    EXPECT_EQ(client.rxRing, nullptr);

    dest.close();
    acceptor.conn.close();
    closeFd(srv);
}
