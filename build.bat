@echo off

:: cd to the script directory
cd "%~dp0"

if "%1"=="" (
    :: Build for desktop
    cmake -DPLATFORM=Desktop -B build
    cmake --build build
    del /q game.exe
    copy build\Debug\game.exe .
    goto :eof
) else if "%1"=="web" (
    :: Build for web
    mkdir build_web
    cd build_web
    call emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
    call emmake make
    cd ..
    del /q game.html game.js game.wasm
    copy build_web\game.html .
    copy build_web\game.js .
    copy build_web\game.wasm .
    goto :eof
) else if "%1"=="clean" (
    :: Clean up old files
    rmdir /s /q build
    rmdir /s /q build_web
    del game.exe
    del game.html game.js game.wasm
    goto :eof
)
