# FileWatcher Example

Use Linux `inotify` with generated Aether bindings to broadcast filesystem
events to one or more clients.

## What You'll Learn
- how to map an OS event source into an Aether notification stream
- how generated bindings work for a service with both methods and notifications
- how to test a Linux-specific integration with the same host runtime

## Prerequisites
- Linux
- repository root checkout
- a directory you can create and delete files in during the demo

## Files That Matter
| File | Why it matters |
|------|----------------|
| `FileWatcher.idl` | service contract for watch management and file-change notifications |
| `gen/server/FileWatcher.*` | generated server base class used by the device |
| `gen/client/FileWatcher.*` | generated client class used by the CLI |
| `filewatcher_device.cpp` | `inotify` integration and notification broadcast |
| `filewatcher_client.cpp` | CLI client that subscribes and prints events |
| `filewatcher_test.cpp` | regression coverage for the example service |

## Step 1: Read the IDL
`FileWatcher.idl` defines:
- methods to watch a directory, remove a watch, and query the active watch count
- `FileEventType` plus `WatchEntry` as the shared types
- one notification, `FileChanged`, for create/modify/delete events

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/platform/linux/file-watcher/FileWatcher.idl --outdir examples/platform/linux/file-watcher/gen
```

Generated outputs include:
- `examples/platform/linux/file-watcher/gen/FileWatcherTypes.h`
- `examples/platform/linux/file-watcher/gen/server/FileWatcher.h`
- `examples/platform/linux/file-watcher/gen/server/FileWatcher.cpp`
- `examples/platform/linux/file-watcher/gen/client/FileWatcher.h`
- `examples/platform/linux/file-watcher/gen/client/FileWatcher.cpp`

## Step 3: Review the User Code
- `filewatcher_device.cpp` subclasses the generated server and converts
  `inotify` events into `FileChanged` notifications.
- `filewatcher_client.cpp` issues `WatchDirectory` once, then prints incoming
  notifications until you stop it.
- Aether handles the client/server transport, framing, and notification fan-out.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/platform/linux/file-watcher/filewatcher_device

# Terminal 2
mkdir -p /tmp/aether-watch
./build/examples/platform/linux/file-watcher/filewatcher_client /tmp/aether-watch
```

Then create, edit, and delete files under `/tmp/aether-watch`.

## Expected Output
Server:

```text
FileWatcher device running. Press Ctrl-C to stop.
[device] WatchDirectory("/tmp/aether-watch") → watchId=1
```

Client:

```text
Watching "/tmp/aether-watch" (watchId=1)
[Created] demo.txt (watchId=1)
[Modified] demo.txt (watchId=1)
[Deleted] demo.txt (watchId=1)
```

## What Just Happened
The generated server and client code gave you a typed IPC surface, while the
device code translated native `inotify` events into Aether notifications. This
is the clearest example of plugging a real OS event source into the framework.

## Testing
Run from the repository root (requires a build with `-e`):

```bash
ctest --test-dir build --output-on-failure -R filewatcher_tests
```

## What To Modify Next
- add another notification for rename events
- connect two clients at once and watch the same directory from both

## Testing
Run from the repository root:

```bash
ctest --test-dir build --output-on-failure -R filewatcher_tests
```

## Related Examples
- [`../sysmon/`](../sysmon/) for another Linux-specific service with live alerts
- [`../../../getting-started/echo/`](../../../getting-started/echo/) for the smaller cross-platform generated path
