# Raylib Testing

This is my personal repo for messing with Raylib and learning how it works, as
well as learning about very fundamental/basic game development.

Runs on Windows, Linux, MacOS, and web browsers. I don't have a Mac to test on
though, so I would appreciate anyone who can confirm if it works there.

## Build for Desktop
- Either run `build.sh` or `build.bat`, depending on your platform.
    - You can also use `make` to build the game.
- Run `game` or `game.exe` to start the game.

## Build for Browser
- Same as desktop, but add `web` as an argument:
    - Run `build.sh web` or `make web`.
- Run game with `emrun game.html`

## Requirements to build:

- raylib in your PATH
- GCC, Clang, or Visual Studio C compiler
- CMake for automated building
    - Alternatively, use make with gcc or clang
- emscripten for browser / web assembly compilation
