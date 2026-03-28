# Audio DSP — Simulated Effects Processor

Demonstrates an IPC service that models a DSP effects processor with 8 effect
slots, parameter control, and real-time signal level monitoring.

## IDL

`AudioDsp.idl` defines:

- **Enum** `EffectType` — EQ, Compressor, Reverb, Delay, Limiter
- **Structs** — `EffectSlot` (slot state + 3 params + name), `SignalLevel` (peak/RMS/clipping)
- **Service** `AudioDsp` — AddEffect, RemoveEffect, SetParameter, GetEffectSlot, GetSignalLevel, GetEffectCount
- **Notifications** — ClipDetected, LevelUpdate

## Build

```bash
cmake -B build -DMS_IPC_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

## Run

```bash
# Terminal 1 — start the DSP device
./build/examples/macos/audio-dsp/dsp_device

# Terminal 2 — connect the dashboard
./build/examples/macos/audio-dsp/dsp_dashboard
```

### Dashboard commands

| Command | Description |
|---------|-------------|
| `add <slot> <eq\|comp\|reverb\|delay\|limiter> <name>` | Place effect in slot |
| `remove <slot>` | Clear a slot |
| `param <slot> <0-2> <value>` | Set parameter |
| `level` | Show signal levels |
| `chain` | Show full effect chain |
| `quit` | Exit |

### Sample output

```
=== DSP Effect Chain ===
  Slot 0: [EQ        ] "Main EQ"  P1=2.5  P2=0.8  P3=1.0  [ON]
  Slot 1: [Compressor] "Bus Comp"  P1=4.0  P2=0.5  P3=0.3  [ON]
  Slot 2: (empty)
  ...
  Signal: Peak=-6.2dB  RMS=-12.4dB  [OK]
```

## Tests

```bash
ctest --test-dir build --output-on-failure -R AudioDspTest
```

Tests cover slot management, parameter updates, effect counting, signal level
queries, invalid slot handling, and both ClipDetected and LevelUpdate
notifications.
