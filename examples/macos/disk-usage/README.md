# Disk Usage Monitor Example

Monitor a simulated set of mounted volumes through typed RPC calls and low-space
notifications.

## What You'll Learn
- how to model inventory-style data with lookup and enumerate methods
- how to use notifications for threshold-based warnings
- how to build a small monitoring client around generated code

## Prerequisites
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `DiskUsage.idl` | volume info and low-space notification contract |
| `disk_device.cpp` | simulated volume state and threshold logic |
| `disk_client.cpp` | CLI client for listing and querying volumes |
| `disk_test.cpp` | example-level tests |

## Step 1: Read the IDL
`DiskUsage.idl` defines:
- `VolumeInfo` as the shared struct
- methods to get usage by mount point, count volumes, and fetch a volume by index
- one notification, `DiskSpaceLow`, when a volume crosses the usage threshold

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/macos/disk-usage/DiskUsage.idl --outdir examples/macos/disk-usage/gen
```

## Step 3: Review the User Code
- `disk_device.cpp` owns the simulated volume list and low-space detection.
- `disk_client.cpp` is a small REPL for querying the current inventory.
- Aether handles request routing and notification delivery.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/macos/disk-usage/disk_device

# Terminal 2
./build/examples/macos/disk-usage/disk_client
```

### Dashboard Commands

| Command | Description |
|---------|-------------|
| `list` | Show all volumes with usage summary |
| `check <mount>` | Query a specific mount point (for example `check /`) |
| `quit` | Disconnect and exit |

## Expected Output
Client session:

```text
> list
Mount        Filesystem   Total      Used
/            apfs         500 GB     350 GB
> check /
```

When a simulated volume crosses the limit, the client prints a `DiskSpaceLow`
notification.

## What Just Happened
The client queried a typed inventory of volumes and then stayed connected long
enough to receive warnings. That pattern works well when you need both a
current snapshot and asynchronous alerts from the same service.

## Testing
Run from the repository root (requires a build with `-e`):

```bash
ctest --test-dir build --output-on-failure -R disk_tests
```

## What To Modify Next
- add another threshold tier such as a warning before the critical notification
- point the service at real host filesystem data instead of the built-in simulation

## Testing
Run from the repository root:

```bash
ctest --test-dir build --output-on-failure -R disk_tests
```

## Related Examples
- [`../battery-monitor/`](../battery-monitor/) for another monitor-style service
- [`../../linux/sysmon/`](../../linux/sysmon/) for a host-backed monitor with real Linux metrics
