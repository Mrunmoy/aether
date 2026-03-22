"""
CLI entry point for ipcgen.

Usage:
    python3 tools/ipcgen example/DeviceMonitor.idl --outdir example/gen
"""

import argparse
import os

from .lexer import tokenize
from .parser import Parser
from .emitter import emit_server_h, emit_server_cpp, emit_client_h, emit_client_cpp, emit_types_h
from .types import fnv1a_32


def main():
    parser = argparse.ArgumentParser(description="aether IDL code generator")
    parser.add_argument("idl", help="Input .idl file")
    parser.add_argument("--outdir", required=True, help="Output directory")
    args = parser.parse_args()

    with open(args.idl) as f:
        text = f.read()

    tokens = tokenize(text)
    idl = Parser(tokens).parse()

    name = idl.service_name
    server_dir = os.path.join(args.outdir, "server")
    client_dir = os.path.join(args.outdir, "client")
    os.makedirs(server_dir, exist_ok=True)
    os.makedirs(client_dir, exist_ok=True)

    files = []

    if idl.enums or idl.structs:
        files.append((args.outdir, f"{name}Types.h", emit_types_h(idl)))

    files.extend([
        (server_dir, f"{name}.h",   emit_server_h(idl)),
        (server_dir, f"{name}.cpp", emit_server_cpp(idl)),
        (client_dir, f"{name}.h",   emit_client_h(idl)),
        (client_dir, f"{name}.cpp", emit_client_cpp(idl)),
    ])

    for directory, filename, content in files:
        path = os.path.join(directory, filename)
        with open(path, "w") as f:
            f.write(content)
        print(f"  wrote {path}")

    print(f"\nGenerated {len(files)} files for service '{name}' "
          f"(serviceId=0x{fnv1a_32(name):08x})")


if __name__ == "__main__":
    main()
