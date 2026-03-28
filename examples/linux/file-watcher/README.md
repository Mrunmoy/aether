# FileWatcher Example

A Linux directory watcher using `inotify`, demonstrating Aether IPC notifications.

## Overview

The **FileWatcher** service watches directories for file creation, modification, and deletion events. When a change is detected, it broadcasts a `FileChanged` notification to all connected clients.

### IDL

```
service FileWatcher {
    int WatchDirectory([in] string[256] path, [out] uint32 watchId);
    int UnwatchDirectory([in] uint32 watchId);
    int GetWatchCount([out] uint32 count);
};

notifications FileWatcher {
    void FileChanged([in] uint32 watchId, [in] FileEventType eventType,
                     [in] string[256] filename);
};
```

## Building

From the repository root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DMS_IPC_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

## Running

Start the server in one terminal:

```bash
./build/examples/linux/file-watcher/filewatcher_device
```

Start the client in another terminal:

```bash
./build/examples/linux/file-watcher/filewatcher_client /path/to/watch
```

Then create, modify, or delete files in the watched directory.

### Sample Output

**Server:**
```
FileWatcher device running. Press Ctrl-C to stop.
[device] WatchDirectory("/home/user/test") → watchId=1
[device] event: watchId=1 type=1 file=newfile.txt
```

**Client:**
```
Watching "/home/user/test" (watchId=1)
Listening for file events. Press Ctrl-C to stop.
[Created] newfile.txt (watchId=1)
[Modified] newfile.txt (watchId=1)
[Deleted] newfile.txt (watchId=1)
```

## Tests

```bash
ctest --test-dir build --output-on-failure -R filewatcher
```
