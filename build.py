#!/usr/bin/env python3
"""
Build script for aether.

Usage:
  python build.py              # build only
  python build.py -c           # clean build
  python build.py -t           # build + run tests
  python build.py -e           # build + examples
  python build.py -p           # build + package SDK tarball
  python build.py -c -t -e     # clean build + tests + examples
"""

import argparse
import glob as globmod
import json
import os
import platform
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.abspath(__file__))
BUILD_DIR = os.path.join(ROOT, "build")


def run(cmd, **kwargs):
    print(f">>> {' '.join(cmd)}")
    result = subprocess.run(cmd, **kwargs)
    if result.returncode != 0:
        sys.exit(result.returncode)


def get_version():
    """Read project version from CMakeLists.txt."""
    ver = os.environ.get("AETHER_SDK_VERSION")
    if ver:
        return ver
    with open(os.path.join(ROOT, "CMakeLists.txt")) as f:
        for line in f:
            if "VERSION" in line and "project(" not in line:
                # Match: VERSION 1.1.0
                parts = line.strip().split()
                for i, p in enumerate(parts):
                    if p == "VERSION" and i + 1 < len(parts):
                        return parts[i + 1]
    return "0.0.0"


def clean():
    if os.path.isdir(BUILD_DIR):
        print(f"Removing {BUILD_DIR}")
        shutil.rmtree(BUILD_DIR)


def configure(examples=False, sanitizers=""):
    os.makedirs(BUILD_DIR, exist_ok=True)
    cmd = ["cmake", "-B", BUILD_DIR, "-DCMAKE_BUILD_TYPE=Release"]
    cmd.append(f"-DAETHER_BUILD_EXAMPLES={'ON' if examples else 'OFF'}")
    if sanitizers:
        cmd.append(f"-DAETHER_SANITIZERS={sanitizers}")
    run(cmd, cwd=ROOT)


def build(examples=False, sanitizers=""):
    configure(examples=examples, sanitizers=sanitizers)
    run(["cmake", "--build", BUILD_DIR, f"-j{os.cpu_count()}"], cwd=ROOT)


def executable_path(*parts):
    suffix = ".exe" if os.name == "nt" else ""
    return os.path.join(*parts) + suffix


def run_sanitized_tests():
    env = os.environ.copy()
    env.setdefault("PYTEST_DISABLE_PLUGIN_AUTOLOAD", "1")
    env.setdefault("ASAN_OPTIONS", "detect_leaks=1:halt_on_error=1")
    env.setdefault("UBSAN_OPTIONS", "print_stacktrace=1:halt_on_error=1")
    env.setdefault("TSAN_OPTIONS", "halt_on_error=1:history_size=7")

    # GoogleTest discovery via ctest is flaky under sanitized runs; invoke the
    # binaries directly so the sanitizer jobs exercise the same logic reliably.
    run([executable_path(BUILD_DIR, "test", "ipc_tests"), "--gtest_brief=1"], cwd=ROOT, env=env)

    for name in ["codegen_server_tests", "codegen_client_tests"]:
        exe = executable_path(BUILD_DIR, "examples", "echo", name)
        if os.path.isfile(exe):
            run([exe, "--gtest_brief=1"], cwd=ROOT, env=env)

    run([sys.executable, "-m", "pytest", "tools/ipcgen/test/", "-v"], cwd=ROOT, env=env)


