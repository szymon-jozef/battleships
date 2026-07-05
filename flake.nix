{
  inputs = {
    nixpkgs = {
      url = "github:nixos/nixpkgs/nixos-unstable";
    };
    flake-utils = {
      url = "github:numtide/flake-utils";
    };
  };
  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };

        build_inputs = with pkgs; [
          clang
          gcc
          cmake
          ninja
          boost
          catch2_3
          spdlog
          clang-tools
          clang-analyzer
        ];

        battleships = (
          with pkgs;
          stdenv.mkDerivation {
            pname = "battleships";
            version = "0.0.1";
            src = ./.;

            nativeBuildInputs = build_inputs;

            buildPhase = "make -j $NIX_BUILD_CORES";
            installPhase = ''
              mkdir -p $out/bin
              cp battleships $out/bin/
            '';
          }
        );
      in
      rec {
        defaultApp = flake-utils.lib.mkApp {
          drv = defaultPackage;
        };

        defaultPackage = battleships;

        devShell = pkgs.mkShell {

          packages = build_inputs;
        };
      }
    );
}
