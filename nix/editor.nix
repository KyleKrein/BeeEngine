{ 
  lib,
  pkgs,
  src,
  buildInputsFile,
  cmakeBuildType ? "Release"
}:
assert pkgs.lib.assertMsg (src.submodules == true)
  "Unable to build without submodules. Append '?submodules=1#' to the URL.";
let
  nethost-lib-path = (import ./unofficial-nethost.nix {inherit pkgs; inherit lib;}).nethost-lib-path;
in
  pkgs.gcc14Stdenv.mkDerivation rec {
    pname = "BeeEngineEditor";
    version = "1.0.0-alpha.1.2";
  
    inherit src;

    nativeBuildInputs = buildInputsFile.nativeBuildInputs;
    buildInputs = buildInputsFile.buildInputs;

    cmakeFlags = [
      "-DCMAKE_BUILD_TYPE=${cmakeBuildType}"
      "-DBEE_USE_VCPKG=OFF"
      "-DBEE_BUILD_TESTS=OFF"
      "-DBEE_BUILD_EDITOR=ON"
      "-DNETHOST_LIB=${nethost-lib-path}"
      "-DBEE_USE_SYSTEM_SDL3=ON"
      #"-DICU_INCLUDE_DIR=${pkgs.icu.dev}/include"
    ];

    meta = with lib; {
      homepage = "https://github.com/KyleKrein/BeeEngine";
      description = ''
      A GUI Editor app for building games, powered by BeeEngine
      '';
      licencse = licenses.mit;
      platforms = with platforms; linux ++ darwin;
      maintainers = [ maintainers.KyleKrein ];    
    };
  }
