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
        ];

        inputsFrom = [
          (config.packages.default)
        ];
      };
    };
}
