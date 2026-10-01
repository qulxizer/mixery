{
  description = "RP2040 pico-sdk dev environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            usbutils
            python3
            python3Packages.pip
            hidapi
            cmake
            ninja
            gcc-arm-embedded
            python3
            picotool
            pico-sdk
            clang-tools
          ];

          shellHook = ''
            export LD_LIBRARY_PATH="${pkgs.hidapi}/lib:$LD_LIBRARY_PATH"
            export PICO_SDK_PATH="${pkgs.pico-sdk}/lib/pico-sdk"
            echo "RP2040 dev environment loaded. PICO_SDK_PATH set."
            if [ -d "venv" ]; then
              source venv/bin/activate
            fi
          '';
        };
      }
    );
}
