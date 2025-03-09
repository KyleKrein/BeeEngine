{ pkgs, lib, ... } :
let 
  dotnet = pkgs.dotnet-sdk;
  version = dotnet.version;
  system = (if pkgs.system == "aarch64-linux" then "linux-arm64" else (if pkgs.system == "x86_64-linux" then "linux-x64" else "unknown"));
  libPath = "${system}/8.0.12/runtimes/${system}/native";
  finalPath = "${dotnet}/share/dotnet/packs/Microsoft.NETCore.App.Host.${libPath}";
in
{
  nethost-lib-path = "${finalPath}/libnethost.a";
}
