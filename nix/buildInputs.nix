pkgs :
{
  # nativeBuildInputs is usually what you want -- tools you need to run
  nativeBuildInputs = with pkgs.buildPackages; [
    nixd
    git
    cmake
    ccache
    ninja
    python3
    llvmPackages_18.clang-tools
    dotnetCorePackages.sdk_8_0_3xx
    #triton-llvm
    #llvmPackages_19.libcxxClang
    #llvmPackages_19.clangNoLibcxx
    llvmPackages_18.libllvm
    llvmPackages_18.clangUseLLVM
    curl
    zip
    unzip
    gnutar
    pkg-config
    autoconf
    automake
    autoconf-archive
    gdb
    wayland-utils
    wayland-scanner
    libGL
    xorg.libX11
    xorg.libXext
    vulkan-headers
    vulkan-loader
    vulkan-validation-layers
    lld_18
    lldb_18
    #vscode-fhs
  ];
  buildInputs = with pkgs; [];
}
