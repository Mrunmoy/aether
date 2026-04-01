#include <gtest/gtest.h>
#include "DiskUsage.h"
#include "ClientBase.h"
#include "RunLoop.h"

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

using namespace aether::ipc;

#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

static void settle()
{
#if defined(_WIN32)
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
#endif
}

// ── Test server with simulated volumes ──────────────────────────────

class TestDiskUsage : public DiskUsage
{
public:
    using DiskUsage::DiskUsage;

    // Expose for notification tests.
    using DiskUsage::notifyDiskSpaceLow;

protected:
    int handleGetDiskUsage(const char *mountPoint, VolumeInfo *info) override
    {
        for (const auto &v : m_volumes)
        {
            if (std::strcmp(v.mountPoint, mountPoint) == 0)
            {
                *info = v;
                return IPC_SUCCESS;
            }
        }
        return 1; // not found
    }

    int handleGetVolumeCount(uint32_t *count) override
    {
        *count = static_cast<uint32_t>(m_volumes.size());
        return IPC_SUCCESS;
    }

    int handleGetVolumeAt(uint32_t index, VolumeInfo *info) override
    {
        if (index >= m_volumes.size())
            return 1; // out of bounds
        *info = m_volumes[index];
        return IPC_SUCCESS;
    }

private:
    static constexpr uint64_t kGB = 1024ULL * 1024ULL * 1024ULL;

    static VolumeInfo makeVolume(const char *mount, const char *fs,
                                 uint64_t total, uint64_t used)
    {
        VolumeInfo v{};
        std::strncpy(v.mountPoint, mount, 256);
        std::strncpy(v.filesystem, fs, 64);
        v.totalBytes = total;
        v.usedBytes = used;
        v.freeBytes = total - used;
        v.usedPercent = 100.0f * static_cast<float>(used)
                        / static_cast<float>(total);
        return v;
    }

    std::vector<VolumeInfo> m_volumes = {
        makeVolume("/",     "ext4",  500ULL * kGB, 350ULL * kGB),
        makeVolume("/home", "ext4", 1000ULL * kGB, 800ULL * kGB),
        makeVolume("/tmp",  "tmpfs",  50ULL * kGB,   5ULL * kGB),
    };
};

// ── Notification-receiving raw client ───────────────────────────────

class NotifyClient : public ClientBase
{
public:
    using ClientBase::ClientBase;

    std::mutex mtx;
    std::condition_variable cv;
    uint32_t lastServiceId = 0;
    uint32_t lastMessageId = 0;
    std::vector<uint8_t> lastPayload;
    int notifyCount = 0;

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        std::lock_guard<std::mutex> lock(mtx);
        lastServiceId = serviceId;
        lastMessageId = messageId;
        lastPayload = payload;
        notifyCount++;
        cv.notify_all();
    }
};

// ── RunLoop RAII guard ──────────────────────────────────────────────

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
// GetVolumeCount — count >= 1
// ═════════════════════════════════════════════════════════════════════

