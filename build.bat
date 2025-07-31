@echo off

:: cd to the script directory
cd "%~dp0"

if "%1"=="" ( :: Build for desktop
    cmake -DPLATFORM=Desktop -B build\desktop
    cmake --build build\desktop
    del /q game.exe
    copy build\desktop\Debug\game.exe .
    goto :eof
) else if "%1"=="gcc" ( :: Build with gcc, useful if Visual Studio is unavailable
    cmake -DPLATFORM=Desktop -G "MinGW Makefiles" -B build\desktop
    cmake --build build\desktop
    del /q game.exe
    copy build\desktop\game.exe .
    goto :eof

) else if "%1"=="web" ( :: Build for web
    mkdir build\web
    cd build\web
    call emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
    call emmake make
    cd ..\..
    del /q game.html game.js game.wasm
    copy build\web\game.html .
    copy build\web\game.js .
    copy build\web\game.wasm .
    goto :eof

) else if "%1"=="clean" ( :: Clean up old files
    rmdir /s /q build
    rmdir /s /q build\web
    del /q game.*
    echo Build files cleaned.
    goto :eof
)
