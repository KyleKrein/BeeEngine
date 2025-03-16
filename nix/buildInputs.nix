{ pkgs }:
{
  # nativeBuildInputs is usually what you want -- tools you need to run
  nativeBuildInputs = with pkgs.buildPackages; [
    cmake
    ninja
    python3
    makeWrapper
    dotnet-sdk
  ];
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
