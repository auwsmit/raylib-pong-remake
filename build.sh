#!/usr/bin/env bash
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

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
    [[ "$clean" == 1 ]] &&    echo "[clean mode]" && goto_script_build_cleanup
    [[ "$release" != 1 ]] && debug=1
    [[ "$debug" == 1 ]] &&    echo "[debug mode]" && release=0
    [[ "$release" == 1 ]] &&  echo "[release mode]" && debug=0
    if [[ "$cmake" == 1 ]]; then
        echo "[cmake build]"
        [[ "$web" != 1 ]] &&  echo "[gcc compile]" && gcc=1
    else
        echo "[simple build]" && simple_build=1
        [[ "$clang" == 1 ]] && echo "[clang compile]" && gcc=0
        [[ "$web" == 1 ]] &&   clang=0
        if [[ "$clang" != 1 && "$web" != 1 ]]; then
            echo "[gcc compile]" && gcc=1
        fi
    fi
    [[ "$web" == 1 ]] && echo "[web compile]"
}

# Define and Choose CMake Lines
script_choose_cmake_lines()
{
    # CMake Line Definitions
    cmake_setup_desktop="cmake -DOUTPUT_NAME=$output -B \"$cmake_build_dir/desktop\" -DPLATFORM=Desktop"
    cmake_build_desktop="cmake --build \"$cmake_build_dir/desktop\""
    cmake_setup_web="emcmake cmake -DOUTPUT_NAME=$output -DCMAKE_EXECUTABLE_SUFFIX=\".html\" -B \"$cmake_build_dir/web\" -DPLATFORM=Web"
    cmake_build_web="emmake make -C \"$cmake_build_dir/web\""

    # Choose CMake Lines
    if [[ "$web" == 1 ]]; then
        output_dir=$cmake_build_dir/web
        cmake_setup_cmd=$cmake_setup_web
        cmake_build_cmd=$cmake_build_web
    else
        output_dir=$cmake_build_dir/desktop
        cmake_setup_cmd=$cmake_setup_desktop
        cmake_build_cmd=$cmake_build_desktop
    fi
    [[ "$release" == 1 ]] && cmake_setup_flags='-DCMAKE_BUILD_TYPE=Release'
    [[ "$debug" == 1 ]] &&   cmake_setup_flags='-DCMAKE_BUILD_TYPE=Debug'
}

# Define and Choose Compile/Link Lines
script_choose_simple_lines()
{
    # Compile/Link Line Definitions
    cc_common='-I"raylib/include" -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Wextra -Wmissing-prototypes -Wstrict-prototypes'
    cc_link='-lraylib -lGL -lm -lpthread -ldl -lrt -lX11'
    cc_debug='-g -O0'
    cc_release='-O3'
    cc_web='-sUSE_GLFW=3 -sFORCE_FILESYSTEM=1 -sASYNCIFY -DPLATFORM_WEB -sEXPORTED_FUNCTIONS=_main,requestFullscreen -sTOTAL_MEMORY=67108864 -sEXPORTED_RUNTIME_METHODS=HEAPF32 --shell-file "$web_shell"'
    cc_weblink='-L"raylib/lib/web" -lraylib'
    cc_out='-o'

    # Choose Compile/Link Lines
    [[ "$gcc" == 1 ]] &&     compile="gcc $cc_common"
    [[ "$clang" == 1 ]] &&   compile="clang $cc_common"
    [[ "$web" == 1 ]] &&     compile="emcc $cc_common $cc_web"
    [[ "$web" == 1 ]] &&     compile_link="$cc_weblink"
    [[ "$web" != 1 ]] &&     compile_link="$cc_link"
    [[ "$web" == 1 ]] &&     compile_out="$cc_out $output.html"
    [[ "$web" != 1 ]] &&     compile_out="$cc_out $output.exe"
    [[ "$debug" == 1 ]] &&   compile="$compile $cc_debug"
    [[ "$release" == 1 ]] && compile="$compile $cc_release"
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

goto_script_build_cleanup()
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
    exit 0
}

main "$@"
