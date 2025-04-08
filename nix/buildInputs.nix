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
  #https://discourse.nixos.org/t/generate-and-install-a-desktop-file-along-with-an-executable/42744/2
  makeDesktopApp = { name, app, icon }: pkgs.stdenvNoCC.mkDerivation rec{
    inherit name;
    buildCommand = let
    inherit app;
    desktopEntry = pkgs.makeDesktopItem {
      name = name;
      desktopName = name;
      exec = "${app}/bin/${name} %f";
      icon = icon; #https://discourse.nixos.org/t/what-is-the-recommended-use-of-makedesktopitem-how-to-setup-the-icon-correctly/13388/7
      #terminal = true;
    };
  in ''
    mkdir -p $out/bin
    cp ${app}/bin/${name} $out/bin
    mkdir -p $out/share/applications
    cp ${desktopEntry}/share/applications/${name}.desktop $out/share/applications/${name}.desktop
  '';
    dontBuild = true;
  };

  inherit nativeBuildInputs;
 
  dotnetNativeBuildInputs = nativeBuildInputs ++ [ pkgs.dotnet-sdk pkgs.makeWrapper ];
  dotnetPostFixup = binary: ''
      # Ensure all dependencies are in PATH
      wrapProgram $out/bin/${binary} \
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
