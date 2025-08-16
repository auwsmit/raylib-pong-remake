@echo off

:: --- Usage Notes ------------------------------------------------------------
::
:: This build script is a wrapper for CMake to build this project on Windows.
::
:: With no arguments, running `build` will download and build raylib if needed,
:: generate a Visual Studio solution for the game, build it with MSVC, and then
:: place a copy of the game executable in the repo directory. This script can
:: take a single argument that specifies which compiler to use, or which
:: platform to compile for.
::
:: You can find the generated Visual Studio solution within build\desktop\
::
:: Below is a list of arguments you can use:
:: `build gcc`  --> use MinGW compiler instead of MSVC
:: `build web`  --> compile to web assembly with emscripten
:: `build clean` -> delete all previously generated build files
::
:: ----------------------------------------------------------------------------

:: cd to the script directory
cd "%~dp0"

:: Build for desktop
:: ----------------------------------------------------------------------------
if "%1"=="" (
    cmake -DPLATFORM=Desktop -B build\desktop
    cmake --build build\desktop
    del /q game.exe
    copy build\desktop\Debug\game.exe .
    goto :eof
)

:: Build with gcc, useful if Visual Studio is unavailable
:: ----------------------------------------------------------------------------
if "%1"=="gcc" (
    cmake -DPLATFORM=Desktop -G "MinGW Makefiles" -B build\desktop
    cmake --build build\desktop
    del /q game.exe
    copy build\desktop\game.exe .
    goto :eof
)

:: Build for web
:: ----------------------------------------------------------------------------
if "%1"=="web" (
    mkdir build\web
    pushd build\web
    call emcmake cmake ..\.. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
    call emmake make
    popd
    del /q game.html game.js game.wasm
    copy build\web\game.html .
    copy build\web\game.js .
    copy build\web\game.wasm .
    goto :eof
)

:: Clean up old files
:: ----------------------------------------------------------------------------
if "%1"=="clean" (
    rmdir /s /q build
    del /q game.*
    echo CMake build files cleaned
    goto :eof
)
