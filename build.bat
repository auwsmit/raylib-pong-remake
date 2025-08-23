@echo off
setlocal enabledelayedexpansion
set script_dir=%~dp0

:: Credit to the RAD Debugger for many pointers and ideas

:: Usage Notes
:: ----------------------------------------------------------------------------
::
:: This build script can be used in two main ways:
:: 1. `.\build` to simply compile the game with a C compiler
:: 2. `.\build cmake` to setup and run the CMake build system
:: Either way, you will find the output executable in the project directory.
::
:: Additional arguments can be given to choose the compiler, compiler settings,
:: or to clean up old build files.
::
:: Below is a non-exhaustive list of arguments you can use:
:: `build release` -> optimized build, no debug symbols
:: `build clang`   -> use clang compiler
:: `build msvc`    -> use msvc compiler
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

:: Project Specific Settings
:: ----------------------------------------------------------------------------
set output=game
set cmake_build_dir=build
set raylib=raylib
set src_dir=code
set src=
for %%f in (%src_dir%\*.c) do set src=!src! "%%f"

:: Unpack Arguments
:: ----------------------------------------------------------------------------
for %%a in (%*) do set "%%a=1"
if "%clean%"=="1" echo [clean mode] && goto :clean
if not "%release%"=="1" set "debug=1"
if "%debug%"=="1"   echo [debug mode]   && set "release=0"
if "%release%"=="1" echo [release mode] && set "debug=0"
if "%clang%"=="1" set "msvc=0"
if "%msvc%"=="1" set "clang=0"
if "%web%"=="1" set "clang=0" && set "msvc=0"
if "%cmake%"=="1" (
    echo [cmake mode]
    if not "%gcc%"=="1" echo [msvc compile] && set "msvc=1"
    if "%gcc%"=="1"     echo [gcc compile]
) else (
    echo [default mode] && set "default_mode=1"
    if not "%clang%"=="1" if not "%msvc%"=="1" if not "%web%"=="1" (
        set "gcc=1" && echo [gcc compile]
    )
    if "%clang%"=="1"  echo [clang compile]
    if "%msvc%"=="1"   echo [msvc compile]
)
if "%web%"=="1" echo [web compile]

:: CMake Definitions
:: ----------------------------------------------------------------------------
if "%cmake%"=="1" (
    set cmake_setup_desktop=cmake -DOUTPUT_NAME=%output% -B "%cmake_build_dir%\desktop" -DPLATFORM=Desktop
    set cmake_build_desktop=cmake --build "%cmake_build_dir%\desktop"
    set cmake_setup_web=call emcmake cmake -DOUTPUT_NAME=%output% -DCMAKE_EXECUTABLE_SUFFIX=".html" -B "%cmake_build_dir%\web" -DPLATFORM=Web
    set cmake_build_web=call emmake make -C "%cmake_build_dir%\web"
)

:: CMake Settings
:: ----------------------------------------------------------------------------
if "%cmake%"=="1"   (
    if "%web%"=="1" (
        set output_dir=%cmake_build_dir%\web
        set cmake_setup_cmd=%cmake_setup_web%
        set cmake_build_cmd=%cmake_build_web%
    ) else (
        set cmake_setup_cmd=%cmake_setup_desktop%
        set cmake_build_cmd=%cmake_build_desktop%
    )
    if "%msvc%"=="1" (
        if "%release%"=="1" (
            set cmake_build_flags=--config Release
            set output_dir=%cmake_build_dir%\desktop\Release
        ) else (
            set cmake_build_flags=--config Debug
            set output_dir=%cmake_build_dir%\desktop\Debug
        )
    ) else (
        set cmake_setup_flags=-G "MinGW Makefiles"
        if "%release%"=="1" set cmake_setup_flags=-DCMAKE_BUILD_TYPE=Release %cmake_setup_flags%
        if "%debug%"=="1"   set cmake_setup_flags=-DCMAKE_BUILD_TYPE=Debug %cmake_setup_flags%
    )
)

