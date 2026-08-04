{
  perSystem =
    {
      pkgs,
      config,
      ...
    }:
    {
      devenv.shells.default = {
        packages = with pkgs; [
          clang
          clang-tools
          clang-analyzer
          valgrind
          pre-commit
          cppcheck
        ];

        env = {
          CMAKE_BUILD_TYPE = "Debug";
          SPDLOG_LEVEL = "debug";
        };

        scripts = {
          configure.exec = ''cmake -B build -G Ninja -D"isDev=True"'';
          build.exec = "cmake --build build";
        };

        git-hooks.hooks = {
          "cppcheck" = {
            enable = true;
            name = "Cppcheck";

            entry = "cppcheck --enable=warning,style,performance,portability --error-exitcode=1 --project=build/compile_commands.json";
            pass_filenames = false;
          };
        };

        inputsFrom = [
          (config.packages.default)
        ];

      };
    };
}
