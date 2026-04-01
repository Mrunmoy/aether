// FileWatcher client — watches directories and prints events.

#include "FileWatcher.h"

#include <atomic>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

using namespace aether::ipc;

static std::atomic<bool> g_running{true};

static void sigintHandler(int)
{
    g_running = false;
}

static const char *eventTypeName(FileEventType type)
{
    switch (type)
    {
    case Created:
        return "Created";
    case Modified:
        return "Modified";
    case Deleted:
        return "Deleted";
    default:
        return "Unknown";
    }
}

class FileWatcherClient : public FileWatcher
{
public:
    using FileWatcher::FileWatcher;

protected:
    void onFileChanged(uint32_t watchId, FileEventType eventType,
                       const char *filename) override
    {
        std::printf("[%s] %s (watchId=%u)\n",
                    eventTypeName(eventType), filename, watchId);
    }
};

int main(int argc, char *argv[])
{
    std::signal(SIGINT, sigintHandler);

    FileWatcherClient client("FileWatcher");
    if (!client.connect())
    {
        std::fprintf(stderr, "Failed to connect to FileWatcher service\n");
        return 1;
    }

    // Default to current directory if no args given.
    std::vector<const char *> dirs;
    if (argc < 2)
    {
        dirs.push_back(".");
    }
    else
    {
        for (int i = 1; i < argc; ++i)
        {
            dirs.push_back(argv[i]);
        }
    }

    // Watch each directory.
    std::vector<uint32_t> watchIds;
    for (const char *dir : dirs)
    {
        uint32_t watchId = 0;
        int rc = client.WatchDirectory(dir, &watchId);
        if (rc != IPC_SUCCESS)
        {
            std::fprintf(stderr, "Failed to watch \"%s\" (error %d)\n", dir, rc);
            continue;
        }
        std::printf("Watching \"%s\" (watchId=%u)\n", dir, watchId);
        watchIds.push_back(watchId);
    }

    if (watchIds.empty())
    {
        std::fprintf(stderr, "No directories being watched\n");
        client.disconnect();
        return 1;
    }

    std::printf("Listening for file events. Press Ctrl-C to stop.\n");
    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // Unwatch before disconnecting.
    for (uint32_t id : watchIds)
    {
        client.UnwatchDirectory(id);
    }

    std::printf("\nDisconnected.\n");
    client.disconnect();
    return 0;
}