:: Compile/Link Line Definitions
:: ----------------------------------------------------------------------------
set cc_common=  -I"%raylib%\include" -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Wextra -Wmissing-prototypes -Wstrict-prototypes
set cc_link=    -L"%raylib%\lib\windows" -lraylib -lopengl32 -lgdi32 -lwinmm
set cc_debug=   -g -O0
set cc_release= -O2
set cc_web= -sUSE_GLFW=3 -sFORCE_FILESYSTEM=1 -sASYNCIFY -DPLATFORM_WEB -sEXPORTED_FUNCTIONS=_main,requestFullscreen -sTOTAL_MEMORY=67108864 -sEXPORTED_RUNTIME_METHODS=HEAPF32 --shell-file "%src_dir%\shell.html"
set cc_weblink= -L"%raylib%\lib\web" -lraylib
set cc_out=     -o
set cl_common=  cl /I"%raylib%\include" /Fo"%src_dir%\\" /W3 /MD /Zi
set cl_link=    /link /LIBPATH:"%raylib%\lib\windows-msvc" raylib.lib gdi32.lib winmm.lib user32.lib shell32.lib
set cl_debug=   -Od /DEBUG
set cl_release= -O2
set cl_out=     /out:

:: Choose Compile/Link Lines
:: ----------------------------------------------------------------------------
if "%gcc%"=="1"      set compile=gcc %cc_common%
if "%clang%"=="1"    set compile=clang %cc_common%
if "%web%"=="1"      set compile=emcc %cc_common% %cc_web%
if "%web%"=="1"      set compile_link=%cc_weblink%
if "%web%"=="1"      set compile_out=%cc_out% %output%.html
if not "%web%"=="1" if not "%msvc%"=="1" set compile_out=%cc_out% "%script_dir%%output%.exe"
if not "%web%"=="1" if not "%msvc%"=="1" set compile_link=%cc_link%
if not "%msvc%"=="1" set compile_debug=%cc_debug%
if not "%msvc%"=="1" set compile_release=%cc_release%

if "%msvc%"=="1"     set compile=%cl_common%
if "%msvc%"=="1"     set compile_debug=%cl_debug%
if "%msvc%"=="1"     set compile_release=%cl_release%
if "%msvc%"=="1"     set compile_link=%cl_link%
if "%msvc%"=="1"     set compile_out=%cl_out%"%script_dir%%output%.exe"

if "%debug%"=="1"    set compile=%compile% %compile_debug%
if "%release%"=="1"  set compile=%compile% %compile_release%

:: Build Target
:: ----------------------------------------------------------------------------
pushd "%script_dir%"
if "%cmake%"=="1" (
    %cmake_setup_cmd% %cmake_setup_flags%
    %cmake_build_cmd% %cmake_build_flags%
    if "%web%"=="1" (
        del /q "%output%.html"
        del /q "%output%.js"
        del /q "%output%.wasm"
        copy "%output_dir%\%output%.html" .
        copy "%output_dir%\%output%.js" .
        copy "%output_dir%\%output%.wasm" .
    ) else (
        del /q "%output%.exe"
        copy "%output_dir%\%output%.exe" .
    )
)

if "%default_mode%"=="1" (
    %compile% %src% %compile_link% %compile_out%
)
popd

:: Clean Up
:: ----------------------------------------------------------------------------
:clean
if not "%clean%"=="1" goto :eof
pushd "%script_dir%"
if "%cmake%"=="1" (
    rmdir /s /q build
    del /q %output%.*
    echo CMake build files cleaned
) else (
    rmdir /s /q build_web
    del /q %output%.exe %src_dir%\*.obj
    del /q %output%.html %output%.js %output%.wasm
    del /q %output%.ilk %output%.pdb vc140.pdb *.rdi
)
popd
