{ pkgs, lib } :
let 
version = pkgs.dotnet-sdk.version;
system = (if pkgs.system == "aarch64-linux" then "linux-arm64" else (if pkgs.system == "x86_64-linux" then "linux-x64" else
"unknown"));
libPath = "${system}/8.0.12/runtimes/${system}/native";
finalPath = "${pkgs.dotnet-sdk}/share/dotnet/packs/Microsoft.NETCore.App.Host.${libPath}";
cmakefile = pkgs.writeText "CMakeLists.txt" ''
cmake_minimum_required(VERSION 3.25)
project(unofficial-nethost)
include_directories($\{CMAKE_CURRENT_SOURCE_DIR})

if(BUILD_SHARED_LIBS)
    set(NETHOST_LIB "libnethost.so")
else()
    set(NETHOST_LIB "libnethost.a")
endif()

add_library(nethost INTERFACE)
target_link_libraries(nethost INTERFACE $\{NETHOST_LIB})
target_include_directories(nethost INTERFACE $<INSTALL_INTERFACE:include>)

set(NETHOST_HEADERS nethost.h ../coreclr_delegates.h ../hostfxr.h)
install(FILES $\{NETHOST_HEADERS} DESTINATION include)
install(TARGETS nethost EXPORT unofficial-nethost-config
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(EXPORT unofficial-nethost-config
    DESTINATION share/unofficial-nethost
    FILE unofficial-nethost-config.cmake
    NAMESPACE unofficial::nethost::
)
'';
buildSrc = {mkDerivation, finalPath, cmakefile}: mkDerivation {
      name = "unofficial-nethost-cmake";
      inherit version;
      src = finalPath;
      phases = [ "installPhase" ];
      installPhase = ''
mkdir -p $out
cp -r $src/* $out/
cp ${cmakefile} $out/CMakeLists.txt
      '';
    };
src = pkgs.callPackage buildSrc {inherit (pkgs.stdenv) mkDerivation; inherit finalPath; inherit cmakefile;};
in
pkgs.llvmPackages_18.stdenv.mkDerivation rec {
    pname = "unofficial-nethost";
    inherit version;
  
    inherit src;

    nativeBuildInputs = with pkgs; [ cmake ninja ];

    cmakeFlags = [
      "-DBUILD_SHARED_LIBS=OFF"
    ];

    meta = with lib; {
      homepage = "https://github.com/dotnet/runtime";
      description = ''
      Nethost package, that mimics unofficial-nethost from vcpkg";
      '';
      licencse = licenses.mit;
      platforms = with platforms; linux ++ darwin;
      maintainers = [ maintainers.KyleKrein ];    
    };
  }
