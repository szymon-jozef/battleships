{
  perSystem =
    { pkgs, config, ... }:
    {
      devShells.default = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          clang
          clang-tools
          clang-analyzer
          valgrind
          cppcheck
        ];

        CMAKE_BUILD_TYPE = "Debug";

        inputsFrom = [
          (config.packages.default)
        ];

      };
    };
}
