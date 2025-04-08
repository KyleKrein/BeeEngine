{
  description = "BeeEngine Flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
    #self.submodules = true; for nix 2.26
  };

  outputs = { self, nixpkgs, ... }@inputs: let
    inherit (nixpkgs) lib;
    eachSystem = lib.genAttrs [ "x86_64-linux" "aarch64-linux" ];
    pkgsFor = eachSystem (system:
      import nixpkgs {
        localSystem = system;
        overlays = with self.overlays; [
        ];
      });
    pkgsCrossFor = eachSystem (system: crossSystem:
      import nixpkgs {
        localSystem = system;
        inherit crossSystem;
        overlays = with self.overlays; [
        ];
      });
    src = self;
  in {
    overlays = eachSystem (system: 
      final: _prev: {
	beeengineeditor = self.packages.${system}.BeeEngineEditor;
	beelocalization = self.packages.${system}.BeeLocalization;
    });
    devShells = eachSystem (system: let 
	pkgs = pkgsFor.${system}; 
	buildInputsFile = (import ./nix/buildInputs.nix {inherit pkgs;});
      in { default = pkgsFor.${system}.mkShell.override { stdenv = pkgs.gcc14Stdenv; } rec {
      # Update the name to something that suites your project.
      name = "BeeEngine-shell";

      nativeBuildInputs = buildInputsFile.nativeBuildInputs ++ [pkgs.ccache];
      buildInputs = buildInputsFile.buildInputs ++ [pkgs.dotnet-sdk];
      NETHOST_LIB = (import ./nix/unofficial-nethost.nix {inherit pkgs; inherit (pkgs) lib;}).nethost-lib-path;
      VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
    };});

    packages = eachSystem(system: 
      let 
	pkgs = pkgsFor.${system}; 
	buildInputsFile = (import ./nix/buildInputs.nix {inherit pkgs;});
      in {
	default = self.packages.${system}.BeeEngineEditor;
	BeeEngineEditor = pkgs.callPackage ./nix/editor.nix { inherit src; inherit buildInputsFile; };
	BeeEngineEditor-Debug = pkgs.callPackage ./nix/editor.nix { inherit src; inherit buildInputsFile; cmakeBuildType = "Debug"; };
	BeeLocalization = pkgs.callPackage ./nix/localizationtool.nix { inherit src; inherit buildInputsFile; };
	BeeLocalization-Debug = pkgs.callPackage ./nix/localizationtool.nix { inherit src; inherit buildInputsFile; cmakeBuildType = "Debug"; };
  });
};
}
