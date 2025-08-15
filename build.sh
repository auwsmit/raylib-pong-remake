#!/usr/bin/env bash

# --- Usage Notes -------------------------------------------------------------
#
# This build script is a wrapper for CMake to build this project on a Unix OS.
#
# With no arguments, running `build.sh` will build the game and place a copy of
# game executable in the repo directory. Currently takes a single argument that
# specifies which compiler to use, or which platform to compile for.
#
# Below is a list of arguments you can use:
# `build.sh web`  --> compile to web assembly with emscripten
# `build.sh clean` -> delete all previously generated build files
#
# -----------------------------------------------------------------------------

# cd to the script directory
cd "$(dirname "$0")"

# Build for Desktop
# -----------------------------------------------------------------------------
if [ -z $1 ]; then
    cmake -DPLATFORM=Desktop -B build/desktop
    cmake --build build/desktop
    rm -f game
    cp build/desktop/game .

# Build for Web
# -----------------------------------------------------------------------------
elif [ $1 == "web" ]; then
    mkdir -p build/web
    cd build/web
    emcmake cmake ../.. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
    emmake make
    cd ../..
    rm -f game.*
    cp build/web/game.html .
    cp build/web/game.js .
    cp build/web/game.wasm .

# Clean up old files
# -----------------------------------------------------------------------------
elif [ $1 == "clean" ]; then
    rm -rf build/ game game.*
    echo "CMake build files cleaned"
fi
