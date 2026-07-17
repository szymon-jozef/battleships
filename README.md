Classic game of battleships.

Assets made by my friend [real-morbius](https://github.com/Real-Morbius)

This is a rewrite of [this project](https://github.com/szymon-jozef/battleship-pygame-lan), but in cpp.

Currently WIP.

On first release this readme will be updated.

# Dependencies
- [boost](https://www.boost.org/)
- [spdlog](https://github.com/gabime/spdlog)
- [raylib](https://www.raylib.com/)
- [glfw3](https://www.glfw.org/)
- [catch2 (for tests)](https://github.com/catchorg/Catch2)

# Installation
## Manual compilation
### Linux/MacOS(probably?)
**Make sure you have dependencies installed!**

First run:
```bash
cmake -B build -G Ninja
```
*or (if you also wish to compile tests)*
```bash
cmake -B build -G Ninja -D"isDev=True"
```
Then just:
```bash
cmake --build build
```

To run the app just
```bash
build/battleships
```
Make sure you're in the project root when you do that!

### Windows
Make sure you have [vcpkgs](https://github.com/microsoft/vcpkg) installed or manager dependencies on your own, but then you're on your own.

Create build directory (as above), but add a toolchain file location.
```bash
cmake -D"CMAKE_TOOLCHAIN_FILE=<path to your vcpkg.cmake>" -B build -G Ninja
```
This should download all the necessary dependencies. Then just compile:
```bash
cmake --build build
```
and run with:
```bash
build/battleships.exe
```

## NixOS
This repo provides a flake, which exposes a package and a module. The module opens port 6767 in the firewall and creates a .desktop entry.

To use it add:
```nix
battleships.url = "github:szymon-jozef/battleships";
```
to your `flake.nix` inputs. Then somewhere in your configuration:
```nix
imports = [
    inputs.battleship.nixosModules.default
];

programs.battleships.enable = true;
```

## Arch Linux
There is a `PKGBUILD` in the repo. Just clone it and run:
```bash
makepkg -si
```

