{ 
  lib,
  pkgs,
  src,
  cmakeBuildType ? "Release"
}:
assert pkgs.lib.assertMsg (src.submodules == true)
  "Unable to build without submodules. Append '?submodules=1#' to the URL.";
let
  buildInputsFile = (import ./buildInputs.nix pkgs);
in
  pkgs.llvmPackages_18.stdenv.mkDerivation rec {
    pname = "BeeEngineEditor";
    version = "0.1.0";
  
    inherit src;

    nativeBuildInputs = buildInputsFile.nativeBuildInputs;
    buildInputs = buildInputsFile.buildInputs;

    cmakeFlags = [
      "-DCMAKE_BUILD_TYPE=${cmakeBuildType}"
      "-DBEE_USE_VCPKG=OFF"
    ];

    meta = with lib; {
      homepage = "https://github.com/nixvital/nix-based-cpp-starterkit";
      description = ''
      A template for Nix based C++ project setup.";
      '';
      licencse = licenses.mit;
      platforms = with platforms; linux ++ darwin;
      maintainers = [ maintainers.KyleKrein ];    
    };
  }
