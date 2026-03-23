#include <gtest/gtest.h>
#include "Connection.h"
#include "FrameIO.h"
#include "Platform.h"

#include <cerrno>
#include <chrono>
#include <cstring>
#include <sys/mman.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace aether::ipc::platform;
using namespace aether::ipc;

#if defined(__APPLE__)
static constexpr int kSignalSocketType = SOCK_STREAM;
#else
static constexpr int kSignalSocketType = SOCK_SEQPACKET;
#endif

// Unique socket name per test to avoid collisions when tests run in parallel.
// Uses the test name provided by gtest.
#define SOCK_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

// ═════════════════════════════════════════════════════════════════════
// UDS Tests
// ═════════════════════════════════════════════════════════════════════

TEST(PlatformTest, ServerSocketCreation)
{
    int fd = serverSocket(SOCK_NAME);
    ASSERT_GE(fd, 0);
    closeFd(fd);
}

TEST(PlatformTest, ClientConnects)
{
    int srv = serverSocket(SOCK_NAME);
    ASSERT_GE(srv, 0);

    int cli = clientSocket(SOCK_NAME);
    ASSERT_GE(cli, 0);

    closeFd(cli);
    closeFd(srv);
}

TEST(PlatformTest, AcceptClient)
{
    int srv = serverSocket(SOCK_NAME);
    ASSERT_GE(srv, 0);

    int cli = clientSocket(SOCK_NAME);
    ASSERT_GE(cli, 0);

    int accepted = acceptClient(srv);
    ASSERT_GE(accepted, 0);

    closeFd(accepted);
    closeFd(cli);
    closeFd(srv);
}

TEST(PlatformTest, SendRecvSignal)
{
    int srv = serverSocket(SOCK_NAME);
    ASSERT_GE(srv, 0);

    int cli = clientSocket(SOCK_NAME);
    ASSERT_GE(cli, 0);

    int accepted = acceptClient(srv);
    ASSERT_GE(accepted, 0);

    // Client sends signal, server receives it.
    EXPECT_EQ(sendSignal(cli), 0);
    EXPECT_EQ(recvSignal(accepted), 0);

    // Server sends signal, client receives it.
    EXPECT_EQ(sendSignal(accepted), 0);
    EXPECT_EQ(recvSignal(cli), 0);

    closeFd(accepted);
    closeFd(cli);
    closeFd(srv);
}

