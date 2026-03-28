# Disk Usage Monitor Example

A simulated disk/volume usage monitor built on the Aether IPC framework. The
device server maintains three simulated volumes — no platform-specific filesystem
APIs are required, so it compiles and runs on **any OS** (Linux, macOS, Windows).

## IDL Overview

The `DiskUsage` service exposes:

| Method           | Description                                |
|------------------|--------------------------------------------|
| `GetDiskUsage`   | Look up a volume by mount point            |
| `GetVolumeCount` | Return the number of simulated volumes     |
| `GetVolumeAt`    | Return volume info at a given index        |

Notification:
- **`DiskSpaceLow`** — fired when a volume exceeds 90% usage (mount point, used percent, free bytes)

## Simulated Volumes

| Mount   | Filesystem | Total  | Used   | Usage |
|---------|------------|--------|--------|-------|
| `/`     | ext4       | 500 GB | 350 GB | 70%   |
| `/home` | ext4       | 1 TB   | 800 GB | 80%   |
| `/tmp`  | tmpfs      | 50 GB  | 5 GB   | 10%   |

The server slowly increases usage on `/` by 1 GB every 3 seconds. When usage
exceeds 90%, it broadcasts a `DiskSpaceLow` notification to all connected clients.

## Building

From the repository root:

```bash
cmake -B build -DAETHER_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

## Running

```bash
# Terminal 1 — start the simulated device
./build/examples/macos/disk-usage/disk_device

# Terminal 2 — connect the CLI client
./build/examples/macos/disk-usage/disk_client
```

### Client Commands

```
> list                   # Show all volumes with bar chart
> check /                # Check a specific mount point
> check /nonexistent     # Returns error for unknown mount
> quit                   # Exit
```

### Sample Output

```
Mount        Filesystem   Total      Used       Free       Usage
-----        ----------   -----      ----       ----       -----------------
/            ext4         500 GB     350 GB     150 GB     [███████░░░]  70%
/home        ext4         1.0 TB     800 GB     200 GB     [████████░░]  80%
/tmp         tmpfs        50 GB      5 GB       45 GB      [█░░░░░░░░░]  10%
```

## Tests

```bash
ctest --test-dir build --output-on-failure -R DiskUsageTest
```

The test suite covers:
- `GetVolumeCount_ReturnsPositive`
- `GetVolumeAt_ValidIndex` / `GetVolumeAt_InvalidIndex`
- `GetDiskUsage_KnownMount` / `GetDiskUsage_UnknownMount`
- `DiskSpaceLow_NotificationFires`
- `UsedPercent_Consistent`
- `RunLoop_Dispatch`
