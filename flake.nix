{
  description = "BeeEngine Flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, ... }@inputs: inputs.utils.lib.eachSystem [
    # Add the system/architecture you would like to support here. Note that not
    # all packages in the official nixpkgs support all platforms.
    "x86_64-linux" "i686-linux" "aarch64-linux" "x86_64-darwin"
  ] (system: let
    pkgs = import nixpkgs {
      inherit system;
    };
    buildInputsFile = (import ./nix/buildInputs.nix pkgs);
    src = self;
  in {
    devShells.default = pkgs.mkShell rec {
      # Update the name to something that suites your project.
      name = "BeeEngine";

      nativeBuildInputs = buildInputsFile.nativeBuildInputs;
      buildInputs = buildInputsFile.buildInputs;
    };

    packages.default = self.packages.${system}.BeeEngineEditor;
    packages.BeeEngineEditor = pkgs.callPackage ./nix/editor.nix { inherit src; };
  });
}
