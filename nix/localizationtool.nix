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
icon = ../src/Engine/Assets/Textures/BeeEngineLogo.png;
tool = pkgs.gcc14Stdenv.mkDerivation rec {
    pname = "BeeLocalization";
    version = "1.0.0-alpha.1.2";
  
    inherit src;

    nativeBuildInputs = buildInputsFile.nativeBuildInputs;
    buildInputs = buildInputsFile.buildInputs;


    cmakeFlags = [
      "-DCMAKE_BUILD_TYPE=${cmakeBuildType}"
      "-DBEE_USE_VCPKG=OFF"
      "-DBEE_BUILD_TESTS=OFF"
      "-DBEE_BUILD_LOC_TOOL=OFF"
      "-DBEE_USE_SYSTEM_SDL3=ON"
    ];
    meta = with lib; {
      homepage = "https://github.com/KyleKrein/BeeEngine";
      description = ''
      A GUI App for easy management of localization files, powered by BeeEngine
      '';
      licencse = licenses.mit;
      platforms = with platforms; linux ++ darwin;
      maintainers = [ maintainers.KyleKrein ];    
    };
  };
in
  buildInputsFile.makeDesktopApp { app = tool; name = "BeeLocalization"; inherit icon; }
