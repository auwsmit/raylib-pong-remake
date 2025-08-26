# Credit to raylib's makefiles and the RAD Debugger for many pointers and ideas

# --- Usage Notes -------------------------------------------------------------
#
# This makefile is used to build this game project on Windows and Linux.
#
# With no arguments, running `make` will build the game executable for Desktop
# with gcc and place a copy in the repo directory. It can take a single
# argument that specifies which compiler to use.
#
# Below is a list of arguments you can use:
# `make msvc`  --> use msvc/cl.exe to compile, and make .pdb debug files
# `make web`   --> compile to web assembly with emscripten
# `make clean` --> delete all previously generated build files
#
# -----------------------------------------------------------------------------

# ==============================================================================
# Platform Detection
# ==============================================================================

ifeq ($(OS),Windows_NT)
    PLATFORM  := WINDOWS
    EXTENSION := .exe
    OBJ_EXT   := .obj
else
    UNAME_S   := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM := LINUX
    endif
    EXTENSION :=
    OBJ_EXT   := .o
endif

# ==============================================================================
# Project Config
# ==============================================================================

# Output executable name
OUTPUT     := pong

# Source code, headers, and object file paths
SRC_DIR    := code
SRC        := $(wildcard $(SRC_DIR)/*.c)
HEADERS    := $(wildcard $(SRC_DIR)/*.h)
OBJS       := $(SRC:.c=$(OBJ_EXT))

# raylib path
RAYLIB_INC := raylib/include
RAYLIB_LIB := raylib/lib

# ==============================================================================
# Compiler Settings
# ==============================================================================

# Compiler fallback just in case
CC ?= gcc

# Debug build by default
CONFIG ?= DEBUG

# Define C compiler flags: CFLAGS
# -----------------------------------------------------------------------------
#  -O1                  defines optimization level
#  -g                   include debug information on compilation
#  -s                   strip unnecessary data from build
#  -Wall                turns on most, but not all, compiler warnings
#  -std=c99             defines C language mode (standard C from 1999 revision)
#  -std=gnu99           defines C language mode (GNU C from 1999 revision)
#  -Wno-missing-braces  ignore invalid warning (GCC bug 53119)
#  -Wno-unused-value    ignore unused return values of some functions (i.e. fread())
#  -D_DEFAULT_SOURCE    use with -std=c99 on Linux and PLATFORM_WEB, required for timespec
CFLAGS := -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result
# Additional flags for compiler (if desired)
#  -Wextra                  enables some extra warning flags that are not enabled by -Wall
#  -Wmissing-prototypes     warn if a global function is defined without a previous prototype declaration
#  -Wstrict-prototypes      warn if a function is declared or defined without specifying the argument types
#  -Werror=implicit-function-declaration   catch function calls without prior declaration
CFLAGS += -Wextra -Wmissing-prototypes -Wstrict-prototypes

# Debug or Release flags
ifeq ($(CONFIG),DEBUG)
    DEBUG_FLAGS = -g -O0
else ifeq ($(CONFIG),RELEASE)
    DEBUG_FLAGS = -O2
endif

# Define C preprocessor flags and linker/library flags
CPPFLAGS := -I$(RAYLIB_INC)
LDFLAGS  := -lraylib
ifeq ($(PLATFORM),WINDOWS)
    LDFLAGS  += -L$(RAYLIB_LIB)/windows -lopengl32 -lgdi32 -lwinmm
else ifeq ($(PLATFORM),LINUX)
    LDFLAGS  += -lGL -lm -lpthread -ldl -lrt -lX11
endif

# Web (emscripten emcc) Flags
# -----------------------------------------------------------------------------
# -sUSE_GLFW=3                Use glfw3 library (context/input management)
# -sALLOW_MEMORY_GROWTH=1     to allow memory resizing -> WARNING: Audio buffers could FAIL!
# -sTOTAL_MEMORY=16777216     to specify heap memory size (default = 16MB) (67108864 = 64MB)
# -sUSE_PTHREADS=1            multithreading support
# -sWASM=0                    disable Web Assembly, emitted by default
# -sASYNCIFY                  lets synchronous C/C++ code interact with asynchronous JS
# -sFORCE_FILESYSTEM=1        force filesystem to load/save files data
# -sASSERTIONS=1              enable runtime checks for common memory allocation errors (-O1 and above turn it off)
# -sGL_ENABLE_GET_PROC_ADDRESS  enable using the *glGetProcAddress() family of functions, required for extensions loading
# -sEXPORTED_FUNCTIONS=       export needed functions (only for newer versions of emscripten to help reduce filesize)
# -sEXPORTED_RUNTIME_METHODS= export runtime functions
# --profiling                 include information for code profiling
# --memory-init-file 0        to avoid an external memory initialization code file (.mem)
# --preload-file resources    specify a resources folder for data compilation
# --source-map-base           allow debugging in browser with source map
WEBFLAGS := -O3 -sUSE_GLFW=3 -sFORCE_FILESYSTEM=1 -sASYNCIFY -DPLATFORM_WEB \
            -sEXPORTED_FUNCTIONS=_main,requestFullscreen -sTOTAL_MEMORY=67108864 \
	    -sEXPORTED_RUNTIME_METHODS=HEAPF32 --shell-file $(SRC_DIR)/shell.html
WEB_LIBS := -lraylib -L$(RAYLIB_LIB)/web

# MSVC Flags
# -----------------------------------------------------------------------------
# /Fo    Specify output directory for object files
# /Od    Disable optimizations (good for debugging)
# /W3    Set warning level to 3 (default is 1, max is 4)
# /MD    Link against MSVCRT.DLL (multithreaded DLL runtime)
# /Zi    Generate complete debugging information (.pdb files)
MSVC_CFLAGS := /Fo"$(SRC_DIR)\\" /Od /W3 /MD /Zi
MSVC_LIBS   := /link /DEBUG /LIBPATH:"$(RAYLIB_LIB)/windows-msvc" \
               raylib.lib gdi32.lib winmm.lib user32.lib shell32.lib

# ==============================================================================
# Targets
# ==============================================================================

# NOTE: make alias syntax reminder
# target: dependency1 dependency2
# % = wildcard match between target and dependency
# $@ = target, $< = dependency1, $^ = all dependencies

# tell `make` that these aren't files
.PHONY: all msvc web gh-pages clean

# Compile project with no arguments given
all: $(OUTPUT)$(EXTENSION)

# Desktop build
$(OUTPUT)$(EXTENSION): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) -DPLATFORM_WEB

# Compile c files to object files
$(SRC_DIR)/%$(OBJ_EXT): $(SRC_DIR)/%.c $(HEADERS)
	$(CC) -c $< -o $@ $(DEBUG_FLAGS) $(CFLAGS) $(CPPFLAGS) -DPLATFORM_WEB

# Build with MSVC cl.exe and produce .pdb debug files
msvc:
	cl /Fe:$(OUTPUT)$(EXTENSION) $(SRC) $(MSVC_CFLAGS) /I"$(RAYLIB_INC)" $(MSVC_LIBS)

# Build to web assembly with emscripten
web:
	emcc -o $(OUTPUT).html $(SRC) $(CFLAGS) $(WEBFLAGS) $(CPPFLAGS) $(WEB_LIBS)

# (Automated) Build for upload to GitHub pages (see .github/workflows/deploy.yaml)
gh-pages:
	@mkdir -p build_web
	emcc -o build_web/index.html $(SRC) $(CFLAGS) $(WEBFLAGS) $(CPPFLAGS) $(WEB_LIBS)

# Clean up generated build files
clean:
	@rm -rf $(OUTPUT)$(EXTENSION) $(OBJS) \
	        $(OUTPUT).html $(OUTPUT).js $(OUTPUT).wasm build_web/ \
	        $(OUTPUT).ilk $(OUTPUT).pdb vc140.pdb *.rdi
	@echo "Make build files cleaned"

