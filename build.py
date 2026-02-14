#!/usr/bin/env python3
"""
Build script for ms-ipc.

Usage:
  python build.py              # build only
  python build.py -c           # clean build
  python build.py -t           # build + run tests
  python build.py -c -t        # clean build + tests
"""

import argparse
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


def configure():
    os.makedirs(BUILD_DIR, exist_ok=True)
    run(["cmake", "-B", BUILD_DIR, "-DCMAKE_BUILD_TYPE=Release"], cwd=ROOT)


def build():
    configure()
    run(["cmake", "--build", BUILD_DIR, f"-j{os.cpu_count()}"], cwd=ROOT)


def test():
    run(["ctest", "--test-dir", BUILD_DIR, "--output-on-failure"], cwd=ROOT)


def main():
    parser = argparse.ArgumentParser(description="Build ms-ipc")
    parser.add_argument("-c", "--clean", action="store_true", help="clean before building")
    parser.add_argument("-t", "--test", action="store_true", help="run tests after building")
    args = parser.parse_args()

    if args.clean:
        clean()

    build()

    if args.test:
        test()


if __name__ == "__main__":
    main()
