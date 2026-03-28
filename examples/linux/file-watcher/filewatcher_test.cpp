// FileWatcher integration tests — exercises the inotify-backed server
// via the generated typed client over shared-memory IPC.

#include <gtest/gtest.h>
#include "FileWatcher.h"
#include "ClientBase.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <mutex>
#include <poll.h>
#include <string>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace aether::ipc;

#define SVC_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

static void settle()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

// ── Inline server implementation ────────────────────────────────────

class TestFileWatcher : public FileWatcher
{
public:
    using FileWatcher::FileWatcher;

    // Expose notify for testing.
    using FileWatcher::notifyFileChanged;

    ~TestFileWatcher() override
    {
        stopInotify();
    }

    bool startInotify()
    {
        m_inotifyFd = inotify_init1(IN_NONBLOCK);
        if (m_inotifyFd < 0)
        {
            return false;
        }
        m_running = true;
        m_inotifyThread = std::thread([this] { inotifyLoop(); });
        return true;
    }

    void stopInotify()
    {
        m_running = false;
        if (m_inotifyThread.joinable())
        {
            m_inotifyThread.join();
        }
        if (m_inotifyFd >= 0)
        {
            close(m_inotifyFd);
            m_inotifyFd = -1;
        }
    }

protected:
    int handleWatchDirectory(const char *path, uint32_t *watchId) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        int wd = inotify_add_watch(m_inotifyFd, path,
                                   IN_CREATE | IN_MODIFY | IN_DELETE);
        if (wd < 0)
        {
            return IPC_ERR_INVALID_ARGUMENT;
        }

        uint32_t id = m_nextWatchId++;
        m_wdToWatchId[wd] = id;
        m_watchIdToWd[id] = wd;

        *watchId = id;
        return IPC_SUCCESS;
    }

    int handleUnwatchDirectory(uint32_t watchId) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_watchIdToWd.find(watchId);
        if (it == m_watchIdToWd.end())
        {
            return IPC_ERR_INVALID_ARGUMENT;
        }

        int wd = it->second;
        inotify_rm_watch(m_inotifyFd, wd);
        m_wdToWatchId.erase(wd);
        m_watchIdToWd.erase(it);
        return IPC_SUCCESS;
    }

    int handleGetWatchCount(uint32_t *count) override
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        *count = static_cast<uint32_t>(m_watchIdToWd.size());
        return IPC_SUCCESS;
    }

private:
    void inotifyLoop()
    {
        constexpr size_t kBufSize = 4096;
        alignas(struct inotify_event) char buf[kBufSize];

        while (m_running)
        {
            struct pollfd pfd{};
            pfd.fd = m_inotifyFd;
            pfd.events = POLLIN;

            int ret = poll(&pfd, 1, 50);
            if (ret <= 0)
            {
                continue;
            }

            ssize_t len = read(m_inotifyFd, buf, sizeof(buf));
            if (len <= 0)
            {
                continue;
            }

            for (const char *ptr = buf; ptr < buf + len;)
            {
                const auto *event =
                    reinterpret_cast<const struct inotify_event *>(ptr);
                handleInotifyEvent(event);
                ptr += sizeof(struct inotify_event) + event->len;
            }
        }
    }

    void handleInotifyEvent(const struct inotify_event *event)
    {
        if (event->len == 0)
        {
            return;
        }

        FileEventType eventType;
        if (event->mask & IN_CREATE)
        {
            eventType = Created;
        }
        else if (event->mask & IN_MODIFY)
        {
            eventType = Modified;
        }
        else if (event->mask & IN_DELETE)
        {
            eventType = Deleted;
        }
        else
        {
            return;
        }

        uint32_t watchId = 0;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_wdToWatchId.find(event->wd);
            if (it == m_wdToWatchId.end())
            {
                return;
            }
            watchId = it->second;
        }

        notifyFileChanged(watchId, eventType, event->name);
    }

    std::mutex m_mutex;
    int m_inotifyFd = -1;
    std::atomic<bool> m_running{false};
    std::thread m_inotifyThread;
    uint32_t m_nextWatchId = 1;
    std::map<int, uint32_t> m_wdToWatchId;
    std::map<uint32_t, int> m_watchIdToWd;
};

// ── Notification-capturing typed client ─────────────────────────────

namespace client
{
#include "ClientBase.h"
}

// We include the client header indirectly via the gen code, but the
// generated server and client share the same class name.  For tests we
// use the raw-notification approach from the echo example, which works
// with the server-side build (links gen/server/FileWatcher.cpp).

