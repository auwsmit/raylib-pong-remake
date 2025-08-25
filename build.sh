#!/usr/bin/env bash
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

# Credit to RAD Debugger for many pointers and ideas

# Usage Notes
# -----------------------------------------------------------------------------
#
# This is the build script for Pong Remake with raylib for Linux.
#
# It can be used in two main ways:
# 1. `./build` to simply compile the game with a C compiler
# 2. `./build cmake` to setup and run the CMake build system
# Either way, you will find the output executable in the project directory.
#
# Additional arguments can be given to choose the compiler, compiler settings,
# or to clean up old build files.
#
# Below is a non-exhaustive list of arguments you can use:
# `build release` -> optimized build, no debug symbols
# `build clang`   -> use clang compiler
# `build web`     -> compile to web assembly with emscripten
# `build clean`   -> delete old generated build files (excluding CMakefi
# CMake build:
# `build cmake`       -> setup and build using CMake
# `build cmake web`   -> compile to web assembly with emscripten
# `build cmake clean` -> delete CMake's old build files
#
# Note: CMake will automatically download and build raylib if needed
#
# -----------------------------------------------------------------------------

# Project Config
# -----------------------------------------------------------------------------
output=pong
cmake_build_dir=build
web_shell=code/shell.html
source_code=
for f in "$script_dir/code"/*.c; do source_code="$source_code $f"; done

# Script Entry Point
# -----------------------------------------------------------------------------
main()
{
    script_unpack_args "$@"
    pushd "$script_dir" > /dev/null
    if [[ "$cmake" == 1 ]]; then
        script_choose_cmake_lines
        script_cmake_config_and_build
    else
        script_choose_simple_lines
        script_simple_build
    fi
    popd
}

# Unpack Arguments
# -----------------------------------------------------------------------------
script_unpack_args()
{
    for arg in "$@"; do eval "$arg=1"; done
    if [[ "$clean" == 1 ]]; then
        echo "[clean mode]" && script_build_cleanup
        exit 0
    fi
    if [[ "$release" != 1 ]]; then debug=1; fi
    if [[ "$release" == 1 ]]; then debug=0 && echo "[release mode]"; fi
    if [[ "$debug" == 1   ]]; then release=0 && echo "[debug mode]"; fi
    if [[ "$cmake" == 1   ]]; then
        echo "[cmake build]"
        if [[ "$web" != 1 ]]; then gcc=1 && echo "[gcc compile]"; fi
    else
        simple_build=1 && echo "[simple build]"
        if [[ "$clang" == 1 ]]; then gcc=0 && echo "[clang compile]"; fi
        if [[ "$web" == 1   ]]; then clang=0; fi
        if [[ "$web" != 1 && "$clang" != 1 ]]; then
            gcc=1 && echo "[gcc compile]"
        fi
    fi
    if [[ "$web" == 1 ]]; then echo "[web compile]"; fi
}

# Define and Choose CMake Lines
script_choose_cmake_lines()
{
    # Line Definitions
    cmake_setup_desktop="cmake -DOUTPUT_NAME=$output -B \"$cmake_build_dir/desktop\" -DPLATFORM=Desktop"
    cmake_build_desktop="cmake --build \"$cmake_build_dir/desktop\""
    cmake_setup_web="emcmake cmake -DOUTPUT_NAME=$output -DCMAKE_EXECUTABLE_SUFFIX=\".html\" -B \"$cmake_build_dir/web\" -DPLATFORM=Web"
    cmake_build_web="emmake make -C \"$cmake_build_dir/web\""

    # Choose Lines
    if [[ "$web" == 1 ]]; then
        output_dir=$cmake_build_dir/web
        cmake_setup_cmd=$cmake_setup_web
        cmake_build_cmd=$cmake_build_web
    else
        output_dir=$cmake_build_dir/desktop
        cmake_setup_cmd=$cmake_setup_desktop
        cmake_build_cmd=$cmake_build_desktop
    fi
    if [[ "$release" == 1 ]]; then cmake_setup_flags='-DCMAKE_BUILD_TYPE=Release'; fi
    if [[ "$debug" == 1 ]]; then cmake_setup_flags='-DCMAKE_BUILD_TYPE=Debug'; fi
}

# Define and Choose Compile/Link Lines
script_choose_simple_lines()
{
    # Line Definitions
    cc_common='-I"raylib/include" -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Wextra -Wmissing-prototypes -Wstrict-prototypes'
    cc_link='-lraylib -lGL -lm -lpthread -ldl -lrt -lX11'
    cc_debug='-g -O0'
    cc_release='-O3'
    cc_web='-sUSE_GLFW=3 -sFORCE_FILESYSTEM=1 -sASYNCIFY -DPLATFORM_WEB -sEXPORTED_FUNCTIONS=_main,requestFullscreen -sTOTAL_MEMORY=67108864 -sEXPORTED_RUNTIME_METHODS=HEAPF32 --shell-file "$web_shell"'
    cc_weblink='-L"raylib/lib/web" -lraylib'
    cc_out='-o'

    # Choose Lines
    if [[ "$gcc" == 1     ]]; then compile="gcc $cc_common"; fi
    if [[ "$clang" == 1   ]]; then compile="clang $cc_common"; fi
    if [[ "$web" == 1     ]]; then compile="emcc $cc_common $cc_web"; fi
    if [[ "$web" == 1     ]]; then compile_link="$cc_weblink"; fi
    if [[ "$web" != 1     ]]; then compile_link="$cc_link"; fi
    if [[ "$web" == 1     ]]; then compile_out="$cc_out $output.html"; fi
    if [[ "$web" != 1     ]]; then compile_out="$cc_out $output.exe"; fi
    if [[ "$debug" == 1   ]]; then compile="$compile $cc_debug"; fi
    if [[ "$release" == 1 ]]; then compile="$compile $cc_release"; fi
}

script_cmake_config_and_build()
{
    eval $cmake_setup_cmd $cmake_setup_flags
    eval $cmake_build_cmd $cmake_build_flags
    if [[ "$web" == 1 ]]; then
        rm -f "$output.html" "$output.js" "$output.wasm"
        cp "$output_dir/$output.html" .
        cp "$output_dir/$output.js" .
        cp "$output_dir/$output.wasm" .
    else
        rm -f "$output.exe"
        cp "$output_dir/$output" .
    fi
}

script_simple_build()
{
    eval $compile $source_code $compile_link $compile_out
}

script_build_cleanup()
{
    pushd "$script_dir" > /dev/null
    if [[ "$cmake" == 1 ]]; then
        rm -rf $output build/
        echo "CMake build files cleaned"
    else
        rm -rf $output build_web/ $output.html $output.js $output.wasm
        echo "Build files cleaned"
    fi
    popd
}

main "$@"
