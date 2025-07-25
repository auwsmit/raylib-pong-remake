#!/usr/bin/env bash

# cd to the script directory
cd "$(dirname "$0")"/..

if [ -z $1 ]; then
    cmake -DPLATFORM=Desktop -B build
    cmake --build build
    rm out.exe
    cp build/out .
elif [ $1 == "web" ]; then
    mkdir build_web
    cd build_web
    emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
    emmake make
    cd ..
    rm out.html out.js out.wasm
    cp build_web/out.html .
    cp build_web/out.js .
    cp build_web/out.wasm .
fi
