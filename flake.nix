{
  description = "BeeEngine Flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
    utils.url = "github:numtide/flake-utils";
    #self.submodules = true; for nix 2.26
  };

  outputs = { self, nixpkgs, ... }@inputs: inputs.utils.lib.eachSystem [
    # Add the system/architecture you would like to support here. Note that not
    # all packages in the official nixpkgs support all platforms.
    "x86_64-linux" "aarch64-linux" "x86_64-darwin"
  ] (system: let
    pkgs = import nixpkgs {
      inherit system;
    };
    buildInputsFile = (import ./nix/buildInputs.nix {inherit pkgs;});
    src = self;
  in {
    devShells.default = pkgs.mkShell.override { stdenv = pkgs.gcc14Stdenv; } rec {
      # Update the name to something that suites your project.
      name = "BeeEngine";

      nativeBuildInputs = buildInputsFile.nativeBuildInputs ++ [pkgs.ccache];
      buildInputs = buildInputsFile.buildInputs ++ [pkgs.dotnet-sdk];
      NETHOST_LIB = (import ./nix/unofficial-nethost.nix {inherit pkgs; inherit (pkgs) lib;}).nethost-lib-path;
      VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
    };

    packages.default = self.packages.${system}.BeeEngineEditor;
    packages.BeeEngineEditor = pkgs.callPackage ./nix/editor.nix { inherit src; inherit buildInputsFile; };
    packages.BeeEngineEditor-Debug = pkgs.callPackage ./nix/editor.nix { inherit src; inherit buildInputsFile; cmakeBuildType = "Debug"; };
    packages.BeeLocalization = pkgs.callPackage ./nix/localizationtool.nix { inherit src; inherit buildInputsFile; };
    packages.BeeLocalization-Debug = pkgs.callPackage ./nix/localizationtool.nix { inherit src; inherit buildInputsFile; cmakeBuildType = "Debug"; };
  });
}
