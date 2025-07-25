@echo off

:: root path of project to run script from anywhere
set proj_root=%~dp0..\
cd "%proj_root%"

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
)
