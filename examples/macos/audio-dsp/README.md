# Audio DSP — Simulated Effects Processor

Model a DSP effect chain with typed RPC calls for control and notifications for
metering and clipping events.

## What You'll Learn
- how to represent a configurable processing chain in IDL
- how to mix stateful control methods with real-time notifications
- how to build a dashboard-style client for a more specialized domain

## Prerequisites
- repository root checkout
- `python3 build.py -e`

## Files That Matter
| File | Why it matters |
|------|----------------|
| `AudioDsp.idl` | effect-chain contract and shared types |
| `dsp_device.cpp` | simulated DSP device implementation |
| `dsp_dashboard.cpp` | CLI dashboard for building and inspecting the chain |
| `dsp_test.cpp` | example-level tests |

## Step 1: Read the IDL
`AudioDsp.idl` defines:
- effect types such as EQ, compressor, reverb, delay, and limiter
- `EffectSlot` and `SignalLevel` as shared structs
- methods to add/remove effects, tweak parameters, and inspect the chain
- notifications for clipping and level updates

## Step 2: Generate Code
Run from the repository root:

```bash
python3 -m tools.ipcgen examples/macos/audio-dsp/AudioDsp.idl --outdir examples/macos/audio-dsp/gen
```

## Step 3: Review the User Code
- `dsp_device.cpp` manages the simulated effect slots and periodically updates
  the signal level.
- `dsp_dashboard.cpp` is the interactive client for building a chain and
  reading back state.
- Aether handles typed marshaling and event delivery between them.

## Build
Run from the repository root:

```bash
python3 build.py -e
```

## Run
Run from the repository root:

```bash
# Terminal 1
./build/examples/macos/audio-dsp/dsp_device

# Terminal 2
./build/examples/macos/audio-dsp/dsp_dashboard
```

## Expected Output
Client:

```text
> add 0 eq MainEQ
> param 0 0 2.5
> chain
Slot 0: [EQ] "MainEQ"
Signal: Peak=-6.2dB RMS=-12.4dB
```

Notifications appear as the simulated signal level changes or clipping is hit.

## What Just Happened
This example treats the generated API as a control plane for a stateful device.
Methods mutate and query the effect chain, while notifications expose live
conditions without forcing the client to poll constantly.

## What To Modify Next
- add another effect parameter or another notification such as bypass-state change
- make the dashboard cache the current chain and redraw it on every notification

## Related Examples
- [`../ble-peripheral/`](../ble-peripheral/) for another stateful device with notifications
- [`../../exhaust-analyzer/`](../../exhaust-analyzer/) for a fuller app-style example with a GUI
