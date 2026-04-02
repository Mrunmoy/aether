// FileWatcher device (server) — watches directories using Linux inotify.

#include "FileWatcher.h"

#include <atomic>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <map>
#include <mutex>
#include <poll.h>
#include <sys/inotify.h>
#include <thread>
#include <unistd.h>

using namespace aether::ipc;

static std::atomic<bool> g_running{true};

static void sigintHandler(int)
{
    g_running = false;
}

class FileWatcherDevice : public FileWatcher
{
public:
    using FileWatcher::FileWatcher;

    ~FileWatcherDevice() override
    {
        stopInotify();
    }

    bool startInotify()
    {
        m_inotifyFd = inotify_init1(IN_NONBLOCK);
        if (m_inotifyFd < 0)
        {
            std::perror("inotify_init1");
            return false;
        }
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
            std::perror("inotify_add_watch");
            return IPC_ERR_INVALID_ARGUMENT;
        }

        uint32_t id = m_nextWatchId++;
        m_wdToWatchId[wd] = id;
        m_watchIdToWd[id] = wd;
        m_watchIdToPath[id] = path;

        *watchId = id;
        std::printf("[device] WatchDirectory(\"%s\") → watchId=%u\n", path, id);
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
        m_watchIdToPath.erase(watchId);
        m_watchIdToWd.erase(it);

        std::printf("[device] UnwatchDirectory(watchId=%u)\n", watchId);
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

            int ret = poll(&pfd, 1, 100); // 100ms timeout
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
                const auto *event = reinterpret_cast<const struct inotify_event *>(ptr);
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

        std::printf("[device] event: watchId=%u type=%u file=%s\n",
                    watchId, static_cast<unsigned>(eventType), event->name);
        notifyFileChanged(watchId, eventType, event->name);
    }

    std::mutex m_mutex;
    int m_inotifyFd = -1;
    std::atomic<bool> m_running{true};
    std::thread m_inotifyThread;
    uint32_t m_nextWatchId = 1;

    std::map<int, uint32_t> m_wdToWatchId;      // inotify wd → watchId
    std::map<uint32_t, int> m_watchIdToWd;       // watchId → inotify wd
    std::map<uint32_t, std::string> m_watchIdToPath;
};

int main()
{
    std::signal(SIGINT, sigintHandler);

    FileWatcherDevice svc("FileWatcher");
    if (!svc.startInotify())
    {
        std::fprintf(stderr, "Failed to init inotify\n");
        return 1;
    }
    if (!svc.start())
    {
        std::fprintf(stderr, "Failed to start service\n");
        return 1;
    }

    std::printf("FileWatcher device running. Press Ctrl-C to stop.\n");
    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::printf("\nShutting down...\n");
    svc.stop();
    svc.stopInotify();
    return 0;
}
