{ self, ... }:
{
  flake.nixosModules.default =
    {
      config,
      lib,
      pkgs,
      ...
    }:
    let
      cfg = config.programs.battleships;

      inherit (lib)
        mkEnableOption
        mkOption
        mkIf
        ;
    in
    {
      options.programs.battleships = {
        enable = mkEnableOption "Battleships";

        package = mkOption {
          default = self.packages.${pkgs.system}.default;
          description = "Package to use";
          type = lib.types.package;
        };
      };

      config = mkIf cfg.enable {
        environment.systemPackages = [ cfg.package ];
        networking.firewall.allowedTCPPorts = [ 6767 ];
      };
    };
}
