# Repository Guidelines

## Project Structure & Module Organization
`inc/` contains the public C++ headers for the core IPC library, and `src/` contains the matching implementations. `test/` holds Google Test coverage for the core library; `test/vendor/googletest/` is a submodule and must be present to build C++ tests. `tools/ipcgen/` contains the Python IDL code generator and its pytest suite in `tools/ipcgen/test/`. `examples/` provides sample services and generated code, while `doc/` contains architecture and design notes. `deps/` vendors `ms-ringbuffer` and `ms-runloop`.

## Build, Test, and Development Commands
Use the root helper script for the standard workflow:

- `python3 build.py` builds the library in `build/`.
- `python3 build.py -t` builds examples, runs `ctest`, and runs `pytest` for `ipcgen`.
- `python3 build.py -c -t` performs a clean rebuild and full test pass.
- `cmake -B build -DCMAKE_BUILD_TYPE=Debug -DMS_IPC_BUILD_EXAMPLES=ON` configures a manual debug build.
- `cmake --build build -j$(nproc)` builds all configured targets.
- `ctest --test-dir build --output-on-failure` runs C++ tests only.
- `python3 -m pytest tools/ipcgen/test/ -v` runs Python generator tests only.

## Coding Style & Naming Conventions
Match the existing style in `inc/` and `src/`: C++17, 4-space indentation, braces on their own line, and namespace-scoped definitions. Use `PascalCase` for types and test files (`ConnectionTest.cpp`), `camelCase` for functions and methods (`connectToServer`), and `kCamelCase` for internal constants (`kSocketTimeoutMs`). Keep headers minimal and prefer fixed-size, POD-friendly types to preserve the wire format assumptions documented in `README.md`.

## Testing Guidelines
Add or update Google Test coverage in `test/` for core runtime changes and in `examples/*/*Test.cpp` for generated-code behavior. Add pytest coverage under `tools/ipcgen/test/` for lexer, parser, or emitter changes. Name new Python tests `test_*.py` and keep test cases focused on one behavior. Run `python3 build.py -t` before opening a PR.

## Commit & Pull Request Guidelines
Recent history favors short, imperative commit subjects such as `Return first error from sendNotify, not last` and `Harden IPC layer against dead peers...`. Keep the first line concise and behavior-focused. PRs should describe the problem, summarize the fix, note any protocol or codegen impact, and list the commands you used to validate the change. Link related issues when applicable, and include screenshots only for example UI changes such as `examples/apps/exhaust-analyzer/`.
