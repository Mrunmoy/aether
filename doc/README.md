# Documentation Guide

Use this directory as a guided reading path, not a flat index.

If you are new to Aether, start with the smallest document that answers your
next question, then go deeper only when you need the detail.

## Start Here

### 1. First success

- [Top-level README](../README.md)
- [`examples/echo/`](../examples/echo/)

Use these if you want to clone the repo, run the recommended example, and
confirm the framework works on your machine.

Stop here if your immediate goal is only "prove this works on my machine."
Everything below is for the next question after first success.

### 2. Mental model

- [Aether in 5 Minutes](AetherIn5Minutes.md)
- [Adoption Guide](AdoptionGuide.md)

Read this next if you want to understand:
- what `ipcgen` generates
- what the runtime handles
- what code you write
- threading, lifetime, common failures, and anti-patterns

Use the Adoption Guide first if you are evaluating whether Aether is a good fit
for a real project and want the short version before the deeper model.

### 3. Example navigation

- [Examples guide](../examples/README.md)
- [IDL cookbook](idl-cookbook.md)

Use these when you want to choose the next example or extend the IDL pattern
from the canonical `echo` example.

## Architecture And Design

### Visual overview

- [Architecture guide](architecture-guide.md)

Use this when you want the runtime layers, call flow, and transport model with
diagrams before reading the deeper design docs.

### Full design references

- [aether-hld.md](aether-hld.md)
- [aether-lld.md](aether-lld.md)
- [ipcgen-hld.md](ipcgen-hld.md)
- [ipcgen-lld.md](ipcgen-lld.md)

These are the deeper specifications for the runtime and generator internals.

## Vision And Direction

- [aether-vision.md](aether-vision.md)

Read this if you want the product intent, platform goals, and language-role
split across C++, Python, C API, and `aether-lite`.

## Practical Notes

- Generated files are checked into the repository for the documented examples.
  You only need to rerun `ipcgen` after changing an `.idl` file.
- If `python3` is not on your `PATH` on Windows, use `py -3` for the same
  commands shown in the docs.

## Internal Project Notes

These are useful for maintainers, but they are not part of the newcomer path:

- `dev-journal-phase5-p0.md`
- `dev-journal-phase5-p1.md`
- `phase5-runloop-plan.md`

If you only want to learn or adopt Aether, skip these.
