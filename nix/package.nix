{
  perSystem =
    { pkgs, ... }:
    {
      packages.default = (
        with pkgs;
        stdenv.mkDerivation {
          pname = "battleships";
          version = "0.0.1";
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
