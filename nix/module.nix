{ self, metadata_json, ... }:
{
  flake.nixosModules.default =
    {
      config,
      lib,
      pkgs,
      ...
    }:
    let
      cfg = config.programs.${metadata_json.name};

      inherit (lib)
        mkEnableOption
        mkOption
        mkIf
        ;

      inherit (pkgs.stdenv.hostPlatform) system;
    in
    {
      options.programs.${metadata_json.name} = {
        enable = mkEnableOption metadata_json.name;

        package = mkOption {
          default = self.packages.${system}.default;
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
