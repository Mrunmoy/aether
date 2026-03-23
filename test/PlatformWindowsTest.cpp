#include <gtest/gtest.h>

#include "Platform.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <chrono>
#include <cstring>
#include <thread>
#include <vector>

using namespace aether::ipc::platform;

#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

TEST(PlatformWindowsTest, ServerClientSignalRoundTrip)
{
    Handle listen = serverSocket(SVC_NAME);
    ASSERT_NE(listen, kInvalidHandle);

    Handle accepted = kInvalidHandle;
    std::thread acceptThread([&] { accepted = acceptClient(listen); });

    Handle client = clientSocket(SVC_NAME);
    ASSERT_NE(client, kInvalidHandle);
    acceptThread.join();
    ASSERT_NE(accepted, kInvalidHandle);

    EXPECT_EQ(sendSignal(client), 0);
    EXPECT_EQ(recvSignal(accepted), 0);

    closeFd(client);
    closeFd(accepted);
    closeFd(listen);
}

TEST(PlatformWindowsTest, SharedMemoryMappingRoundTrip)
{
    const char *mappingName = "Local\\aether_test_mapping";
    Handle shm = shmCreate(4096, mappingName);
    ASSERT_NE(shm, kInvalidHandle);

    void *writer = mapSharedMemory(shm, 4096);
    ASSERT_NE(writer, nullptr);

    uint32_t magic = 0xDEADBEEF;
    std::memcpy(writer, &magic, sizeof(magic));

    HANDLE reopened = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, mappingName);
    ASSERT_NE(reopened, nullptr);

    void *reader = MapViewOfFile(reopened, FILE_MAP_ALL_ACCESS, 0, 0, 4096);
    ASSERT_NE(reader, nullptr);

    uint32_t readMagic = 0;
    std::memcpy(&readMagic, reader, sizeof(readMagic));
    EXPECT_EQ(readMagic, magic);

    unmapSharedMemory(reader, 4096);
    CloseHandle(reopened);
    unmapSharedMemory(writer, 4096);
    closeFd(shm);
}

TEST(PlatformWindowsTest, ShutdownUnblocksAcceptAndRecv)
{
    Handle listen = serverSocket(SVC_NAME);
    ASSERT_NE(listen, kInvalidHandle);

    Handle accepted = kInvalidHandle;
    std::thread acceptThread([&] { accepted = acceptClient(listen); });

    ASSERT_EQ(shutdownConnection(listen), 0);
    acceptThread.join();
    EXPECT_EQ(accepted, kInvalidHandle);
    closeFd(listen);

    listen = serverSocket(SVC_NAME);
    ASSERT_NE(listen, kInvalidHandle);

    std::thread acceptThread2([&] { accepted = acceptClient(listen); });
    Handle client = clientSocket(SVC_NAME);
    ASSERT_NE(client, kInvalidHandle);
    acceptThread2.join();
    ASSERT_NE(accepted, kInvalidHandle);

    std::thread recvThread([&] { EXPECT_EQ(recvSignal(accepted), -1); });
    ASSERT_EQ(shutdownConnection(accepted), 0);
    recvThread.join();

    closeFd(client);
    closeFd(accepted);
    closeFd(listen);
}
