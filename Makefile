# Credit to raylib's examples makefile for many ideas
.PHONY: all web web-release clean

# Define environment variables:
# ----------------------------------------------------------------------------
# Source files to compile
PROJ_CODE = $(wildcard *.c)
# Name of output executable
PROJ_EXE  = out
# The executable's file extension (for windows and web platforms)
PROJ_EXT  =
ifeq ($(OS),Windows_NT)
    PROJ_EXT = .exe
endif

# Compiler of choice
CC = gcc

# Define compiler flags: CFLAGS
#------------------------------------------------------------------------------
#  -O1                  defines optimization level
#  -g                   include debug information on compilation
#  -s                   strip unnecessary data from build
#  -Wall                turns on most, but not all, compiler warnings
#  -std=c99             defines C language mode (standard C from 1999 revision)
#  -std=gnu99           defines C language mode (GNU C from 1999 revision)
#  -Wno-missing-braces  ignore invalid warning (GCC bug 53119)
#  -Wno-unused-value    ignore unused return values of some functions (i.e. fread())
#  -D_DEFAULT_SOURCE    use with -std=c99 on Linux and PLATFORM_WEB, required for timespec
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result
# Additional flags for compiler (if desired)
#  -Wextra                  enables some extra warning flags that are not enabled by -Wall
#  -Wmissing-prototypes     warn if a global function is defined without a previous prototype declaration
#  -Wstrict-prototypes      warn if a function is declared or defined without specifying the argument types
#  -Werror=implicit-function-declaration   catch function calls without prior declaration
CFLAGS += -Wextra -Wmissing-prototypes -Wstrict-prototypes

# 128MB of memory for emscripten, default is 16MB
WEB_HEAP_SIZE = 134217728
# Define web flags
# ----------------------------------------------------------------------------
# -Os                        # size optimization
# -O2                        # optimization level 2, if used, also set --memory-init-file 0
# -sUSE_GLFW=3              # Use glfw3 library (context/input management)
# -sALLOW_MEMORY_GROWTH=1   # to allow memory resizing -> WARNING: Audio buffers could FAIL!
# -sTOTAL_MEMORY=16777216   # to specify heap memory size (default = 16MB) (67108864 = 64MB)
# -sUSE_PTHREADS=1          # multithreading support
# -sWASM=0                  # disable Web Assembly, emitted by default
# -sASYNCIFY                # lets synchronous C/C++ code interact with asynchronous JS
# -sFORCE_FILESYSTEM=1      # force filesystem to load/save files data
# -sASSERTIONS=1            # enable runtime checks for common memory allocation errors (-O1 and above turn it off)
# -sGL_ENABLE_GET_PROC_ADDRESS # enable using the *glGetProcAddress() family of functions, required for extensions loading
# --profiling                # include information for code profiling
# --memory-init-file 0       # to avoid an external memory initialization code file (.mem)
# --preload-file resources   # specify a resources folder for data compilation
# --source-map-base          # allow debugging in browser with source map
WEBFLAGS = -sUSE_GLFW=3 -sTOTAL_MEMORY=$(WEB_HEAP_SIZE) -sFORCE_FILESYSTEM=1 -sASYNCIFY -DPLATFORM_WEB

# # Standard Windows Raylib install location
# RAYLIB_SRC = C:\raylib\raylib\src
# RAYLIB_INC = $(RAYLIB_SRC)
# RAYLIB_LIB = $(RAYLIB_SRC)

# Local raylib location
RAYLIB_INC = raylib/include
RAYLIB_LIB = raylib/lib
RAYLIB_WEB = raylib/libweb

# Include and library flags
INCLUDES   = -I$(RAYLIB_INC)
LIBS       = -L$(RAYLIB_LIB) -lraylib

# Platform specific flags
# -----------------------------------------------------------------------------
# I only use Windows and Linux atm, and can use raylib's
# CMakeLists file to compile on any other platforms
ifeq ($(OS),Windows_NT)
    LIBS += -lopengl32 -lgdi32 -lwinmm
else
    ifeq ($(shell uname),Linux)
        LIBS += -lGL -lm -lpthread -ldl -lrt -lX11
    endif
endif

ifeq ($(shell command -v rm >/dev/null 2>&1 && echo yes),yes)
    DELETE = rm -f
else
    DELETE = del
endif

all: $(PROJ_EXE)$(PROJ_EXT)

# Compile for desktop (default)
$(PROJ_EXE)$(PROJ_EXT): $(PROJ_CODE)
	$(CC) -o $(PROJ_EXE)$(PROJ_EXT) $(PROJ_CODE) $(CFLAGS) $(INCLUDES) $(LIBS) -DPLATFORM_DESKTOP

# Compile for Web
# ---------------------------------------------------------------------------
# Using a local copy of raylib's web library because Desktop is the default.
web:
	emcc -o $(PROJ_EXE).html $(PROJ_CODE) $(WEBFLAGS) $(INCLUDES) -L$(RAYLIB_WEB) -lraylib --emrun

web-release:
	emcc -o $(PROJ_EXE).html $(PROJ_CODE) -O2 $(WEBFLAGS) $(INCLUDES) -L$(RAYLIB_WEB) -lraylib

# Clean up old build files
clean:
	$(DELETE) $(PROJ_EXE)$(PROJ_EXT) $(PROJ_EXE).html $(PROJ_EXE).js $(PROJ_EXE).wasm
