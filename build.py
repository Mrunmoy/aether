#!/usr/bin/env python3
"""
Build script for aether.

Usage:
  python build.py              # build only
  python build.py -c           # clean build
  python build.py -t           # build + run tests
  python build.py -e           # build + examples
  python build.py -c -t -e     # clean build + tests + examples
"""

import argparse
import json
import os
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.abspath(__file__))
BUILD_DIR = os.path.join(ROOT, "build")


def run(cmd, **kwargs):
    print(f">>> {' '.join(cmd)}")
    result = subprocess.run(cmd, **kwargs)
    if result.returncode != 0:
        sys.exit(result.returncode)


def clean():
    if os.path.isdir(BUILD_DIR):
        print(f"Removing {BUILD_DIR}")
        shutil.rmtree(BUILD_DIR)


def configure(examples=False):
    os.makedirs(BUILD_DIR, exist_ok=True)
    cmd = ["cmake", "-B", BUILD_DIR, "-DCMAKE_BUILD_TYPE=Release"]
    cmd.append(f"-DAETHER_BUILD_EXAMPLES={'ON' if examples else 'OFF'}")
    run(cmd, cwd=ROOT)


def build(examples=False):
    configure(examples=examples)
    run(["cmake", "--build", BUILD_DIR, f"-j{os.cpu_count()}"], cwd=ROOT)


def test():
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
    run([sys.executable, "-m", "pytest", "tools/ipcgen/test/", "-v"], cwd=ROOT)


def main():
    parser = argparse.ArgumentParser(description="Build aether")
    parser.add_argument("-c", "--clean", action="store_true", help="clean before building")
    parser.add_argument("-t", "--test", action="store_true", help="run tests after building")
    parser.add_argument("-e", "--examples", action="store_true", help="build examples")
    args = parser.parse_args()

    if args.clean:
        clean()

    # When testing, always build examples (codegen tests live there).
    build(examples=args.examples or args.test)

    if args.test:
        test()


if __name__ == "__main__":
    main()