TEST(DiskUsageTest, GetVolumeCount_ReturnsPositive)
{
    TestDiskUsage svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(DiskUsage::kServiceId, DiskUsage::kGetVolumeCount,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint32_t));

    uint32_t count;
    std::memcpy(&count, response.data(), sizeof(count));
    EXPECT_GE(count, 1u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetVolumeAt — valid index returns data with totalBytes > 0
// ═════════════════════════════════════════════════════════════════════

TEST(DiskUsageTest, GetVolumeAt_ValidIndex)
{
    TestDiskUsage svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t index = 0;
    std::vector<uint8_t> request(sizeof(index));
    std::memcpy(request.data(), &index, sizeof(index));

    std::vector<uint8_t> response;
    int rc = client.call(DiskUsage::kServiceId, DiskUsage::kGetVolumeAt,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(VolumeInfo));

    VolumeInfo info;
    std::memcpy(&info, response.data(), sizeof(info));
    EXPECT_GT(info.totalBytes, 0u);
    EXPECT_GT(info.freeBytes, 0u);
    EXPECT_GT(std::strlen(info.mountPoint), 0u);
    EXPECT_GT(std::strlen(info.filesystem), 0u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetVolumeAt — invalid index returns error
// ═════════════════════════════════════════════════════════════════════

TEST(DiskUsageTest, GetVolumeAt_InvalidIndex)
{
    TestDiskUsage svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t index = 999;
    std::vector<uint8_t> request(sizeof(index));
    std::memcpy(request.data(), &index, sizeof(index));

    std::vector<uint8_t> response;
    int rc = client.call(DiskUsage::kServiceId, DiskUsage::kGetVolumeAt,
                         request, &response);

    EXPECT_GT(rc, 0); // positive = application error

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetDiskUsage — known mount "/" returns valid data
// ═════════════════════════════════════════════════════════════════════

TEST(DiskUsageTest, GetDiskUsage_KnownMount)
{
    TestDiskUsage svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Marshal mountPoint as char[257]
    char mountPoint[257] = {};
    std::strncpy(mountPoint, "/", 256);
    std::vector<uint8_t> request(257);
    std::memcpy(request.data(), mountPoint, 257);

    std::vector<uint8_t> response;
    int rc = client.call(DiskUsage::kServiceId, DiskUsage::kGetDiskUsage,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(VolumeInfo));

    VolumeInfo info;
    std::memcpy(&info, response.data(), sizeof(info));
    EXPECT_STREQ(info.mountPoint, "/");
    EXPECT_GT(info.totalBytes, 0u);
    EXPECT_GT(info.usedBytes, 0u);
    EXPECT_GT(info.freeBytes, 0u);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// GetDiskUsage — unknown mount returns error
// ═════════════════════════════════════════════════════════════════════

TEST(DiskUsageTest, GetDiskUsage_UnknownMount)
{
    TestDiskUsage svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    char mountPoint[257] = {};
    std::strncpy(mountPoint, "/nonexistent", 256);
    std::vector<uint8_t> request(257);
    std::memcpy(request.data(), mountPoint, 257);

    std::vector<uint8_t> response;
    int rc = client.call(DiskUsage::kServiceId, DiskUsage::kGetDiskUsage,
                         request, &response);

    EXPECT_GT(rc, 0); // positive = application error

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// DiskSpaceLow notification fires and carries correct payload
// ═════════════════════════════════════════════════════════════════════

TEST(DiskUsageTest, DiskSpaceLow_NotificationFires)
{
    TestDiskUsage svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    float usedPercent = 92.5f;
    uint64_t freeBytes = 37ULL * 1024ULL * 1024ULL * 1024ULL;
    ASSERT_EQ(svc.notifyDiskSpaceLow("/", usedPercent, freeBytes), IPC_SUCCESS);

    {
        std::unique_lock<std::mutex> lock(client.mtx);
        ASSERT_TRUE(client.cv.wait_for(lock, std::chrono::milliseconds(2000),
                                        [&] { return client.notifyCount > 0; }));
    }

    EXPECT_EQ(client.lastServiceId, DiskUsage::kServiceId);
    EXPECT_EQ(client.lastMessageId, DiskUsage::kDiskSpaceLow);

    // Payload layout: char[257] + float + uint64_t
    ASSERT_GE(client.lastPayload.size(), 257 + sizeof(float) + sizeof(uint64_t));

    char recvMount[257];
    std::memcpy(recvMount, client.lastPayload.data(), 257);
    recvMount[256] = '\0';
    EXPECT_STREQ(recvMount, "/");

    float recvPercent;
    std::memcpy(&recvPercent, client.lastPayload.data() + 257, sizeof(recvPercent));
    EXPECT_FLOAT_EQ(recvPercent, 92.5f);

    uint64_t recvFree;
    std::memcpy(&recvFree, client.lastPayload.data() + 257 + sizeof(float),
                sizeof(recvFree));
    EXPECT_EQ(recvFree, freeBytes);

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// UsedPercent is consistent with usedBytes / totalBytes * 100
// ═════════════════════════════════════════════════════════════════════

TEST(DiskUsageTest, UsedPercent_Consistent)
{
    TestDiskUsage svc(SVC_NAME);
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Get volume count
    std::vector<uint8_t> countReq;
    std::vector<uint8_t> countResp;
    ASSERT_EQ(client.call(DiskUsage::kServiceId, DiskUsage::kGetVolumeCount,
                          countReq, &countResp), IPC_SUCCESS);

    uint32_t count;
    std::memcpy(&count, countResp.data(), sizeof(count));

    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t index = i;
        std::vector<uint8_t> request(sizeof(index));
        std::memcpy(request.data(), &index, sizeof(index));

        std::vector<uint8_t> response;
        ASSERT_EQ(client.call(DiskUsage::kServiceId, DiskUsage::kGetVolumeAt,
                              request, &response), IPC_SUCCESS);

        VolumeInfo info;
        std::memcpy(&info, response.data(), sizeof(info));

        float expected = 100.0f * static_cast<float>(info.usedBytes)
                         / static_cast<float>(info.totalBytes);

        EXPECT_NEAR(info.usedPercent, expected, 0.1f)
            << "Volume " << info.mountPoint << " usedPercent mismatch";

        EXPECT_EQ(info.usedBytes + info.freeBytes, info.totalBytes)
            << "Volume " << info.mountPoint << " bytes don't add up";
    }

    client.disconnect();
    svc.stop();
}

// ═════════════════════════════════════════════════════════════════════
// RunLoop mode — dispatch works
// ═════════════════════════════════════════════════════════════════════

TEST(DiskUsageTest, RunLoop_Dispatch)
{
    ms::RunLoop loop;
    loop.init("DskRL");

    TestDiskUsage svc(SVC_NAME, &loop);
    ASSERT_TRUE(svc.start());

    RunLoopGuard guard(loop);
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request;
    std::vector<uint8_t> response;
    int rc = client.call(DiskUsage::kServiceId, DiskUsage::kGetVolumeCount,
                         request, &response);

    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint32_t));

    uint32_t count;
    std::memcpy(&count, response.data(), sizeof(count));
    EXPECT_EQ(count, 3u);

    client.disconnect();
    svc.stop();
}
