# Raylib Testing

This is my personal repo for messing with Raylib and learning how it works.

## Build for Desktop
- `make` in the root directory, and then run `out`.
- Alternatively, with `cmake` installed, you can also run a build script for your platform.
    - `./build_scripts/build.sh` or `build.bat` for Windows.

## Build for Browser
- Same as desktop, but add `web` as an argument.
    - `make web` or `build.sh web`.

## Requirements:

- raylib
- make or cmake
- gcc for desktop compilation
- emscripten for browser / web compilation
