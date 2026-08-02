{ metadata_json, ... }:
{
  perSystem =
    { pkgs, ... }:
    let
      desktopItem = pkgs.makeDesktopItem {
        name = metadata_json.name;
        desktopName = metadata_json.name;
        exec = metadata_json.name;
        comment = metadata_json.description;
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
          pname = metadata_json.name;
          version = metadata_json.version;
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
            wrapProgram $out/bin/${metadata_json.name} --set BATTLESHIPS_ASSETS_DIR "$out/share/battleships/assets"
          '';

          meta = {
            description = metadata_json.description;
            homepage = metadata_json.homepage_url;
            license = lib.licenses.gpl3;
            platforms = lib.platforms.linux;
            downloadPage = metadata_json.release_url;
            mainProgram = metadata_json.name;
          };
        }
      );
    };
}
