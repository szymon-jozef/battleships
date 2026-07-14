{
  perSystem =
    { pkgs, ... }:
    {
      packages.default = (
        with pkgs;
        stdenv.mkDerivation {
          pname = "battleships";
          version = builtins.head (
            builtins.match ".*VERSION\ ([0-9.]+).*" (builtins.readFile ./CMakeLists.txt)
          );
          src = ./..;

          nativeBuildInputs = with pkgs; [
            clang
            cmake
          ];

          buildInputs = with pkgs; [
            boost
            catch2_3
            spdlog
            raylib
          ];

        }
      );
    };
}