class NotifyClient : public ClientBase
{
public:
    using ClientBase::ClientBase;

    struct Event
    {
        uint32_t watchId;
        FileEventType eventType;
        std::string filename;
    };

    std::mutex mtx;
    std::condition_variable cv;
    std::vector<Event> events;

    bool waitForEvents(int count, int timeoutMs = 2000)
    {
        std::unique_lock<std::mutex> lock(mtx);
        return cv.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                           [&] { return static_cast<int>(events.size()) >= count; });
    }

protected:
    void onNotification(uint32_t serviceId, uint32_t messageId,
                        const std::vector<uint8_t> &payload) override
    {
        if (serviceId != FileWatcher::kServiceId)
        {
            return;
        }
        if (messageId != FileWatcher::kFileChanged)
        {
            return;
        }
        if (payload.size() < sizeof(uint32_t) + sizeof(FileEventType) + 257)
        {
            return;
        }

        Event e;
        std::memcpy(&e.watchId, payload.data(), sizeof(e.watchId));
        std::memcpy(&e.eventType, payload.data() + sizeof(uint32_t),
                     sizeof(e.eventType));

        char fname[257];
        std::memcpy(fname, payload.data() + sizeof(uint32_t) + sizeof(FileEventType), 257);
        fname[256] = '\0';
        e.filename = fname;

        {
            std::lock_guard<std::mutex> lock(mtx);
            events.push_back(std::move(e));
            cv.notify_all();
        }
    }
};

// ── Test fixture ────────────────────────────────────────────────────

class FileWatcherTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create a unique temp directory for this test.
        char tmpl[] = "filewatcher_test_XXXXXX";
        char *dir = mkdtemp(tmpl);
        ASSERT_NE(dir, nullptr);
        m_tmpDir = dir;
    }

    void TearDown() override
    {
        // Remove temp directory and contents.
        if (!m_tmpDir.empty())
        {
            std::string cmd = "rm -rf " + m_tmpDir;
            (void)system(cmd.c_str());
        }
    }

    // Create an additional temp directory (for multi-dir tests).
    std::string createExtraTmpDir()
    {
        char tmpl[] = "filewatcher_extra_XXXXXX";
        char *dir = mkdtemp(tmpl);
        if (dir)
        {
            m_extraDirs.push_back(dir);
        }
        return dir ? dir : "";
    }

    void createFile(const std::string &dir, const std::string &name)
    {
        std::string path = dir + "/" + name;
        int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd >= 0)
        {
            close(fd);
        }
    }

    void modifyFile(const std::string &dir, const std::string &name)
    {
        std::string path = dir + "/" + name;
        int fd = open(path.c_str(), O_WRONLY | O_APPEND);
        if (fd >= 0)
        {
            (void)write(fd, "x", 1);
            close(fd);
        }
    }

    void deleteFile(const std::string &dir, const std::string &name)
    {
        std::string path = dir + "/" + name;
        unlink(path.c_str());
    }

    std::string m_tmpDir;
    std::vector<std::string> m_extraDirs;

    ~FileWatcherTest() override
    {
        for (auto &d : m_extraDirs)
        {
            std::string cmd = "rm -rf " + d;
            (void)system(cmd.c_str());
        }
    }
};

// ═════════════════════════════════════════════════════════════════════
// WatchDirectory — returns a valid watchId
// ═════════════════════════════════════════════════════════════════════

TEST_F(FileWatcherTest, WatchDirectory_ReturnsValidId)
{
    TestFileWatcher svc(SVC_NAME);
    ASSERT_TRUE(svc.startInotify());
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Marshal path → call WatchDirectory.
    std::vector<uint8_t> request(257, 0);
    std::strncpy(reinterpret_cast<char *>(request.data()), m_tmpDir.c_str(), 256);

    std::vector<uint8_t> response;
    int rc = client.call(FileWatcher::kServiceId, FileWatcher::kWatchDirectory,
                         request, &response);
    ASSERT_EQ(rc, IPC_SUCCESS);
    ASSERT_EQ(response.size(), sizeof(uint32_t));

    uint32_t watchId;
    std::memcpy(&watchId, response.data(), sizeof(watchId));
    EXPECT_GT(watchId, 0u);

    client.disconnect();
    svc.stop();
    svc.stopInotify();
}

// ═════════════════════════════════════════════════════════════════════
// UnwatchDirectory — succeeds after watching
// ═════════════════════════════════════════════════════════════════════

