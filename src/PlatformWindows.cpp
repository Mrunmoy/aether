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
#include <new>
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
        std::mutex g_socketTimeoutMutex;
        std::unordered_map<Handle, DWORD> g_socketTimeouts;
        std::atomic<uintptr_t> g_nextListenerToken{1};
        std::atomic<uint32_t> g_nextSharedMemoryId{1};

        // High bit set to ensure listener tokens never collide with real HANDLE values.
        constexpr uintptr_t kTokenTag = uintptr_t(1) << (sizeof(uintptr_t) * 8 - 1);

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

        DWORD lookupSocketTimeout(Handle handle)
        {
            std::lock_guard<std::mutex> lock(g_socketTimeoutMutex);
            auto it = g_socketTimeouts.find(handle);
            return (it != g_socketTimeouts.end()) ? it->second : INFINITE;
        }

        bool waitForOverlapped(HANDLE file, OVERLAPPED *ov, DWORD timeoutMs, DWORD *transferred)
        {
            DWORD waitRc = WaitForSingleObject(ov->hEvent, timeoutMs);
            if (waitRc != WAIT_OBJECT_0)
            {
                CancelIoEx(file, ov);
                return false;
            }
            return GetOverlappedResult(file, ov, transferred, FALSE) != FALSE;
        }

        bool readExact(Handle h, void *data, uint32_t size)
        {
            auto *out = static_cast<uint8_t *>(data);
            uint32_t done = 0;
            const DWORD timeoutMs = lookupSocketTimeout(h);
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
                    if (!waitForOverlapped(hFile, &ov, timeoutMs, &chunk))
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
            const DWORD timeoutMs = lookupSocketTimeout(h);
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
                    if (!waitForOverlapped(hFile, &ov, timeoutMs, &chunk))
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

        // ── Async Accept / Signal State (RunLoop integration) ───────────

        struct AsyncAcceptState
        {
            HANDLE eventHandle = nullptr;
            HANDLE currentPipe = INVALID_HANDLE_VALUE;
            OVERLAPPED ov{};
            uint64_t serviceHash = 0;
            bool pendingConnect = false;
        };

        struct AsyncSignalState
        {
            HANDLE eventHandle = nullptr;
            OVERLAPPED ov{};
            uint8_t buf = 0;
            bool pendingRead = false;
        };

        // Lock ordering: g_asyncAcceptMutex, g_asyncSignalMutex → g_socketTimeoutMutex.
        // Never acquire g_asyncAcceptMutex or g_asyncSignalMutex while holding
        // g_socketTimeoutMutex.

        std::mutex g_asyncAcceptMutex;
        std::unordered_map<Handle, AsyncAcceptState *> g_asyncAccepts;

        std::mutex g_asyncSignalMutex;
        std::unordered_map<Handle, AsyncSignalState *> g_asyncSignals;

        HANDLE createPipeInstance(uint64_t serviceHash)
        {
            std::string path = pipeName(serviceHash);
            return CreateNamedPipeA(path.c_str(),
                                    PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                                    PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                    PIPE_UNLIMITED_INSTANCES, 4096, 4096, 0, nullptr);
        }

        bool armAccept(AsyncAcceptState *state)
        {
            ResetEvent(state->eventHandle);
            state->pendingConnect = false;

            std::memset(&state->ov, 0, sizeof(state->ov));
            state->ov.hEvent = state->eventHandle;

            BOOL connected = ConnectNamedPipe(state->currentPipe, &state->ov);
            if (connected)
            {
                SetEvent(state->eventHandle);
                return true;
            }

            DWORD err = GetLastError();
            if (err == ERROR_PIPE_CONNECTED)
            {
                SetEvent(state->eventHandle);
                return true;
            }
            if (err == ERROR_IO_PENDING)
            {
                state->pendingConnect = true;
                return true;
            }
            return false;
        }

        bool armRecvSignal(AsyncSignalState *state, HANDLE pipe)
        {
            ResetEvent(state->eventHandle);
            state->pendingRead = false;
            state->buf = 0;

            std::memset(&state->ov, 0, sizeof(state->ov));
            state->ov.hEvent = state->eventHandle;

            DWORD bytesRead = 0;
            BOOL ok = ReadFile(pipe, &state->buf, 1, &bytesRead, &state->ov);
            if (ok)
            {
                if (bytesRead == 0)
                {
                    return false;
                }
                SetEvent(state->eventHandle);
                return true;
            }

            DWORD err = GetLastError();
            if (err == ERROR_IO_PENDING)
            {
                state->pendingRead = true;
                return true;
            }
            return false;
        }

        Handle completeAcceptLocked(AsyncAcceptState *state)
        {
            if (state->pendingConnect)
            {
                DWORD ignored = 0;
                if (!GetOverlappedResult(state->currentPipe, &state->ov,
                                         &ignored, FALSE))
                {
                    // Accept failed — try to re-arm.
                    CloseHandle(state->currentPipe);
                    state->currentPipe = createPipeInstance(state->serviceHash);
                    if (state->currentPipe != INVALID_HANDLE_VALUE)
                    {
                        if (!armAccept(state))
                        {
                            CloseHandle(state->currentPipe);
                            state->currentPipe = INVALID_HANDLE_VALUE;
                            ResetEvent(state->eventHandle);
                        }
                    }
                    else
                    {
                        ResetEvent(state->eventHandle);
                    }
                    return kInvalidHandle;
                }
            }

            Handle result = reinterpret_cast<Handle>(state->currentPipe);

            // Create new pipe instance and re-arm for next accept.
            state->currentPipe = createPipeInstance(state->serviceHash);
            if (state->currentPipe != INVALID_HANDLE_VALUE)
            {
                if (!armAccept(state))
                {
                    CloseHandle(state->currentPipe);
                    state->currentPipe = INVALID_HANDLE_VALUE;
                    ResetEvent(state->eventHandle);
                }
            }
            else
            {
                ResetEvent(state->eventHandle);
            }

            return result;
        }

        int completeRecvSignalLocked(Handle sockFd, AsyncSignalState *state)
        {
            HANDLE pipe = reinterpret_cast<HANDLE>(sockFd);

            if (state->pendingRead)
            {
                DWORD bytesRead = 0;
                if (!GetOverlappedResult(pipe, &state->ov, &bytesRead, FALSE))
                {
                    return -1;
                }
                if (bytesRead == 0)
                {
                    return -1;
                }
            }

            // Drain residual signal bytes so we don't wake up extra times.
            for (;;)
            {
                DWORD avail = 0;
                if (!PeekNamedPipe(pipe, nullptr, 0, nullptr, &avail, nullptr)
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

            // Re-arm for next signal.
            if (!armRecvSignal(state, pipe))
            {
                ResetEvent(state->eventHandle);
                return -1;
            }

            return 0;
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

        Handle token = reinterpret_cast<Handle>(
            kTokenTag | g_nextListenerToken.fetch_add(1, std::memory_order_relaxed));
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
        // Fast path: complete a pending async accept (RunLoop mode).
        {
            std::lock_guard<std::mutex> lock(g_asyncAcceptMutex);
            auto it = g_asyncAccepts.find(listenFd);
            if (it != g_asyncAccepts.end())
            {
                return completeAcceptLocked(it->second);
            }
        }

        // Blocking accept path (threaded mode).
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
        uint8_t dummy = 0;
        const void *payload = (dataLen > 0) ? data : &dummy;
        const uint32_t payloadLen = (dataLen > 0) ? dataLen : 1;
        return writeExact(sockFd, payload, payloadLen) ? 0 : -1;
    }

    int recvFd(Handle sockFd, Handle *receivedFd, void *data, uint32_t dataLen)
    {
        uint8_t dummy = 0;
        void *payload = (dataLen > 0) ? data : &dummy;
        const uint32_t payloadLen = (dataLen > 0) ? dataLen : 1;
        if (!readExact(sockFd, payload, payloadLen))
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
        return static_cast<int>(payloadLen);
    }

    int sendSignal(Handle sockFd)
    {
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
            // Write completed with error (broken pipe, etc.)
            CancelIoEx(reinterpret_cast<HANDLE>(sockFd), &ov);
            CloseHandle(ov.hEvent);
            return -1;
        }

        CancelIoEx(reinterpret_cast<HANDLE>(sockFd), &ov);
        CloseHandle(ov.hEvent);
        // Signal already pending — coalesced, treat as success.
        return 0;
    }

    int recvSignal(Handle sockFd)
    {
        // Fast path: complete a pending async signal (RunLoop mode).
        {
            std::lock_guard<std::mutex> lock(g_asyncSignalMutex);
            auto it = g_asyncSignals.find(sockFd);
            if (it != g_asyncSignals.end())
            {
                return completeRecvSignalLocked(sockFd, it->second);
            }
        }

        // Blocking path (threaded mode).
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

    // ── Async Accept / Signal (RunLoop integration) ──────────────────

    Handle beginAccept(Handle listenFd)
    {
        ListenerEntry entry{};
        if (!lookupListener(listenFd, &entry))
        {
            return kInvalidHandle;
        }

        auto *state = new (std::nothrow) AsyncAcceptState;
        if (state == nullptr)
        {
            return kInvalidHandle;
        }
        state->serviceHash = entry.serviceHash;
        state->eventHandle = CreateEventA(nullptr, TRUE, FALSE, nullptr);
        if (state->eventHandle == nullptr)
        {
            delete state;
            return kInvalidHandle;
        }

        state->currentPipe = createPipeInstance(entry.serviceHash);
        if (state->currentPipe == INVALID_HANDLE_VALUE)
        {
            CloseHandle(state->eventHandle);
            delete state;
            return kInvalidHandle;
        }

        if (!armAccept(state))
        {
            CloseHandle(state->currentPipe);
            CloseHandle(state->eventHandle);
            delete state;
            return kInvalidHandle;
        }

        Handle eventAsHandle = reinterpret_cast<Handle>(state->eventHandle);
        {
            std::lock_guard<std::mutex> lock(g_asyncAcceptMutex);
            g_asyncAccepts[listenFd] = state;
        }
        return eventAsHandle;
    }

    void cancelAccept(Handle listenFd)
    {
        AsyncAcceptState *state = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_asyncAcceptMutex);
            auto it = g_asyncAccepts.find(listenFd);
            if (it == g_asyncAccepts.end())
            {
                return;
            }
            state = it->second;
            g_asyncAccepts.erase(it);
        }

        if (state->pendingConnect)
        {
            CancelIoEx(state->currentPipe, &state->ov);
            DWORD ignored = 0;
            GetOverlappedResult(state->currentPipe, &state->ov, &ignored, TRUE);
        }
        if (state->currentPipe != INVALID_HANDLE_VALUE)
        {
            CloseHandle(state->currentPipe);
        }
        // Event handle is NOT closed — caller owns it and must close it
        // after RunLoop::removeSource has fully processed the removal.
        delete state;
    }

    Handle beginRecvSignal(Handle sockFd)
    {
        auto *state = new (std::nothrow) AsyncSignalState;
        if (state == nullptr)
        {
            return kInvalidHandle;
        }
        state->eventHandle = CreateEventA(nullptr, TRUE, FALSE, nullptr);
        if (state->eventHandle == nullptr)
        {
            delete state;
            return kInvalidHandle;
        }

        HANDLE pipe = reinterpret_cast<HANDLE>(sockFd);
        if (!armRecvSignal(state, pipe))
        {
            CloseHandle(state->eventHandle);
            delete state;
            return kInvalidHandle;
        }

        Handle eventAsHandle = reinterpret_cast<Handle>(state->eventHandle);
        {
            std::lock_guard<std::mutex> lock(g_asyncSignalMutex);
            g_asyncSignals[sockFd] = state;
        }
        return eventAsHandle;
    }

    void cancelRecvSignal(Handle sockFd)
    {
        AsyncSignalState *state = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_asyncSignalMutex);
            auto it = g_asyncSignals.find(sockFd);
            if (it == g_asyncSignals.end())
            {
                return;
            }
            state = it->second;
            g_asyncSignals.erase(it);
        }

        if (state->pendingRead)
        {
            CancelIoEx(reinterpret_cast<HANDLE>(sockFd), &state->ov);
            DWORD ignored = 0;
            GetOverlappedResult(reinterpret_cast<HANDLE>(sockFd), &state->ov,
                                &ignored, TRUE);
        }
        // Event handle is NOT closed — caller owns it.
        delete state;
    }

    // Named pipes do not expose SO_SNDTIMEO/SO_RCVTIMEO directly, so we emulate
    // the timeout contract in readExact/writeExact by bounding each overlapped
    // wait on the connection handle.
    int setSocketTimeouts(Handle sockFd, uint32_t timeoutMs)
    {
        std::lock_guard<std::mutex> lock(g_socketTimeoutMutex);
        g_socketTimeouts[sockFd] = timeoutMs;
        return 0;
    }

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
            {
                std::lock_guard<std::mutex> timeoutLock(g_socketTimeoutMutex);
                g_socketTimeouts.erase(fd);
            }
            CloseHandle(eventToClose);
            return;
        }

        {
            std::lock_guard<std::mutex> timeoutLock(g_socketTimeoutMutex);
            g_socketTimeouts.erase(fd);
        }
        CloseHandle(reinterpret_cast<HANDLE>(fd));
    }

} // namespace aether::ipc::platform

#endif // _WIN32
