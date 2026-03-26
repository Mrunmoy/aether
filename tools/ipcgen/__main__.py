"""
CLI entry point for ipcgen.

Usage:
    python3 tools/ipcgen example/DeviceMonitor.idl --outdir example/gen
    python3 tools/ipcgen example/DeviceMonitor.idl --outdir example/gen --backend c_api
"""

import argparse
import os

from .lexer import tokenize
from .parser import Parser
from .emitter import emit_server_h, emit_server_cpp, emit_client_h, emit_client_cpp, emit_types_h
from .c_api_emitter import (emit_c_api_server_h, emit_c_api_server_cpp,
                            emit_c_api_client_h, emit_c_api_client_cpp)
from .python_emitter import emit_python_client
from .aether_lite_emitter import emit_aether_lite_h, emit_aether_lite_c
from .types import fnv1a_32


def main():
    parser = argparse.ArgumentParser(description="aether IDL code generator")
    parser.add_argument("idl", help="Input .idl file")
    parser.add_argument("--outdir", required=True, help="Output directory")
    parser.add_argument("--backend", choices=["cpp", "c_api", "python", "aether_lite"],
                        default="cpp",
                        help="Code generation backend: 'cpp' (default) uses "
                             "ServiceBase/ClientBase (source build); 'c_api' "
                             "generates wrappers using aether_ipc.h (SDK); "
                             "'python' generates a typed Python client module; "
                             "'aether_lite' generates C99 dispatch tables for "
                             "bare-metal MCU firmware")
    args = parser.parse_args()

    with open(args.idl) as f:
        text = f.read()

    tokens = tokenize(text)
    idl = Parser(tokens).parse()

    name = idl.service_name

    if args.backend == "aether_lite":
        # Flat output: {ServiceName}.h and {ServiceName}.c in outdir.
        os.makedirs(args.outdir, exist_ok=True)

        files = [
            (args.outdir, f"{name}.h", emit_aether_lite_h(idl)),
            (args.outdir, f"{name}.c", emit_aether_lite_c(idl)),
        ]
    else:
        server_dir = os.path.join(args.outdir, "server")
        client_dir = os.path.join(args.outdir, "client")
        os.makedirs(server_dir, exist_ok=True)
        os.makedirs(client_dir, exist_ok=True)

        files = []

        if args.backend == "python":
            files.append((client_dir, f"{name}.py", emit_python_client(idl)))
        else:
            if idl.enums or idl.structs:
                files.append((args.outdir, f"{name}Types.h", emit_types_h(idl)))

            if args.backend == "c_api":
                files.extend([
                    (server_dir, f"{name}.h",   emit_c_api_server_h(idl)),
                    (server_dir, f"{name}.cpp", emit_c_api_server_cpp(idl)),
                    (client_dir, f"{name}.h",   emit_c_api_client_h(idl)),
                    (client_dir, f"{name}.cpp", emit_c_api_client_cpp(idl)),
                ])
            else:
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

    backend_label = f", backend={args.backend}" if args.backend != "cpp" else ""
    print(f"\nGenerated {len(files)} files for service '{name}' "
          f"(serviceId=0x{fnv1a_32(name):08x}{backend_label})")


if __name__ == "__main__":
    main()
