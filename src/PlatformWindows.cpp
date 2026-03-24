#include "Platform.h"

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <atomic>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>

namespace aether::ipc::platform
{

    namespace
    {
        struct ListenerEntry
        {
            uint64_t serviceHash = 0;
            HANDLE stopEvent = nullptr;
        };

        std::mutex g_listenerMutex;
        std::unordered_map<Handle, ListenerEntry> g_listeners;
        std::atomic<uintptr_t> g_nextListenerToken{1};
        std::atomic<uint32_t> g_nextSharedMemoryId{1};

        uint64_t fnv1a64(const char *name)
        {
            constexpr uint64_t kOffset = 14695981039346656037ULL;
            constexpr uint64_t kPrime = 1099511628211ULL;
            uint64_t hash = kOffset;
            for (const unsigned char *p = reinterpret_cast<const unsigned char *>(name); *p != 0;
                 ++p)
            {
                hash ^= *p;
                hash *= kPrime;
            }
            return hash;
        }

        std::string hex64(uint64_t value)
        {
            char buf[17];
            std::snprintf(buf, sizeof(buf), "%016llX",
                          static_cast<unsigned long long>(value));
            return std::string(buf);
        }

        std::string pipeName(uint64_t hash)
        {
            return "\\\\.\\pipe\\aether_" + hex64(hash);
        }

        std::string listenerStopName(uint64_t hash)
        {
            return "Local\\aether_stop_" + hex64(hash);
        }

        std::string sharedMemoryName(const char *serviceName)
        {
            const uint64_t hash = fnv1a64(serviceName);
            char buf[128];
            std::snprintf(buf, sizeof(buf), "Local\\aether_shm_%s_%08X_%08X",
                          hex64(hash).c_str(), static_cast<unsigned>(GetCurrentProcessId()),
                          static_cast<unsigned>(g_nextSharedMemoryId.fetch_add(
                              1, std::memory_order_relaxed)));
            return std::string(buf);
        }

        bool readExact(Handle h, void *data, uint32_t size)
        {
            auto *out = static_cast<uint8_t *>(data);
            uint32_t done = 0;
            OVERLAPPED ov{};
            ov.hEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);
            if (ov.hEvent == nullptr)
            {
                return false;
            }
            HANDLE hFile = reinterpret_cast<HANDLE>(h);
            while (done < size)
            {
                ResetEvent(ov.hEvent);
                ov.Offset = 0;
                ov.OffsetHigh = 0;
                DWORD chunk = 0;
                if (!ReadFile(hFile, out + done, size - done, &chunk, &ov))
                {
                    if (GetLastError() != ERROR_IO_PENDING)
                    {
                        CloseHandle(ov.hEvent);
                        return false;
                    }
                    // Wait with periodic checks so CancelIoEx can break us out.
                    while (WaitForSingleObject(ov.hEvent, 100) == WAIT_TIMEOUT)
                    {
                        // Keep waiting — CancelIoEx will signal the event.
                    }
                    if (!GetOverlappedResult(hFile, &ov, &chunk, FALSE))
                    {
                        CloseHandle(ov.hEvent);
                        return false;
                    }
                }
                if (chunk == 0)
                {
                    CloseHandle(ov.hEvent);
                    return false;
                }
                done += chunk;
            }
            CloseHandle(ov.hEvent);
            return true;
        }

