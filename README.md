# Raylib Pong Remake

[**Click here to play in the browser!**](https://auwsmit.github.io/raylib-pong-remake/)

This is my first Raylib project, made to learn about the library and basic game
development. Still a work in progress.

Runs on Windows, Linux, MacOS, and web browsers. I don't have a Mac to test on
though, so I would appreciate anyone who can confirm if it works there.

## Controls

- `Enter`/`Space` to select

- `Esc`/`Backspace` to go back

- Player 1: Press `W`/`S` to move. Hold `A`/`D`/`LShift` to move faster

- Player 2: Press `I`/`K` or `Up`/`Down` to move. Hold `Left`/`Right`/`RShift`/`J`/`L` to move faster

## Build for Desktop
- Either run `build.sh` or `build.bat`, depending on your platform.
    - You can also use `make` to build the game.
- Run `game` or `game.exe` to start the game.

## Build for Browser
- Same as desktop, but add `web` as an argument:
    - Run `build.sh web` or `make web`.
- Run game with `emrun game.html`

## Requirements to build:

- Library: [raylib](https://www.raylib.com/), duh :P
- A C compiler: [GCC](https://gcc.gnu.org/), [Clang](https://clang.llvm.org/), or [Visual Studio](https://visualstudio.microsoft.com/)
- Build system: [CMake](https://cmake.org/)
    - Alternatively, use [Make](https://en.wikipedia.org/wiki/Make_(software)) with GCC or Clang
- [emscripten](https://emscripten.org/) for browser / web assembly compilation

For Windows users with no build tools or less experience, [w64devkit](https://github.com/skeeto/w64devkit) is a fast and easy way to build this project. Just download and extract it anywhere. Then use the included unix-like terminal, or add the `w64devkit/bin` tools to your PATH. Now you can simply `make` this project like you would on Linux. As an added bonus, you now have access to many nice Unix command line tools.

Also for Windows users, if you have GCC but not Visual Studio for some reason, you can use `build.bat gcc`.
