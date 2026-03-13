{
  description = "ms-ipc — lightweight IPC framework with shared-memory transport";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShells.${system}.default = pkgs.mkShell {
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
      };
    };
}
