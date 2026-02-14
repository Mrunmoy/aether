#include <gtest/gtest.h>
#include "Platform.h"

#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

using namespace ms::ipc::platform;

// Unique socket name per test to avoid collisions when tests run in parallel.
// Uses the test name provided by gtest.
#define SOCK_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

// ═════════════════════════════════════════════════════════════════════
// UDS Tests
// ═════════════════════════════════════════════════════════════════════

TEST(PlatformTest, ServerSocketCreation) {
    int fd = serverSocket(SOCK_NAME);
    ASSERT_GE(fd, 0);
    closeFd(fd);
}

TEST(PlatformTest, ClientConnects) {
    int srv = serverSocket(SOCK_NAME);
    ASSERT_GE(srv, 0);

    int cli = clientSocket(SOCK_NAME);
    ASSERT_GE(cli, 0);

    closeFd(cli);
    closeFd(srv);
}

TEST(PlatformTest, AcceptClient) {
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

TEST(PlatformTest, SendRecvSignal) {
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

TEST(PlatformTest, SendRecvFd) {
    int srv = serverSocket(SOCK_NAME);
    ASSERT_GE(srv, 0);

    int cli = clientSocket(SOCK_NAME);
    ASSERT_GE(cli, 0);

    int accepted = acceptClient(srv);
    ASSERT_GE(accepted, 0);

    // Create a shared memory region and write a magic value.
    int shmFd = shmCreate(4096);
    ASSERT_GE(shmFd, 0);

    void* writerMap = mmap(nullptr, 4096, PROT_READ | PROT_WRITE,
                           MAP_SHARED, shmFd, 0);
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
    void* readerMap = mmap(nullptr, 4096, PROT_READ, MAP_SHARED, receivedFd, 0);
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

TEST(PlatformTest, SendRecvFdNoAncillaryData) {
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

TEST(PlatformTest, ConnectToNonexistent) {
    int fd = clientSocket("no_such_service_exists");
    EXPECT_EQ(fd, -1);
}

// ═════════════════════════════════════════════════════════════════════
// Shared Memory Tests
// ═════════════════════════════════════════════════════════════════════

TEST(PlatformTest, ShmCreate) {
    int fd = shmCreate(4096);
    ASSERT_GE(fd, 0);
    closeFd(fd);
}

TEST(PlatformTest, ShmWriteAndMmap) {
    int fd = shmCreate(4096);
    ASSERT_GE(fd, 0);

    void* ptr = mmap(nullptr, 4096, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);
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

TEST(PlatformTest, ShmZeroSize) {
    // memfd_create with ftruncate(0) should succeed — valid but empty region.
    int fd = shmCreate(0);
    ASSERT_GE(fd, 0);
    closeFd(fd);
}

// ═════════════════════════════════════════════════════════════════════
// Close Tests
// ═════════════════════════════════════════════════════════════════════

TEST(PlatformTest, CloseFdNegativeOne) {
    // Should not crash or fail.
    closeFd(-1);
}
