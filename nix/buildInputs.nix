{ pkgs }:
let 
in
{
  # nativeBuildInputs is usually what you want -- tools you need to run
  nativeBuildInputs = with pkgs.buildPackages; [
    cmake
    ninja
    python3
    dotnetCorePackages.sdk_8_0_3xx
    #llvmPackages.clang-tools
  ];
  buildInputs = with pkgs; [
    freetype
    icu.dev
    boost
    libGL
    sdl3
    simdjson
   # (simdjson.override {stdenv = llvmPackages_19.libcxxStdenv;})
    xorg.libX11
    xorg.libXext
    vulkan-headers
    vulkan-loader
    vulkan-validation-layers
    dotnetCorePackages.sdk_8_0_3xx
  ];
}
