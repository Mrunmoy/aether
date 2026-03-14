# Fallback for users without flakes enabled.
# Prefer: nix develop
#
# This reads the pinned nixpkgs revision from flake.lock so both
# nix-shell and nix develop use the same package set.
let
  lock = builtins.fromJSON (builtins.readFile ./flake.lock);
  nixpkgsInfo = lock.nodes.nixpkgs.locked;
  nixpkgs = fetchTarball {
    url = "https://github.com/NixOS/nixpkgs/archive/${nixpkgsInfo.rev}.tar.gz";
    sha256 = nixpkgsInfo.narHash;
  };
  pkgs = import nixpkgs {};
in
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    cmake
    gnumake
    gcc
    python3
    python3Packages.pytest
    pkg-config
    qt5.wrapQtAppsHook
  ];

  buildInputs = with pkgs; [
    qt5.qtbase
  ];

  shellHook = ''
    export QT_PLUGIN_PATH="${pkgs.qt5.qtbase.bin}/lib/qt-${pkgs.qt5.qtbase.version}/plugins"
  '';
}
