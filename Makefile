# Credit to raylib's makefiles for many pointers and ideas

# Tell `make` that these aren't files
.PHONY: all web web-release clean

# =============================================================================
# DEFINE ENVIRONMENT VARIABLES:
# =============================================================================

# Source files to compile
CODE = $(wildcard code/*.c)
HEADERS = $(wildcard code/*.h)

# Add object files from source files (not currently in use)
OBJS = $(CODE:.c=.o)

# Name of output executable
OUTPUT = game

# Name of output files for web assembly
WEB_OUT = index

# The executable's file extension (for Windows and Web platforms)
EXTENSION =
ifeq ($(OS),Windows_NT)
    EXTENSION = .exe
endif

# Define C compiler flags: CFLAGS
# ------------------------------------------------------------------------
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

# Define web compiler flags: WEBFLAGS
# -----------------------------------------------------------------------
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
# -sEXPORTED_FUNCTIONS       # require functions which are not included in order to reduce code size
# --profiling                # include information for code profiling
# --memory-init-file 0       # to avoid an external memory initialization code file (.mem)
# --preload-file resources   # specify a resources folder for data compilation
# --source-map-base          # allow debugging in browser with source map
WEBFLAGS = -sUSE_GLFW=3 -sTOTAL_MEMORY=$(WEB_HEAP_SIZE) -sFORCE_FILESYSTEM=1 -sASYNCIFY -DPLATFORM_WEB -sEXPORTED_FUNCTIONS=_main,requestFullscreen
# Use shell file for Web compilation page layout
WEBFLAGS += --shell-file code/shell.html

# 128MB of memory for emscripten, default is 16MB
WEB_HEAP_SIZE = 134217728

# Local raylib location
RAYLIB_INC = raylib/include
RAYLIB_LIB = raylib/lib

# Include and library flags (more libraries added below)
INCLUDES = -I$(RAYLIB_INC)
LIBS = -lraylib

# =============================================================================
# CROSS-PLATFORM COMPATIBILITY
# =============================================================================
# I only use Windows and Linux atm, but this might work on macOS

# Detect compiler (gcc or clang preferred)
ifeq ($(OS),Windows_NT)
    CC ?= $(shell where gcc 2>nul || where clang 2>nul || echo cc)
else
    CC ?= $(shell command -v gcc || command -v clang || echo cc)
endif

# Platform-specific library flags
ifeq ($(OS),Windows_NT)
    LIBS += -L$(RAYLIB_LIB)/windows
    LIBS += -lopengl32 -lgdi32 -lwinmm
else
    ifeq ($(shell uname),Linux)
        LIBS += -lGL -lm -lpthread -ldl -lrt -lX11
    endif
endif

# =============================================================================
# MAKEFILE TARGETS
# =============================================================================
all: $(OUTPUT)$(EXTENSION)

# Compile for desktop (default)
$(OUTPUT)$(EXTENSION): $(CODE) $(HEADERS)
	$(CC) -o $(OUTPUT)$(EXTENSION) $(CODE) $(CFLAGS) $(INCLUDES) $(LIBS) -DPLATFORM_DESKTOP

# Compile for Web:
# Use a local copy of raylib's web library in case it isn't available
web: LIBS = -lraylib -L$(RAYLIB_LIB)/web
web:
	emcc -o $(WEB_OUT).html $(CODE) $(WEBFLAGS) $(INCLUDES) $(LIBS)

# Clean up old build files
clean:
	rm -rf $(OUTPUT)$(EXTENSION) $(OUTPUT).html $(OUTPUT).js $(OUTPUT).wasm $(OBJS)
	@echo "Build files cleaned."

