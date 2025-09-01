# Pong Remake with raylib

<p align="center">
  <a href="https://auwsmit.github.io/raylib-pong-remake/">
    <img style="width:500px" src="https://github.com/auwsmit/raylib-pong-remake/blob/master/demo.gif">
  </a>
  <p align="center">
    <a href="https://auwsmit.github.io/raylib-pong-remake/"><b>Click here to play in the browser!</b></a>
  </p>
</p>

This is my first [raylib](https://www.raylib.com/) project, made to learn about
the library and basic game development. Expect lots of overly explanatory
comments within. There's still lots of refinements and features I wanted to add,
but I set a deadline, and so this is the finished state of the game after a
little over a month of work in my free time.

Runs on Windows, Linux, MacOS, and web browsers. I don't have a Mac to test on
though, so I would appreciate if anyone can confirm it compiles there. I'll also
gladly take any advice/feedback/tips/pointers on things to do differently or
improve.

## Controls

- **Select/Confirm:** `Mouse click` or `Enter` or `Space`

- **Move Player 1:**
    - `Mouse movement` (single player only)
    - Press `W`/`S` to move up and down
    - Hold `A`/`D`/`LShift` to move faster

- **Move Player 2:**
    - Press `I`/`K` or `↑`/`↓` to move up and down
    - Hold `J`/`L`/`←`/`→` to move faster

- **Pause:** `P`

- **Back to title:** `Esc`/`Backspace`

- **Toggle fullscreen:** `Alt+Enter`/`F11`/`Shift+F` (desktop only)

## Build for Desktop
1. Build by running `./build.sh cmake` or `.\build.bat cmake`, depending on your platform
    - Alternatively, just run `make` to build the game
2. Play by running `./pong` or `.\pong.exe`

## Build for Browser
1. Same as desktop, but add `web` as an argument:
    - Run `build.sh cmake web` or `make web`
2. Play by running `emrun pong.html`

## Requirements to build:

- Library: [raylib](https://www.raylib.com/), duh :P
- A C compiler: [GCC](https://gcc.gnu.org/), [Clang](https://clang.llvm.org/) ([llvm-mingw](https://github.com/mstorsjo/llvm-mingw)
on Windows), or [Visual Studio](https://visualstudio.microsoft.com/)
    - [emscripten](https://emscripten.org/) (only for browser / web assembly compilation)
- Build system: [CMake](https://cmake.org/) or [Make](https://en.wikipedia.org/wiki/Make_(software)) (macOS must use CMake)

For Windows users with no build tools or less experience, [w64devkit](https://github.com/skeeto/w64devkit) is a fast and easy way to build this project. Just download and extract it anywhere. Then use the included unix-like terminal, or add the `w64devkit/bin` tools to your PATH. Now you can simply `make` this project like you would on Linux. As an added bonus, you now have access to many nice Unix command line tools.
