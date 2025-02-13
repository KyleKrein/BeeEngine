{ pkgs }:
let 
unofficial-nethost = pkgs.callPackage ./unofficial-nethost.nix {};
in
{
  # nativeBuildInputs is usually what you want -- tools you need to run
  nativeBuildInputs = with pkgs.buildPackages; [
    cmake
    ninja
    python3
    dotnetCorePackages.sdk_8_0_3xx
    llvmPackages.clang-tools
  ];
  buildInputs = with pkgs; [
    freetype
    icu
    boost
    libGL
    simdjson
    xorg.libX11
    xorg.libXext
    vulkan-headers
    vulkan-loader
    vulkan-validation-layers
    dotnetCorePackages.sdk_8_0_3xx
    unofficial-nethost
  ];
}
