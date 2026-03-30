# Service Status Example

Monitor a simulated set of background services with typed queries and
state-change notifications.

## What You'll Learn
- how to model service inventory and state transitions in IDL
- how to pair query methods with change notifications
- how to build a small status client around generated code

## Prerequisites
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `ServiceStatus.idl` | service inventory and state-change contract |
| `svcstat_device.cpp` | simulated service state transitions |
| `svcstat_client.cpp` | CLI client for listing and querying services |
| `svcstat_test.cpp` | example-level tests |

## Step 1: Read the IDL
`ServiceStatus.idl` defines:
- `ServiceInfo` plus the `ServiceState` enum
- methods to look up one service, count them, and enumerate them by index
- one notification, `ServiceStateChanged`, when a service changes state

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/windows/service-status/ServiceStatus.idl --outdir examples/windows/service-status/gen
```

## Step 3: Review the User Code
- `svcstat_device.cpp` owns the simulated service inventory and toggles state.
- `svcstat_client.cpp` uses the generated client API for `list` and `status`
  commands while listening for notifications.
- Aether handles the transport, request dispatch, and notification delivery.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/windows/service-status/svcstat_device

# Terminal 2
./build/examples/windows/service-status/svcstat_client
```

## Expected Output
Client session:

```text
> list
Spooler   Running
BITS      Stopped
> status Spooler
```

As the simulation toggles service states, the client prints
`ServiceStateChanged` notifications automatically.

## What Just Happened
The service exposed both a queryable inventory and an event stream. That is the
same shape many operational tools need: inspect the current world, then watch
for changes without polling constantly.

## Testing
Run from the repository root (requires a build with `-e`):

```bash
ctest --test-dir build --output-on-failure -R svcstat_tests
```

## What To Modify Next
- add start/stop control methods to move from pure monitoring to service control
- add filtering in the client for only one watched service

## Testing
Run from the repository root:

```bash
ctest --test-dir build --output-on-failure -R svcstat_tests
```

## Related Examples
- [`process-monitor/`](../process-monitor/) for another lifecycle-monitor example
- [`../../linux/file-watcher/`](../../linux/file-watcher/) for a notification-oriented service driven by real host events