TEST_F(FileWatcherTest, UnwatchDirectory_Succeeds)
{
    TestFileWatcher svc(SVC_NAME);
    ASSERT_TRUE(svc.startInotify());
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Watch.
    std::vector<uint8_t> request(257, 0);
    std::strncpy(reinterpret_cast<char *>(request.data()), m_tmpDir.c_str(), 256);
    std::vector<uint8_t> response;
    int rc = client.call(FileWatcher::kServiceId, FileWatcher::kWatchDirectory,
                         request, &response);
    ASSERT_EQ(rc, IPC_SUCCESS);
    uint32_t watchId;
    std::memcpy(&watchId, response.data(), sizeof(watchId));

    // Unwatch.
    std::vector<uint8_t> unwatchReq(sizeof(watchId));
    std::memcpy(unwatchReq.data(), &watchId, sizeof(watchId));
    std::vector<uint8_t> unwatchResp;
    rc = client.call(FileWatcher::kServiceId, FileWatcher::kUnwatchDirectory,
                     unwatchReq, &unwatchResp);
    EXPECT_EQ(rc, IPC_SUCCESS);

    client.disconnect();
    svc.stop();
    svc.stopInotify();
}

// ═════════════════════════════════════════════════════════════════════
// GetWatchCount — tracks correctly
// ═════════════════════════════════════════════════════════════════════

TEST_F(FileWatcherTest, GetWatchCount_TracksCorrectly)
{
    std::string dir2 = createExtraTmpDir();
    ASSERT_FALSE(dir2.empty());

    TestFileWatcher svc(SVC_NAME);
    ASSERT_TRUE(svc.startInotify());
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    auto watchDir = [&](const std::string &dir, uint32_t *outId) -> int
    {
        std::vector<uint8_t> req(257, 0);
        std::strncpy(reinterpret_cast<char *>(req.data()), dir.c_str(), 256);
        std::vector<uint8_t> resp;
        int rc = client.call(FileWatcher::kServiceId, FileWatcher::kWatchDirectory,
                             req, &resp);
        if (rc == IPC_SUCCESS && resp.size() >= sizeof(uint32_t))
        {
            std::memcpy(outId, resp.data(), sizeof(*outId));
        }
        return rc;
    };

    auto getCount = [&]() -> uint32_t
    {
        std::vector<uint8_t> req;
        std::vector<uint8_t> resp;
        int rc = client.call(FileWatcher::kServiceId, FileWatcher::kGetWatchCount,
                             req, &resp);
        if (rc != IPC_SUCCESS || resp.size() < sizeof(uint32_t))
        {
            return 0xFFFFFFFF;
        }
        uint32_t count;
        std::memcpy(&count, resp.data(), sizeof(count));
        return count;
    };

    // Watch two directories.
    uint32_t id1 = 0, id2 = 0;
    ASSERT_EQ(watchDir(m_tmpDir, &id1), IPC_SUCCESS);
    ASSERT_EQ(watchDir(dir2, &id2), IPC_SUCCESS);
    EXPECT_EQ(getCount(), 2u);

    // Unwatch one.
    std::vector<uint8_t> unwatchReq(sizeof(id1));
    std::memcpy(unwatchReq.data(), &id1, sizeof(id1));
    std::vector<uint8_t> unwatchResp;
    ASSERT_EQ(client.call(FileWatcher::kServiceId, FileWatcher::kUnwatchDirectory,
                          unwatchReq, &unwatchResp),
              IPC_SUCCESS);
    EXPECT_EQ(getCount(), 1u);

    client.disconnect();
    svc.stop();
    svc.stopInotify();
}

// ═════════════════════════════════════════════════════════════════════
// FileCreated — notification fires
// ═════════════════════════════════════════════════════════════════════

TEST_F(FileWatcherTest, FileCreated_NotificationFires)
{
    TestFileWatcher svc(SVC_NAME);
    ASSERT_TRUE(svc.startInotify());
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    // Watch directory via raw call.
    std::vector<uint8_t> request(257, 0);
    std::strncpy(reinterpret_cast<char *>(request.data()), m_tmpDir.c_str(), 256);
    std::vector<uint8_t> response;
    int rc = client.call(FileWatcher::kServiceId, FileWatcher::kWatchDirectory,
                         request, &response);
    ASSERT_EQ(rc, IPC_SUCCESS);
    uint32_t watchId;
    std::memcpy(&watchId, response.data(), sizeof(watchId));

    // Create a file in the watched directory.
    settle();
    createFile(m_tmpDir, "hello.txt");

    // Wait for notification.
    ASSERT_TRUE(client.waitForEvents(1));

    {
        std::lock_guard<std::mutex> lock(client.mtx);
        ASSERT_FALSE(client.events.empty());
        auto &ev = client.events[0];
        EXPECT_EQ(ev.watchId, watchId);
        EXPECT_EQ(ev.eventType, Created);
        EXPECT_EQ(ev.filename, "hello.txt");
    }

    client.disconnect();
    svc.stop();
    svc.stopInotify();
}