        bool writeExact(Handle h, const void *data, uint32_t size)
        {
            const auto *in = static_cast<const uint8_t *>(data);
            uint32_t done = 0;
            OVERLAPPED ov{};
            ov.hEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);
            if (ov.hEvent == nullptr)
            {
                return false;
            }
            HANDLE hFile = reinterpret_cast<HANDLE>(h);
            while (done < size)
            {
                ResetEvent(ov.hEvent);
                ov.Offset = 0;
                ov.OffsetHigh = 0;
                DWORD chunk = 0;
                if (!WriteFile(hFile, in + done, size - done, &chunk, &ov))
                {
                    if (GetLastError() != ERROR_IO_PENDING)
                    {
                        CloseHandle(ov.hEvent);
                        return false;
                    }
                    while (WaitForSingleObject(ov.hEvent, 100) == WAIT_TIMEOUT)
                    {
                    }
                    if (!GetOverlappedResult(hFile, &ov, &chunk, FALSE))
                    {
                        CloseHandle(ov.hEvent);
                        return false;
                    }
                }
                if (chunk == 0)
                {
                    CloseHandle(ov.hEvent);
                    return false;
                }
                done += chunk;
            }
            CloseHandle(ov.hEvent);
            return true;
        }

        bool lookupListener(Handle token, ListenerEntry *out)
        {
            std::lock_guard<std::mutex> lock(g_listenerMutex);
            auto it = g_listeners.find(token);
            if (it == g_listeners.end())
            {
                return false;
            }
            *out = it->second;
            return true;
        }
    } // namespace

    Handle serverSocket(const char *name)
    {
        const char *safeName = (name != nullptr) ? name : "";
        const uint64_t hash = fnv1a64(safeName);
        SetLastError(0);
        HANDLE stopEvent = CreateEventA(nullptr, TRUE, FALSE, listenerStopName(hash).c_str());
        if (stopEvent == nullptr)
        {
            return kInvalidHandle;
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            CloseHandle(stopEvent);
            return kInvalidHandle;
        }

        Handle token = reinterpret_cast<Handle>(g_nextListenerToken.fetch_add(1, std::memory_order_relaxed));
        {
            std::lock_guard<std::mutex> lock(g_listenerMutex);
            g_listeners.emplace(token, ListenerEntry{hash, stopEvent});
        }
        return token;
    }

    Handle clientSocket(const char *name)
    {
        const char *safeName = (name != nullptr) ? name : "";
        std::string path = pipeName(fnv1a64(safeName));
        DWORD backoffMs = 10;
        for (int attempt = 0; attempt < 50; ++attempt)
        {
            HANDLE h = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                                   OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
            if (h != INVALID_HANDLE_VALUE)
            {
                return reinterpret_cast<Handle>(h);
            }

            DWORD err = GetLastError();
            if (err == ERROR_PIPE_BUSY)
            {
                if (!WaitNamedPipeA(path.c_str(), backoffMs))
                {
                    Sleep(backoffMs);
                }
            }
            else if (err == ERROR_FILE_NOT_FOUND)
            {
                Sleep(backoffMs);
            }
            else
            {
                break;
            }

            if (backoffMs < 200)
            {
                backoffMs *= 2;
                if (backoffMs > 200)
                    backoffMs = 200;
            }
        }

        return kInvalidHandle;
    }

    Handle acceptClient(Handle listenFd)
    {
        ListenerEntry entry{};
        if (!lookupListener(listenFd, &entry))
        {
            return kInvalidHandle;
        }

        std::string path = pipeName(entry.serviceHash);
        HANDLE pipe = CreateNamedPipeA(path.c_str(),
                                       PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                                       PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                       PIPE_UNLIMITED_INSTANCES, 4096, 4096, 0, nullptr);
        if (pipe == INVALID_HANDLE_VALUE)
        {
            return kInvalidHandle;
        }

        OVERLAPPED ov{};
        ov.hEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);
        if (ov.hEvent == nullptr)
        {
            CloseHandle(pipe);
            return kInvalidHandle;
        }

        BOOL connected = ConnectNamedPipe(pipe, &ov);
        if (!connected)
        {
            DWORD err = GetLastError();
            if (err == ERROR_PIPE_CONNECTED)
            {
                CloseHandle(ov.hEvent);
                return reinterpret_cast<Handle>(pipe);
            }

            if (err != ERROR_IO_PENDING)
            {
                CloseHandle(ov.hEvent);
                CloseHandle(pipe);
                return kInvalidHandle;
            }

            HANDLE waits[2] = {ov.hEvent, entry.stopEvent};
            DWORD waitRc = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
            if (waitRc == WAIT_OBJECT_0)
            {
                DWORD ignored = 0;
                if (!GetOverlappedResult(pipe, &ov, &ignored, FALSE))
                {
                    CloseHandle(ov.hEvent);
                    CloseHandle(pipe);
                    return kInvalidHandle;
                }
                CloseHandle(ov.hEvent);
                return reinterpret_cast<Handle>(pipe);
            }

            CancelIoEx(pipe, &ov);
            CloseHandle(ov.hEvent);
            CloseHandle(pipe);
            return kInvalidHandle;
        }

        CloseHandle(ov.hEvent);
        return reinterpret_cast<Handle>(pipe);
    }

    int sendFd(Handle sockFd, Handle /*fdToSend*/, const void *data, uint32_t dataLen)
    {
        if (dataLen == 0)
        {
            return 0;
        }
        return writeExact(sockFd, data, dataLen) ? 0 : -1;
    }

    int recvFd(Handle sockFd, Handle *receivedFd, void *data, uint32_t dataLen)
    {
        if (dataLen > 0 && !readExact(sockFd, data, dataLen))
        {
            return -1;
        }

        *receivedFd = kInvalidHandle;
        if (dataLen >= sizeof(SharedMemoryHandshake))
        {
            auto *hs = static_cast<SharedMemoryHandshake *>(data);
            if (hs->shmName[0] != '\0')
            {
                HANDLE mapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, hs->shmName);
                if (mapping != nullptr)
                {
                    *receivedFd = reinterpret_cast<Handle>(mapping);
                }
            }
        }
        return static_cast<int>(dataLen);
    }

    int sendSignal(Handle sockFd)
    {
        DWORD bytesAvail = 0;
        DWORD totalBytesAvail = 0;
        // Check if the pipe already has pending data (signal coalescing).
        // PeekNamedPipe on write end fails, so we attempt a non-blocking
        // overlapped write instead.
        OVERLAPPED ov{};
        ov.hEvent = CreateEventA(nullptr, TRUE, FALSE, nullptr);
        if (ov.hEvent == nullptr)
        {
            return -1;
        }

        uint8_t byte = 1;
        DWORD written = 0;
        BOOL ok = WriteFile(reinterpret_cast<HANDLE>(sockFd), &byte, 1, &written, &ov);
        if (ok)
        {
            CloseHandle(ov.hEvent);
            return 0;
        }

        DWORD err = GetLastError();
        if (err != ERROR_IO_PENDING)
        {
            CloseHandle(ov.hEvent);
            return -1;
        }

        // Wait with a short timeout; if the write would block, treat as coalesced.
        DWORD waitRc = WaitForSingleObject(ov.hEvent, 50);
        if (waitRc == WAIT_OBJECT_0)
        {
            if (GetOverlappedResult(reinterpret_cast<HANDLE>(sockFd), &ov, &written, FALSE))
            {
                CloseHandle(ov.hEvent);
                return 0;
            }
        }

        CancelIoEx(reinterpret_cast<HANDLE>(sockFd), &ov);
        CloseHandle(ov.hEvent);
        // Signal already pending — coalesced, treat as success.
        return 0;
    }

    int recvSignal(Handle sockFd)
    {
        uint8_t byte = 0;
        if (!readExact(sockFd, &byte, 1))
        {
            return -1;
        }

        // Drain any residual bytes so we don't wake up N-1 extra times.
        for (;;)
        {
            DWORD avail = 0;
            if (!PeekNamedPipe(reinterpret_cast<HANDLE>(sockFd),
                               nullptr, 0, nullptr, &avail, nullptr)
                || avail == 0)
            {
                break;
            }
            uint8_t drain[64];
            DWORD toRead = (avail < sizeof(drain)) ? avail : sizeof(drain);
            if (!readExact(sockFd, drain, toRead))
            {
                break;
            }
        }

        return 0;
    }

    // TODO: Named pipes don't support SO_SNDTIMEO/SO_RCVTIMEO-style
    // timeouts. The handshake relies on overlapped I/O timeouts instead.
    // This is a known limitation on the Windows platform.
    int setSocketTimeouts(Handle /*sockFd*/, uint32_t /*timeoutMs*/) { return 0; }

    int shutdownConnection(Handle sockFd)
    {
        ListenerEntry entry{};
        if (lookupListener(sockFd, &entry))
        {
            return SetEvent(entry.stopEvent) ? 0 : -1;
        }
        HANDLE h = reinterpret_cast<HANDLE>(sockFd);
        // Cancel any pending overlapped I/O, then disconnect the pipe
        // so any future ReadFile/WriteFile fails immediately.
        CancelIoEx(h, nullptr);
        FlushFileBuffers(h);
        DisconnectNamedPipe(h);
        return 0;
    }

    Handle shmCreate(uint32_t size, const char *name)
    {
        std::string mappingName = (name != nullptr) ? std::string(name) : sharedMemoryName("aether");
        HANDLE h = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size,
                                      mappingName.c_str());
        if (h == nullptr)
        {
            return kInvalidHandle;
        }
        return reinterpret_cast<Handle>(h);
    }

    void *mapSharedMemory(Handle shmFd, uint32_t size)
    {
        return MapViewOfFile(reinterpret_cast<HANDLE>(shmFd), FILE_MAP_ALL_ACCESS, 0, 0, size);
    }

    void unmapSharedMemory(void *base, uint32_t /*size*/)
    {
        if (base != nullptr)
        {
            UnmapViewOfFile(base);
        }
    }

    void closeFd(Handle fd)
    {
        if (fd == kInvalidHandle)
        {
            return;
        }

        HANDLE eventToClose = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_listenerMutex);
            auto it = g_listeners.find(fd);
            if (it != g_listeners.end())
            {
                eventToClose = it->second.stopEvent;
                g_listeners.erase(it);
            }
        }
        if (eventToClose)
        {
            CloseHandle(eventToClose);
            return;
        }

        CloseHandle(reinterpret_cast<HANDLE>(fd));
    }

} // namespace aether::ipc::platform

#endif // _WIN32
