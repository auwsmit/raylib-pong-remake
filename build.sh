#!/usr/bin/env bash

# cd to the script directory
cd "$(dirname "$0")"

if [ -z $1 ]; then
    # Build for Desktop
    cmake -DPLATFORM=Desktop -B build
    cmake --build build
    rm -f game
    cp build/game .
elif [ $1 == "web" ]; then
    # Build for Web
    mkdir build_web
    cd build_web
    emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
    emmake make
    cd ..
    rm -f game.html game.js game.wasm
    cp build_web/game.html .
    cp build_web/game.js .
    cp build_web/game.wasm .
elif [ $1 == "clean" ]; then
    # Clean up old files
    rm -rf build
    rm -rf build_web
    rm -f game game.html game.js game.wasm
fi
