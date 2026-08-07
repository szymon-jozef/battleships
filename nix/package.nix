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
      src = pkgs.lib.cleanSource ./..;

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
              glaze
            ];

            postInstall = ''
              wrapProgram $out/bin/${metadata_json.name} --set BATTLESHIPS_ASSETS_DIR "$out/share/battleships/assets"
            '';
          }
        );

        # Generic release to use in github actions
        # You probably don't want to use it anywhere elsewhere
        # It build this game statically
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
              (fmt.override { enableShared = false; })
              (spdlog.override { staticBuild = true; })

              glfw3 # we build this dynamically
              glaze # header only
            ];

            postFixup = ''
              BIN="$out/bin/${metadata_json.name}"

              patchelf --set-interpreter /lib64/ld-linux-x86-64.so.2 "$BIN"
              patchelf --set-rpath "" "$BIN"

              # we do this as this target is for github actions
              cp $out/bin/battleships $out/
              cp $out/share/battleships/assets $out/ -r

              rm $out/{bin,share} -rf

              # we package this app
              cd $out
              tar czf battleships-${metadata_json.version}-linux.tar.gz ./battleships .//assets
              rm $out/assets battleships -rf
            '';
          }
        );
      };
    };
}
