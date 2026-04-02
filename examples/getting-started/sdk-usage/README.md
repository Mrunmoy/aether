# SDK Usage Example: generated wrappers against a packaged SDK

This example shows how to consume Aether as a packaged SDK instead of building
the runtime from source in your project.

## What You'll Learn

- when to use `ipcgen --backend c_api`
- how generated wrappers depend only on `aether_ipc.h`
- how to build a client and server against an extracted SDK tarball
- how the SDK path differs from the in-tree source-build path

## Prerequisites

- Linux or another platform with a matching Aether SDK tarball
- C++17 compiler, CMake, and Python 3
- an extracted Aether SDK tarball
- a source checkout if you want to run `ipcgen` locally

If you are starting from this repository, produce the SDK tarball first:

```bash
python3 build.py -p
```

That creates an `aether-sdk-*.tar.gz` file in the repo root.

## Files That Matter

| File | Why it matters |
|------|----------------|
| [`TemperatureSensor.idl`](TemperatureSensor.idl) | Defines the service contract |
| [`gen/server/TemperatureSensor.h`](gen/server/TemperatureSensor.h) | Generated typed server wrapper for the C API backend |
| [`gen/client/TemperatureSensor.h`](gen/client/TemperatureSensor.h) | Generated typed client wrapper for the C API backend |
| [`temp_server.cpp`](temp_server.cpp) | User-written server implementation |
| [`temp_client.cpp`](temp_client.cpp) | User-written client implementation |
| [`CMakeLists.txt`](CMakeLists.txt) | Builds both binaries against the extracted SDK |

## Step 1: Read the IDL

[`TemperatureSensor.idl`](TemperatureSensor.idl) defines:

- `GetTemperature`
- `SetThreshold`
- `OverTemperature`

The contract is still expressed in IDL, even though this example does not link
against the full Aether source tree.

## Step 2: Generate Code

Run `ipcgen` with the `c_api` backend:

```bash
python3 -m tools.ipcgen examples/getting-started/sdk-usage/TemperatureSensor.idl \
    --outdir examples/getting-started/sdk-usage/gen --backend c_api
```

The generated wrappers are already checked in. Rerun this command only after
changing the IDL.

This generates wrappers that:

- expose the same typed client and server API shape as the source-build path
- use `aether_ipc.h` internally
- do not require `ServiceBase` or `ClientBase` headers from the source tree

Generated outputs:

- `gen/server/TemperatureSensor.h`
- `gen/server/TemperatureSensor.cpp`
- `gen/client/TemperatureSensor.h`
- `gen/client/TemperatureSensor.cpp`

## Step 3: Review the User Code

[`temp_server.cpp`](temp_server.cpp):

- subclasses the generated server wrapper
- implements `handleGetTemperature(...)`
- implements `handleSetThreshold(...)`
- triggers a typed notification when the threshold is crossed

[`temp_client.cpp`](temp_client.cpp):

- subclasses the generated client wrapper
- overrides `onOverTemperature(...)`
- connects and calls the typed methods

Your code is still typed and high-level. The difference is that the generated
wrappers delegate through the stable C API rather than the in-tree C++ runtime
classes.

## Build

Extract the packaged SDK, then point CMake at the extracted directory.

```bash
tar xzf aether-sdk-*.tar.gz -C /tmp
SDK_DIR="$(find /tmp -maxdepth 1 -type d -name 'aether-sdk-*' | head -n 1)"
cmake -S examples/getting-started/sdk-usage -B build/sdk-usage \
    -DAETHER_SDK="$SDK_DIR"
cmake --build build/sdk-usage
```

## Run

```bash
# Terminal 1
./build/sdk-usage/temp_server

# Terminal 2
./build/sdk-usage/temp_client
```

## Expected Output

Server output:

```text
[server] Temperature sensor running. Ctrl+C to stop.
[server] GetTemperature → 72.5°C
[server] SetThreshold(high=50.0, low=-10.0)
[server] 72.5°C exceeds threshold — broadcasting
[server] Stopped.
```

Client output:

```text
[client] Connected to temp_sensor
[client] Current temperature: 72.5°C
[client] ⚠ Over-temperature: 72.5°C!
[client] Threshold set to [50.0, -10.0]°C
[client] Done.
```

The threshold log and notification may appear in either order depending on when
the callback runs.

## What Just Happened

You kept the IDL and typed generated API, but swapped the implementation layer
underneath it. Instead of inheriting from the in-tree C++ runtime classes, the
generated wrappers call the stable C API from the SDK artifact. That makes this
path suitable for downstream consumers that do not vendor the full Aether
source tree.

## What To Modify Next

- Add another method to [`TemperatureSensor.idl`](TemperatureSensor.idl),
  regenerate, and update both [`temp_server.cpp`](temp_server.cpp) and
  [`temp_client.cpp`](temp_client.cpp).
- Compare this path with [`../echo/`](../echo/) to decide whether your project
  wants source-build flexibility or SDK distribution.

## Related Examples

- [`../echo/`](../echo/) for the default source-build path
- [`../c-echo/`](../c-echo/) for the raw C API without code generation
