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

      pname = metadata_json.name;
      version = metadata_json.version;
      src = ./..;

      cmakeFlags = [
        "-DCMAKE_GENERATOR=Ninja"
        "-DCMAKE_BUILD_TYPE=Release"
      ];

      nativeBuildInputs = with pkgs; [
        ninja
        cmake
        copyDesktopItems
        makeWrapper
      ];

      meta = with pkgs.lib; {
        description = metadata_json.description;
        homepage = metadata_json.homepage_url;
        license = licenses.gpl3;
        platforms = platforms.linux;
        downloadPage = metadata_json.release_url;
        mainProgram = metadata_json.name;
      };

    in
    {
      packages = {
        default = (
          pkgs.stdenv.mkDerivation {

            inherit
              pname
              version
              src
              cmakeFlags
              nativeBuildInputs
              meta
              ;

            desktopItems = [ desktopItem ];

            buildInputs = with pkgs; [
              boost
              spdlog
              raylib
              glfw3
            ];

            postInstall = ''
              wrapProgram $out/bin/${metadata_json.name} --set BATTLESHIPS_ASSETS_DIR "$out/share/battleships/assets"
            '';
          }
        );

        generic = (
          pkgs.stdenv.mkDerivation {
            inherit
              pname
              version
              src
              meta
              ;

            cmakeFlags = cmakeFlags ++ [
              (pkgs.lib.strings.cmakeBool "BUILD_SHARED_LIBS" false)
              "-DBoost_USE_STATIC_LIBS=ON"
            ];

            nativeBuildInputs = nativeBuildInputs ++ [ pkgs.patchelf ];

            buildInputs = with pkgs; [
              (boost.override { enableStatic = true; })
              (raylib.override { sharedLib = false; })

              (spdlog.overrideAttrs (old: {
                cmakeFlags = old.cmakeFlags or [ ] ++ [
                  (pkgs.lib.strings.cmakeBool "BUILD_SHARED_LIBS" false)
                ];
              }))

              (glfw3.overrideAttrs (old: {
                cmakeFlags = old.cmakeFlags or [ ] ++ [
                  (pkgs.lib.strings.cmakeBool "BUILD_SHARED_LIBS" false)
                ];
              }))
            ];

            postFixup = ''
              BIN="$out/bin/${metadata_json.name}"

              patchelf --set-interpreter /lib64/ld-linux-x86-64.so.2 "$BIN"

              patchelf --set-rpath "" "$BIN"
            '';
          }
        );
      };
    };
}
