{
  perSystem =
    { pkgs, ... }:
    let
      desktopItem = pkgs.makeDesktopItem {
        name = "battleships";
        desktopName = "Battleships";
        exec = "battleships";
        comment = "Online game of battleships";
        categories = [
          "Game"
        ];
        terminal = false;
      };
    in
    {
      packages.default = (
        with pkgs;
        stdenv.mkDerivation {
          pname = "battleships";
          version = builtins.head (
            builtins.match ".*VERSION\ ([0-9.]+).*" (builtins.readFile ../CMakeLists.txt)
          );
          src = ./..;

          nativeBuildInputs = with pkgs; [
            ninja
            cmake
            copyDesktopItems
            makeWrapper
            glfw3
          ];

          buildInputs = with pkgs; [
            boost
            catch2_3
            spdlog
            raylib
          ];

          desktopItems = [ desktopItem ];

          postInstall = ''
            wrapProgram $out/bin/battleships --set BATTLESHIPS_ASSETS_DIR "$out/usr/share/battleships/assets"
          '';

        }
      );
    };
}
