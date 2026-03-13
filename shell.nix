# Nix development shell for ms-ipc
#
# Provides Qt5 for the exhaust-analyzer example. On non-NixOS systems,
# the Qt client must be run from within the nix-shell so that the nix
# dynamic linker and glibc are used at runtime.
#
# Usage:
#   nix-shell shell.nix
#
#   # Build everything:
#   python3 build.py -e
#
#   # Run server + client (both inside nix-shell):
#   ./build/examples/exhaust-analyzer/exhaust_server &
#   ./build/examples/exhaust-analyzer/exhaust_client
{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    qt5.qtbase
  ];

  nativeBuildInputs = with pkgs; [
    cmake
    gnumake
    gcc
    python3
    python3Packages.pytest
    pkg-config
  ];

  shellHook = ''
    export CMAKE_PREFIX_PATH="${pkgs.qt5.qtbase.dev}/lib/cmake''${CMAKE_PREFIX_PATH:+:$CMAKE_PREFIX_PATH}"
    export QT_PLUGIN_PATH="$(find ${pkgs.qt5.qtbase.bin}/lib -name plugins -path '*/qt-*' | head -1)"
  '';
}