TEST(PlatformTest, SendRecvFd)
{
    int srv = serverSocket(SOCK_NAME);
    ASSERT_GE(srv, 0);

    int cli = clientSocket(SOCK_NAME);
    ASSERT_GE(cli, 0);

    int accepted = acceptClient(srv);
    ASSERT_GE(accepted, 0);

    // Create a shared memory region and write a magic value.
    int shmFd = shmCreate(4096);
    ASSERT_GE(shmFd, 0);

    void *writerMap = mmap(nullptr, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    ASSERT_NE(writerMap, MAP_FAILED);
    uint32_t magic = 0xDEADBEEF;
    std::memcpy(writerMap, &magic, sizeof(magic));

    // Send the FD from client to server with ancillary data.
    uint16_t version = 42;
    EXPECT_EQ(sendFd(cli, shmFd, &version, sizeof(version)), 0);

    // Server receives the FD and ancillary data.
    int receivedFd = -1;
    uint16_t receivedVersion = 0;
    int n = recvFd(accepted, &receivedFd, &receivedVersion, sizeof(receivedVersion));
    EXPECT_GT(n, 0);
    EXPECT_GE(receivedFd, 0);
    EXPECT_EQ(receivedVersion, 42);

    // mmap the received FD and verify the magic value is there.
    void *readerMap = mmap(nullptr, 4096, PROT_READ, MAP_SHARED, receivedFd, 0);
    ASSERT_NE(readerMap, MAP_FAILED);
    uint32_t readMagic = 0;
    std::memcpy(&readMagic, readerMap, sizeof(readMagic));
    EXPECT_EQ(readMagic, 0xDEADBEEF);

    munmap(readerMap, 4096);
    munmap(writerMap, 4096);
    closeFd(receivedFd);
    closeFd(shmFd);
    closeFd(accepted);
    closeFd(cli);
    closeFd(srv);
}

TEST(PlatformTest, SendRecvFdNoAncillaryData)
{
    int srv = serverSocket(SOCK_NAME);
    ASSERT_GE(srv, 0);

    int cli = clientSocket(SOCK_NAME);
    ASSERT_GE(cli, 0);

    int accepted = acceptClient(srv);
    ASSERT_GE(accepted, 0);

    // Send FD with no ancillary data (dataLen = 0).
    int shmFd = shmCreate(4096);
    ASSERT_GE(shmFd, 0);

    EXPECT_EQ(sendFd(cli, shmFd, nullptr, 0), 0);

    int receivedFd = -1;
    int n = recvFd(accepted, &receivedFd, nullptr, 0);
    EXPECT_GT(n, 0);
    EXPECT_GE(receivedFd, 0);

    closeFd(receivedFd);
    closeFd(shmFd);
    closeFd(accepted);
    closeFd(cli);
    closeFd(srv);
}

TEST(PlatformTest, ConnectToNonexistent)
{
    int fd = clientSocket("no_such_service_exists");
    EXPECT_EQ(fd, -1);
}

// ═════════════════════════════════════════════════════════════════════
// Shared Memory Tests
// ═════════════════════════════════════════════════════════════════════

TEST(PlatformTest, ShmCreate)
{
    int fd = shmCreate(4096);
    ASSERT_GE(fd, 0);
    closeFd(fd);
}

TEST(PlatformTest, ShmWriteAndMmap)
{
    int fd = shmCreate(4096);
    ASSERT_GE(fd, 0);

    void *ptr = mmap(nullptr, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ASSERT_NE(ptr, MAP_FAILED);

    // Write a pattern, read it back.
    uint64_t pattern = 0xCAFEBABE12345678ULL;
    std::memcpy(ptr, &pattern, sizeof(pattern));

    uint64_t readback = 0;
    std::memcpy(&readback, ptr, sizeof(readback));
    EXPECT_EQ(readback, pattern);

    munmap(ptr, 4096);
    closeFd(fd);
}

TEST(PlatformTest, ShmZeroSize)
{
    // Zero-sized shared memory regions should still be representable.
    int fd = shmCreate(0);
    ASSERT_GE(fd, 0);
    closeFd(fd);
}

// ═════════════════════════════════════════════════════════════════════
// Close Tests
// ═════════════════════════════════════════════════════════════════════

TEST(PlatformTest, CloseFdNegativeOne)
{
    // Should not crash or fail.
    closeFd(-1);
}

// ═════════════════════════════════════════════════════════════════════
// Closed-socket signal tests (MSG_NOSIGNAL / SIGPIPE safety)
// ═════════════════════════════════════════════════════════════════════

TEST(PlatformTest, SendSignalToClosedSocket)
{
    int fds[2];
    ASSERT_EQ(socketpair(AF_UNIX, kSignalSocketType, 0, fds), 0);

#if defined(__APPLE__)
    int enable = 1;
    ASSERT_EQ(setsockopt(fds[0], SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(enable)), 0);
#endif

    // Close the receiving end.
    closeFd(fds[1]);

    // sendSignal on the remaining end should return -1 (not crash from SIGPIPE).
    EXPECT_EQ(sendSignal(fds[0]), -1);

    closeFd(fds[0]);
}

TEST(PlatformTest, RecvSignalFromClosedSocket)
{
    int fds[2];
    ASSERT_EQ(socketpair(AF_UNIX, kSignalSocketType, 0, fds), 0);

    // Close the sending end.
    closeFd(fds[1]);

    // recvSignal on the remaining end should return -1.
    EXPECT_EQ(recvSignal(fds[0]), -1);

    closeFd(fds[0]);
}

// ═════════════════════════════════════════════════════════════════════
// Socket timeout test
// ═════════════════════════════════════════════════════════════════════

TEST(PlatformTest, SetSocketTimeouts)
{
    int fds[2];
    ASSERT_EQ(socketpair(AF_UNIX, kSignalSocketType, 0, fds), 0);

    // Setting send timeout should succeed.
    EXPECT_EQ(setSocketTimeouts(fds[0], 100), 0);

    // Verify SO_SNDTIMEO was set by reading it back.
    timeval tv{};
    socklen_t len = sizeof(tv);
    ASSERT_EQ(getsockopt(fds[0], SOL_SOCKET, SO_SNDTIMEO, &tv, &len), 0);
    EXPECT_EQ(tv.tv_sec, 0);
    EXPECT_GE(tv.tv_usec, 90000); // ~100ms (allow kernel rounding)

    closeFd(fds[0]);
    closeFd(fds[1]);
}

#if defined(__APPLE__)
TEST(PlatformTest, ServerSocketPathCanBeReusedAfterClose)
{
    int srv = serverSocket(SOCK_NAME);
    ASSERT_GE(srv, 0);
    closeFd(srv);

    int rebound = serverSocket(SOCK_NAME);
    ASSERT_GE(rebound, 0);
    closeFd(rebound);
}

TEST(PlatformTest, ClosingAcceptedClientDoesNotUnlinkListener)
{
    int srv = serverSocket(SOCK_NAME);
    ASSERT_GE(srv, 0);

    int cli1 = clientSocket(SOCK_NAME);
    ASSERT_GE(cli1, 0);
    int accepted1 = acceptClient(srv);
    ASSERT_GE(accepted1, 0);

    closeFd(accepted1);
    closeFd(cli1);

    int cli2 = clientSocket(SOCK_NAME);
    ASSERT_GE(cli2, 0);
    int accepted2 = acceptClient(srv);
    ASSERT_GE(accepted2, 0);

    closeFd(accepted2);
    closeFd(cli2);
    closeFd(srv);
}
#endif

TEST(PlatformTest, PendingWakeupDrainsQueuedFrameBurst)
{
    int srv = serverSocket(SOCK_NAME);
    ASSERT_GE(srv, 0);

    Connection server;
    std::thread acceptThread([&] { server = acceptConnection(srv); });
    Connection client = connectToServer(SOCK_NAME);
    ASSERT_TRUE(client.valid());
    acceptThread.join();
    ASSERT_TRUE(server.valid());

    int bufSize = 1024;
    ASSERT_EQ(setsockopt(client.socketFd, SOL_SOCKET, SO_SNDBUF, &bufSize, sizeof(bufSize)), 0);
    ASSERT_EQ(setsockopt(server.socketFd, SOL_SOCKET, SO_RCVBUF, &bufSize, sizeof(bufSize)), 0);

    uint8_t byte = 1;
    int queuedSignals = 0;
    for (; queuedSignals < 65536; ++queuedSignals)
    {
        ssize_t n = send(client.socketFd, &byte, 1, MSG_NOSIGNAL | MSG_DONTWAIT);
        if (n == 1)
        {
            continue;
        }
        ASSERT_TRUE(errno == EAGAIN || errno == EWOULDBLOCK);
        break;
    }
    ASSERT_LT(queuedSignals, 65536) << "failed to reach a full wakeup queue";
    ASSERT_GT(queuedSignals, 0) << "failed to saturate the wakeup socket";

    constexpr uint32_t kBurstFrames = 2048;
    for (uint32_t seq = 1; seq <= kBurstFrames; ++seq)
    {
        FrameHeader hdr{};
        hdr.version = kProtocolVersion;
        hdr.flags = FRAME_REQUEST;
        hdr.serviceId = 1;
        hdr.messageId = 1;
        hdr.seq = seq;

        ASSERT_EQ(writeFrame(client.txRing, hdr, nullptr, 0), IPC_SUCCESS);
        ASSERT_EQ(sendSignal(client.socketFd), 0);
    }

    ASSERT_EQ(recvSignal(server.socketFd), 0);

    uint32_t received = 0;
    while (true)
    {
        FrameHeader hdr{};
        std::vector<uint8_t> payload;
        if (readFrameAlloc(server.rxRing, &hdr, &payload) != IPC_SUCCESS)
        {
            break;
        }

        ++received;
        EXPECT_EQ(hdr.version, kProtocolVersion);
        EXPECT_EQ(hdr.flags, FRAME_REQUEST);
        EXPECT_EQ(hdr.seq, received);
        EXPECT_TRUE(payload.empty());
    }

    EXPECT_EQ(received, kBurstFrames);

    client.close();
    server.close();
    closeFd(srv);
}
