# Process Monitor Example

Monitor a simulated process list with typed queries and notifications for
process start/exit events.

## What You'll Learn
- how to model an inventory of runtime entities in Aether
- how to pair lookup methods with lifecycle notifications
- how to build a small operator client around generated code

## Prerequisites
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `ProcessMonitor.idl` | process inventory and notification contract |
| `procmon_device.cpp` | simulated process table and lifecycle events |
| `procmon_client.cpp` | CLI client for listing and querying processes |
| `procmon_test.cpp` | example-level tests |

## Step 1: Read the IDL
`ProcessMonitor.idl` defines:
- `ProcessInfo` as the shared struct
- methods to count processes, query one by PID, or access one by index
- notifications for `ProcessStarted` and `ProcessExited`

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/windows/process-monitor/ProcessMonitor.idl --outdir examples/windows/process-monitor/gen
```

## Step 3: Review the User Code
- `procmon_device.cpp` maintains the simulated process list and emits lifecycle
  notifications when it changes.
- `procmon_client.cpp` uses the generated client to list processes and inspect
  one by PID.
- Aether provides the typed IPC plumbing between them.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/windows/process-monitor/procmon_device

# Terminal 2
./build/examples/windows/process-monitor/procmon_client
```

## Expected Output
Client session:

```text
> list
PID   Name
...
> info 1234
```

When the simulation adds or removes a process, the client prints the
corresponding notification automatically.

## What Just Happened
The client can pull the current process snapshot through methods and then stay
connected for lifecycle changes. That combination works well whenever a service
has both current state and event history concerns.

## Testing
Run from the repository root (requires a build with `-e`):

```bash
ctest --test-dir build --output-on-failure -R procmon_tests
```

## What To Modify Next
- add a method for filtering by process name
- keep a rolling event log in the client instead of printing notifications inline

## Testing
Run from the repository root:

```bash
ctest --test-dir build --output-on-failure -R procmon_tests
```

## Related Examples
- [`service-status/`](../service-status/) for another lifecycle-monitor example
- [`../../linux/sysmon/`](../../linux/sysmon/) for a monitoring service backed by real host data
