{
  description = "qitools — personal common tools, cross platform Qt programs";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      eachSystem = f: nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});

      ## CMake prefers Qt6 and falls back to Qt5.15, so the same expression
      ## builds against either set depending on which scope calls it.
      mkQitools = qtScope: qtScope.callPackage ./nix/package.nix { src = self; };
    in
    {
      overlays.default = final: _prev: {
        qitools = mkQitools final.qt6;
      };

      packages = eachSystem (pkgs: rec {
        qitools = mkQitools pkgs.qt6;
        qitools-qt5 = mkQitools pkgs.libsForQt5;
        default = qitools;
      });

      devShells = eachSystem (pkgs: {
        default = pkgs.mkShell {
          inputsFrom = [ (mkQitools pkgs.qt6) ];
          packages = with pkgs; [
            qt6.qttools
            gdb
          ];

          ## The in-tree build writes to bin/ and needs the ced submodule, which
          ## the packaged build substitutes with a fetchFromGitHub copy.
          shellHook = ''
            echo "qitools dev shell — cmake -S src -B build && cmake --build build"
            if [ ! -f src/codecconvert/ced/compact_enc_det/compact_enc_det.cc ]; then
              echo "warning: src/codecconvert/ced is empty, run: git submodule update --init"
            fi
          '';
        };
      });

      formatter = eachSystem (pkgs: pkgs.nixfmt);
    };
}
