# Raylib Testing

This is my personal repo for messing with Raylib and learning how it works.

Runs on Windows, Linux, MacOS, and web browsers. I don't have a Mac to test on though, so I would appreciate anyone who can confirm if it works there.

## Build for Desktop
- Run `make`, and then run the `game` executable. (no Mac support)
- **Alternatively**, with `cmake` installed, you can run a build script for your platform:
    - Run `build.bat` for Windows
    - Run `build.sh` for Linux/Mac
    - This method also automatically fetches raylib for compilation.

## Build for Browser
- Same as desktop, but add `web` as an argument:
    - Run `make web` or `build.sh web`.
- Run game with `emrun game.html`

## Requirements:

- raylib
- make and gcc or clang
    - alternatively, cmake
- emscripten for browser / web compilation
