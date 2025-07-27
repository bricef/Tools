# shell.nix
let
  pkgs = import <nixpkgs> {};
in
  pkgs.mkShell {
    packages = [
      pkgs.wayland
      pkgs.wayland-scanner
      pkgs.libGL
      pkgs.raylib
      pkgs.xorg.libX11
      pkgs.xorg.libXcursor
      pkgs.xorg.libXrandr
      pkgs.xorg.libXinerama
      pkgs.xorg.libXext
      pkgs.xorg.libXcomposite
      pkgs.xorg.libXfixes
      pkgs.xorg.libXrender
      pkgs.xorg.libXi
      pkgs.libxkbcommon
    ];
    shellHook = ''export LD_LIBRARY_PATH=${pkgs.wayland}/lib:$LD_LIBRARY_PATH'';
  } 
  
