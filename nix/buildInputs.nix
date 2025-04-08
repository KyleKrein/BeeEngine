{ pkgs }:
let
 # nativeBuildInputs is usually what you want -- tools you need to run
  nativeBuildInputs = with pkgs.buildPackages; [
    cmake
    ninja
    python3
  ];
  wrapperPath = pkgs.lib.makeBinPath ([ pkgs.dotnet-sdk ]);
in
{
  inherit nativeBuildInputs;
 
  dotnetNativeBuildInputs = nativeBuildInputs ++ [ pkgs.dotnet-sdk pkgs.makeWrapper ];
  inherit wrapperPath;
  postFixup = ''
      # Ensure all dependencies are in PATH
      wrapProgram $out/bin/BeeEngineEditor \
        --prefix PATH : "${wrapperPath}"
  '';
  # libraries
  buildInputs = with pkgs; [
    freetype
    icu.dev
    boost
    libGL
    sdl3
    simdjson
    xorg.libX11
    xorg.libXext
    vulkan-headers
    vulkan-loader
    vulkan-validation-layers
  ];
}
