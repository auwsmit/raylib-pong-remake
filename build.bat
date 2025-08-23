@echo off

:: NOTE: only CMake commands are working right now

:: Usage Notes
:: ----------------------------------------------------------------------------
::
:: This build script can be used in two main ways:
:: 1. `build` to simply compile the game with a C compiler
:: 2. `build cmake` to setup and run the CMake build system
:: Either way, you will find the output executable in the project directory.
::
:: Additional arguments can be given to choose the compiler, compiler settings,
:: or to clean up old build files.
::
:: Below is a non-exhaustive list of arguments you can use:
:: `build release` -> optimized build, no debug symbols
:: `build clang`   -> use clang compiler
:: `build msvc`    -> use msvc compiler, and make .pdb debug files
:: `build web`     -> compile to web assembly with emscripten
:: `build clean`   -> delete old generated build files (excluding CMake)
:: CMake mode:
:: `build cmake`       -> setup and build using CMake
:: `build cmake gcc`   -> use MinGW compiler instead of MSVC
:: `build cmake web`   -> compile to web assembly with emscripten
:: `build cmake clean` -> delete CMake's old build files
::
:: Notes for CMake:
:: - CMake will automatically download and build raylib if needed
:: - You can find the generated Visual Studio solution in build\desktop\
::
:: ----------------------------------------------------------------------------

set script_dir=%~dp0

:: Unpack Arguments
:: ----------------------------------------------------------------------------
for %%a in (%*) do set "%%a=1"
if "%clean%"=="1" echo [clean mode] && goto :clean
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0   && echo [release mode]
if "%web%"=="1" set clang=0 && set msvc=0 && set gcc=0
if "%cmake%"=="1" ( echo [cmake mode]
    if "%gcc%"=="1" echo [cmake-mingw mode]
    if "%~2"==""    echo [msvc compile]
) else (              echo [default mode]
    if "%clang%"=="1" set msvc=0 && echo [clang compile]
    if "%msvc%"=="1"  set clang=0 && echo [msvc compile]
)
if "%~1"==""      echo [gcc compile]
if "%web%"=="1"   echo [web compile]

:: Project Specific Settings
:: ----------------------------------------------------------------------------
set output=game
set cmake_build_dir=%script_dir%\build

:: CMake Definitions
:: ----------------------------------------------------------------------------
if "%cmake%"=="1" (
    set cmake_setup_desktop=cmake -DOUTPUT_NAME=%output% -B "%cmake_build_dir%\desktop" -DPLATFORM=Desktop
    set cmake_build_desktop=cmake --build "%cmake_build_dir%\desktop"
    set cmake_setup_web=call emcmake cmake -DOUTPUT_NAME=%output% -DCMAKE_EXECUTABLE_SUFFIX=".html" -B "%cmake_build_dir%\web" -DPLATFORM=Web
    set cmake_build_web=call emmake make -C "%cmake_build_dir%\web"
)

:: Per-Build Settings
:: ----------------------------------------------------------------------------
if "%cmake%"=="1"   (
    if "%web%"=="1" (
        set output_dir=%cmake_build_dir%\web
        set cmake_setup_cmd=%cmake_setup_web%
        set cmake_build_cmd=%cmake_build_web%
    ) else (
        set output_dir=%cmake_build_dir%\desktop\Debug
        set cmake_setup_cmd=%cmake_setup_desktop%
        set cmake_build_cmd=%cmake_build_desktop%
    )

    if "%release%"=="1" if "%msvc%"=="1" set cmake_setup_flags=--config Release
    if "%gcc%"=="1"     set cmake_setup_flags=-G "MinGW Makefiles"
    if "%release%"=="1" set cmake_setup_flags=-DCMAKE_BUILD_TYPE=Release %cmake_setup_flags%
    if "%debug%"=="1"   set cmake_setup_flags=-DCMAKE_BUILD_TYPE=Debug %cmake_setup_flags%
)

:: Build Target
:: ----------------------------------------------------------------------------
if "%cmake%"=="1" (
    %cmake_setup_cmd% %cmake_setup_flags%
    %cmake_build_cmd%
    if "%web%"=="1" (
        del /q "%script_dir%\%output%.html"
        del /q "%script_dir%\%output%.js"
        del /q "%script_dir%\%output%.wasm"
        copy "%output_dir%\%output%.html" "%script_dir%"
        copy "%output_dir%\%output%.js" "%script_dir%"
        copy "%output_dir%\%output%.wasm" "%script_dir%"
    ) else (
        :: desktop
        del /q "%script_dir%\%output%.exe"
        copy "%output_dir%\%output%.exe" "%script_dir%"
    )
)

:: Clean Up
:: ----------------------------------------------------------------------------
:clean
if not "%clean%"=="1" goto :eof
if "%cmake%"=="1" (
    rmdir /s /q "%script_dir%\build"
    del /q "%script_dir%\game.*"
    echo CMake build files cleaned
) else (
    REM echo Build files cleaned
)
