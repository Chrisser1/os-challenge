{
  description = "A C development environment for the 02159 OS Challenge";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      # The development shell is defined here
      devShells.${system}.default = pkgs.mkShell {
        # These packages will be available in the shell
        nativeBuildInputs = with pkgs; [
          gcc          # The specified C compiler
          gdb          # The GNU Debugger
          gnumake      # For running the Makefile
          pkg-config   # Manages library flags
          openssl.dev  # Provides the SHA256 library and headers
        ];
      };
    };
}