// ═════════════════════════════════════════════════════════════════════
// FileModified — notification fires
// ═════════════════════════════════════════════════════════════════════

TEST_F(FileWatcherTest, FileModified_NotificationFires)
{
    // Pre-create the file before watching.
    createFile(m_tmpDir, "data.bin");

    TestFileWatcher svc(SVC_NAME);
    ASSERT_TRUE(svc.startInotify());
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request(257, 0);
    std::strncpy(reinterpret_cast<char *>(request.data()), m_tmpDir.c_str(), 256);
    std::vector<uint8_t> response;
    ASSERT_EQ(client.call(FileWatcher::kServiceId, FileWatcher::kWatchDirectory,
                          request, &response),
              IPC_SUCCESS);
    uint32_t watchId;
    std::memcpy(&watchId, response.data(), sizeof(watchId));

    settle();
    modifyFile(m_tmpDir, "data.bin");

    ASSERT_TRUE(client.waitForEvents(1));

    {
        std::lock_guard<std::mutex> lock(client.mtx);
        ASSERT_FALSE(client.events.empty());
        auto &ev = client.events[0];
        EXPECT_EQ(ev.watchId, watchId);
        EXPECT_EQ(ev.eventType, Modified);
        EXPECT_EQ(ev.filename, "data.bin");
    }

    client.disconnect();
    svc.stop();
    svc.stopInotify();
}

// ═════════════════════════════════════════════════════════════════════
// FileDeleted — notification fires
// ═════════════════════════════════════════════════════════════════════

TEST_F(FileWatcherTest, FileDeleted_NotificationFires)
{
    createFile(m_tmpDir, "removeme.txt");

    TestFileWatcher svc(SVC_NAME);
    ASSERT_TRUE(svc.startInotify());
    ASSERT_TRUE(svc.start());
    settle();

    NotifyClient client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    std::vector<uint8_t> request(257, 0);
    std::strncpy(reinterpret_cast<char *>(request.data()), m_tmpDir.c_str(), 256);
    std::vector<uint8_t> response;
    ASSERT_EQ(client.call(FileWatcher::kServiceId, FileWatcher::kWatchDirectory,
                          request, &response),
              IPC_SUCCESS);
    uint32_t watchId;
    std::memcpy(&watchId, response.data(), sizeof(watchId));

    settle();
    deleteFile(m_tmpDir, "removeme.txt");

    ASSERT_TRUE(client.waitForEvents(1));

    {
        std::lock_guard<std::mutex> lock(client.mtx);
        ASSERT_FALSE(client.events.empty());
        auto &ev = client.events[0];
        EXPECT_EQ(ev.watchId, watchId);
        EXPECT_EQ(ev.eventType, Deleted);
        EXPECT_EQ(ev.filename, "removeme.txt");
    }

    client.disconnect();
    svc.stop();
    svc.stopInotify();
}

// ═════════════════════════════════════════════════════════════════════
// InvalidWatchId — unwatch returns error
// ═════════════════════════════════════════════════════════════════════

TEST_F(FileWatcherTest, InvalidWatchId_ReturnsError)
{
    TestFileWatcher svc(SVC_NAME);
    ASSERT_TRUE(svc.startInotify());
    ASSERT_TRUE(svc.start());
    settle();

    ClientBase client(SVC_NAME);
    ASSERT_TRUE(client.connect());
    settle();

    uint32_t bogusId = 99999;
    std::vector<uint8_t> request(sizeof(bogusId));
    std::memcpy(request.data(), &bogusId, sizeof(bogusId));
    std::vector<uint8_t> response;
    int rc = client.call(FileWatcher::kServiceId, FileWatcher::kUnwatchDirectory,
                         request, &response);
    EXPECT_EQ(rc, IPC_ERR_INVALID_ARGUMENT);

    client.disconnect();
    svc.stop();
    svc.stopInotify();
}