def test(sanitizers=""):
    if sanitizers:
        run_sanitized_tests()
        return

    has_cpp_tests = False
    try:
        result = subprocess.run(
            ["ctest", "--test-dir", BUILD_DIR, "--show-only=json-v1"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            check=True,
        )
        data = json.loads(result.stdout)
        has_cpp_tests = bool(data.get("tests") or [])
    except (subprocess.CalledProcessError, json.JSONDecodeError):
        result = subprocess.run(
            ["ctest", "--test-dir", BUILD_DIR, "-N"],
            cwd=ROOT,
            capture_output=True,
            text=True,
        )
        if result.returncode != 0:
            print("Failed to discover C++ tests. Ensure CTest is installed and tests are configured.",
                  file=sys.stderr)
            sys.exit(1)
        for line in result.stdout.splitlines():
            if "Total Tests:" in line:
                try:
                    has_cpp_tests = int(line.split("Total Tests:", 1)[1].strip()) > 0
                except ValueError:
                    has_cpp_tests = False
                break

    if not has_cpp_tests:
        print("No C++ tests were discovered. Ensure test submodules are initialized.", file=sys.stderr)
        sys.exit(1)

    # C++ tests (core + codegen examples)
    run(["ctest", "--test-dir", BUILD_DIR, "--output-on-failure"], cwd=ROOT)
    # Python tests for the code generator
    env = os.environ.copy()
    env.setdefault("PYTEST_DISABLE_PLUGIN_AUTOLOAD", "1")
    run([sys.executable, "-m", "pytest", "tools/ipcgen/test/", "-v"], cwd=ROOT, env=env)


def package():
    """Assemble the SDK tarball."""
    version = get_version()
    arch = platform.machine()  # e.g. x86_64, aarch64
    sdk_name = f"aether-sdk-{version}-linux-{arch}"
    staging = os.path.join(ROOT, sdk_name)

    # Clean previous staging
    if os.path.isdir(staging):
        shutil.rmtree(staging)

    print(f"Packaging {sdk_name}...")

    # Install to staging dir
    run(["cmake", "--install", BUILD_DIR, "--prefix", staging], cwd=ROOT)

    # Create fat static archive (merge deps into libaether.a)
    with tempfile.TemporaryDirectory() as tmpdir:
        objs = []
        for lib in [
            os.path.join(BUILD_DIR, "libaether.a"),
            os.path.join(BUILD_DIR, "deps", "ouroboros", "libouroboros.a"),
            os.path.join(BUILD_DIR, "deps", "vortex", "libvortex.a"),
        ]:
            if os.path.isfile(lib):
                lib_name = os.path.splitext(os.path.basename(lib))[0]
                extract_dir = os.path.join(tmpdir, lib_name)
                os.makedirs(extract_dir, exist_ok=True)
                run(["ar", "x", lib], cwd=extract_dir)
                objs.extend(globmod.glob(os.path.join(extract_dir, "*.o")))
        fat_lib = os.path.join(staging, "lib", "libaether.a")
        if objs:
            run(["ar", "rcs", fat_lib] + objs)
            run(["ranlib", fat_lib])

    # Clean out GTest/GMock artifacts that leaked from vendor/
    for d in ["gmock", "gtest"]:
        p = os.path.join(staging, "include", d)
        if os.path.isdir(p):
            shutil.rmtree(p)
    for pattern_dir in [
        os.path.join(staging, "lib", "cmake", "GTest"),
    ]:
        if os.path.isdir(pattern_dir):
            shutil.rmtree(pattern_dir)
    for f in os.listdir(os.path.join(staging, "lib")):
        if f.startswith("libgtest") or f.startswith("libgmock"):
            os.remove(os.path.join(staging, "lib", f))
    for f in os.listdir(os.path.join(staging, "lib", "pkgconfig")):
        if "gtest" in f or "gmock" in f:
            os.remove(os.path.join(staging, "lib", "pkgconfig", f))

    # Copy SDK example
    example_src = os.path.join(ROOT, "examples", "c-echo")
    example_dst = os.path.join(staging, "example")
    os.makedirs(example_dst, exist_ok=True)
    for f in ["echo_server.c", "echo_client.c"]:
        shutil.copy2(os.path.join(example_src, f), example_dst)

    # Create standalone example CMakeLists for SDK consumers
    with open(os.path.join(example_dst, "CMakeLists.txt"), "w") as f:
        f.write("""\
cmake_minimum_required(VERSION 3.14)
project(aether_example C)

# Option 1: find_package (if SDK is installed or CMAKE_PREFIX_PATH is set)
# find_package(aether REQUIRED)

# Option 2: direct path (simpler for quick tests)
set(AETHER_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." CACHE PATH "Path to Aether SDK")

add_executable(echo_server echo_server.c)
target_include_directories(echo_server PRIVATE ${AETHER_SDK_DIR}/include)
target_link_libraries(echo_server ${AETHER_SDK_DIR}/lib/libaether.a stdc++ pthread)

add_executable(echo_client echo_client.c)
target_include_directories(echo_client PRIVATE ${AETHER_SDK_DIR}/include)
target_link_libraries(echo_client ${AETHER_SDK_DIR}/lib/libaether.a stdc++ pthread)
""")

    # Copy README and LICENSE
    for f in ["LICENSE"]:
        src = os.path.join(ROOT, f)
        if os.path.isfile(src):
            shutil.copy2(src, staging)

    # Create SDK README
    with open(os.path.join(staging, "README.md"), "w") as f:
        f.write(f"""\
# Aether IPC SDK v{version}

Pre-built IPC library for Linux ({arch}).

The SDK exposes a stable **C API** (`aether_ipc.h`) with opaque handles.
Link against the fat static archive — all dependencies are bundled.

## Quick Start

```bash
cd example/
cmake -B build -DAETHER_SDK_DIR=$(pwd)/..
cmake --build build
./build/echo_server &
sleep 0.2
./build/echo_client
```

## Contents

| Path | Description |
|------|-------------|
| `include/aether_ipc.h` | C API header (only public header) |
| `lib/libaether.a` | Fat static library (all deps bundled) |
| `lib/libaether.so*` | Shared library |
| `lib/pkgconfig/aether.pc` | pkg-config support |
| `lib/cmake/aether/` | CMake find_package() support |
| `example/` | Echo server + client in pure C |

## Linking

### CMake (recommended)
```cmake
find_package(aether REQUIRED)
target_link_libraries(my_app PRIVATE aether::aether)
```

### pkg-config
```bash
cc -o my_app my_app.c $(pkg-config --cflags --libs aether) -lstdc++
```

### Manual
```bash
cc -o my_app my_app.c -I<sdk>/include <sdk>/lib/libaether.a -lstdc++ -lpthread
```
""")

    # Create tarball
    tarball = f"{sdk_name}.tar.gz"
    run(["tar", "-czf", tarball, sdk_name], cwd=ROOT)

    # Clean staging dir
    shutil.rmtree(staging)

    print(f"\n✓ SDK packaged: {tarball}")
    # Show contents
    run(["tar", "-tzf", tarball], cwd=ROOT)


def main():
    parser = argparse.ArgumentParser(description="Build aether")
    parser.add_argument("-c", "--clean", action="store_true", help="clean before building")
    parser.add_argument("-t", "--test", action="store_true", help="run tests after building")
    parser.add_argument("-e", "--examples", action="store_true", help="build examples")
    parser.add_argument("-p", "--package", action="store_true", help="package SDK tarball")
    parser.add_argument("--sanitizers", default="",
                        help="comma-separated GCC/Clang sanitizers, e.g. address,undefined or thread")
    args = parser.parse_args()

    if args.clean:
        clean()

    # When testing or packaging, always build examples.
    build(examples=args.examples or args.test or args.package, sanitizers=args.sanitizers)

    if args.test:
        test(sanitizers=args.sanitizers)

    if args.package:
        package()


if __name__ == "__main__":
    main()
