# TODO: SDK Packaging & Distribution

Distribute aether as a closed-source SDK — private source repo, public
SDK repo with pre-built binaries and headers only.

## 1. CMake install targets

Add `install()` commands to CMakeLists.txt so `cmake --install` produces
a clean SDK layout:

```
sdk/
├── lib/libaether.a
├── lib/libms-runloop.a
└── include/ (6 aether headers + RunLoop.h + spsc/RingBuffer.h)
```

## 2. Bundle ipcgen as standalone binary

Use PyInstaller (or Nuitka) to package the Python ipcgen tool into a
single executable that runs without a Python installation:

```bash
pyinstaller --onefile tools/ipcgen/__main__.py -n ipcgen
# produces: dist/ipcgen
```

## 3. Add --package flag to build.py

Extend build.py with a `--package` (or `-p`) flag that:
1. Builds the C++ libraries (release mode)
2. Runs cmake --install to collect libs + headers
3. Bundles ipcgen with PyInstaller
4. Assembles the final SDK directory:

```
aether-sdk/
├── bin/ipcgen
├── lib/libaether.a, libms-runloop.a
├── include/ (public headers only)
├── example/ (IDL + server/client example + CMakeLists.txt)
├── README.md
└── LICENSE
```

## 4. Create aether-sdk public repo

Public GitHub repo (`Mrunmoy/aether-sdk`) containing only SDK artifacts:
- Pre-built libraries and headers (no .cpp source files)
- ipcgen binary (no Python source)
- README with usage instructions and CMake integration example
- Closed-source license
- Working example that builds against the SDK

## 5. GitHub Actions CI (auto-publish)

Set up CI on the **private** aether repo that triggers on version tags:

```
on push tag v* → build → test → package SDK → upload to aether-sdk releases
```

Workflow:
1. Tag a release on aether (e.g., `git tag v1.0.0`)
2. CI builds and runs all 217 tests
3. CI runs the package script to assemble the SDK
4. CI uses a Personal Access Token to create a GitHub Release on
   the public aether-sdk repo with the SDK tarball attached

Customers download from: `github.com/Mrunmoy/aether-sdk/releases`

## 6. Change license

Replace MIT license with a closed-source license before any public
release. The SDK headers and binaries need appropriate usage terms.
