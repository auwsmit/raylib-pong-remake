@echo off

:: cd to the script directory
cd "%~dp0"

if "%1"=="" (
    cmake -DPLATFORM=Desktop -B build
    cmake --build build
    del out.exe
    copy build\Debug\out.exe .
    goto :eof
) else if "%1"=="web" (
    mkdir build_web
    cd build_web
    call emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
    call emmake make
    cd ..
    del out.html out.js out.wasm
    copy build_web\out.html .
    copy build_web\out.js .
    copy build_web\out.wasm .
    goto :eof
) else if "%1"=="clean" (
    rmdir /s /q build
    rmdir /s /q build_web
    del out.exe
    del out.html out.js out.wasm
    goto :eof
)
