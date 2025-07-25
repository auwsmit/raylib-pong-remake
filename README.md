# Raylib Testing

This is my personal repo for messing with Raylib and learning how it works.

## Build for Desktop
- Run `make` in the root directory
- Alternatively, with `cmake` installed, you can run a build script for your platform:
    - `./build.sh` or `build.bat` for Windows.
- Run with the `out` executable.

## Build for Browser
- Same as desktop, but add `web` as an argument:
    - `make web` or `build.sh web`.
- Run with `emrun out.html`

## Requirements:

- raylib
- make or cmake
- gcc for desktop compilation
- emscripten for browser / web compilation
