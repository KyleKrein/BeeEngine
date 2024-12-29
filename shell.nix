{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = with pkgs.buildPackages; [ 
      nixd 
      git 
      cmake 
      ccache 
      ninja 
      python3 
      clang-tools 
      dotnetCorePackages.sdk_8_0_3xx
      #triton-llvm
      #llvmPackages_19.libcxxClang
      #llvmPackages_19.clangNoLibcxx
      clang_18
      curl
      zip
      unzip
      gnutar
      pkg-config
      autoconf
      automake
      autoconf-archive
      libGL
      xorg.libX11
      xorg.libXext
      vulkan-headers
      vulkan-loader
      vulkan-validation-layers
      lld
      lldb
      gdb
      wayland-utils
      wayland-scanner
      #vscode-fhs
      ];
     shellHook = 
     ''
  echo 'LD_LIBRARY_PATH=${pkgs.wayland}/lib:$LD_LIBRARY_PATH'
	code .
     '';
     NIXPKGS_ALLOW_UNFREE=1;
}
