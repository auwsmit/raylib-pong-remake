#!/usr/bin/env bash

# cd to the script directory
cd "$(dirname "$0")"

if [ -z $1 ]; then # Build for Desktop
    cmake -DPLATFORM=Desktop -B build/desktop
    cmake --build build/desktop
    rm -f game
    cp build/desktop/game .
elif [ $1 == "web" ]; then # Build for Web
    mkdir -p build/web
    cd build/web
    emcmake cmake ../.. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
    emmake make
    cd ../..
    rm -f game.*
    cp build/web/game.html .
    cp build/web/game.js .
    cp build/web/game.wasm .
elif [ $1 == "clean" ]; then # Clean up old files
    rm -f game game.*
    echo "CMake's build files cleaned."
fi
