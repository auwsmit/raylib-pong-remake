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
# `make CONFIG=RELEASE`  -> optimized build, no debug files (debug is default)
# `make msvc`  --> use msvc/cl.exe to compile
# `make web`   --> compile to web assembly with emscripten
# `make clean` --> delete all previously generated build files
#
# -----------------------------------------------------------------------------

# ==============================================================================
# Platform Settings
# ==============================================================================

ifeq ($(PLATFORM),WEB)
    EXTENSION := .html
    OBJ_EXT   := .obj
else ifeq ($(OS),Windows_NT)
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
ifeq ($(PLATFORM),WEB)
    CC := emcc
else
    CC ?= gcc
endif

# Debug build by default
CONFIG ?= DEBUG

# Debug or Release flags
ifeq ($(CC),cl)
    ifeq ($(CONFIG),RELEASE)
        OPT_FLAGS := /O2
    else ifeq ($(CONFIG),DEBUG)
        DEBUG_FLAGS := /Od /Zi
    endif
else ifeq ($(PLATFORM),WEB) # Web always optimized
    OPT_FLAGS := -O3
else ifeq ($(CONFIG),RELEASE)
    OPT_FLAGS := -O2
else ifeq ($(CONFIG),DEBUG)
    DEBUG_FLAGS := -g -O0
endif

# Define C compiler flags
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

# MSVC cl.exe Flags
# -----------------------------------------------------------------------------
# /Fo    Specify output directory for object files
# /Od    Disable optimizations (good for debugging)
# /W3    Set warning level to 3 (default is 1, max is 4)
# /MD    Link against MSVCRT.DLL (multithreaded DLL runtime)
# /Zi    Generate complete debugging information (.pdb files)
ifeq ($(CC),cl)
    CFLAGS := /Fo"$(SRC_DIR)\\" /W3 /MD
endif

# Define C preprocessor flags and linker flags
CPPFLAGS := -I$(RAYLIB_INC)
LINKFLAGS  := -lraylib
ifeq ($(CC),cl)
    LINKFLAGS  := /link /LIBPATH:"$(RAYLIB_LIB)/windows-msvc" \
                  raylib.lib gdi32.lib winmm.lib user32.lib shell32.lib
    ifeq ($(CONFIG),DEBUG)
        LINKFLAGS += /DEBUG
    endif
else ifeq ($(PLATFORM),WINDOWS)
    LINKFLAGS  += -L$(RAYLIB_LIB)/windows -lopengl32 -lgdi32 -lwinmm
else ifeq ($(PLATFORM),LINUX)
    LINKFLAGS  += -lGL -lm -lpthread -ldl -lrt -lX11
else ifeq ($(PLATFORM),WEB)
    # Web (emscripten emcc) Flags
    # -----------------------------------------------------------------------------
    # -Os                         size optimization
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
    LINKFLAGS  += -L$(RAYLIB_LIB)/web --shell-file $(SRC_DIR)/shell.html \
    -sUSE_GLFW=3 -sFORCE_FILESYSTEM=1 -sASYNCIFY -sTOTAL_MEMORY=67108864 \
    -sEXPORTED_FUNCTIONS=_main,requestFullscreen -sEXPORTED_RUNTIME_METHODS=HEAPF32
endif

# Define output flags
ifeq ($(PLATFORM),WINDOWS)
    PLATFORM := DESKTOP
endif
ifeq ($(PLATFORM),LINUX)
    PLATFORM := DESKTOP
endif
ifeq ($(CC),cl)
    CFLAG_C := /c
    CFLAG_O := /Fe:
    OUTFLAG := $(CFLAG_O)$(OUTPUT)$(EXTENSION)
    PLATFLAG := /DPLATFORM_$(PLATFORM)
else
    CFLAG_C := -c
    CFLAG_O := -o
    OUTFLAG := $(CFLAG_O) $(OUTPUT)$(EXTENSION)
    PLATFLAG := -DPLATFORM_$(PLATFORM)
endif

# ==============================================================================
# Targets
# ==============================================================================

# MAKE SYNTAX NOTES
# ----------------------------------------------------
# target: dependency1 dependency2
# % = wildcard match between target and dependency
# $@ = target, $< = dependency1, $^ = all dependencies
# ----------------------------------------------------

# tell `make` that these aren't files
.PHONY: all clang msvc web gh-pages clang new clean

# (Default) Compile for desktop with no arguments/platform specified
all: $(OUTPUT)$(EXTENSION)

# Link object files into final executable
$(OUTPUT)$(EXTENSION): $(OBJS)
	$(CC) $(OUTFLAG) $^ $(LINKFLAGS)

# Compile c files to object files
$(SRC_DIR)/%$(OBJ_EXT): $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAG_C) $< $(CFLAG_O) $@ $(OPT_FLAGS) $(DEBUG_FLAGS) $(CFLAGS) $(PLATFLAG) $(CPPFLAGS)

# Build with clang
clang:
	$(MAKE) CC=clang

# Build with MSVC cl.exe
msvc:
	$(MAKE) CC=cl

# Build to web assembly with emscripten
web:
	$(MAKE) PLATFORM=WEB

# Build for upload to GitHub pages
# (Automated by GitHub workflow: .github/workflows/deploy.yaml)
gh-pages:
	@mkdir -p build_web
	$(MAKE) PLATFORM=WEB OUTPUT=build_web/index

# Build from scratch
new:
	$(MAKE) clean && $(MAKE)

# Clean up generated build files
clean:
	@rm -rf $(OUTPUT)$(EXTENSION) $(OBJS) \
	        $(OUTPUT).html $(OUTPUT).js $(OUTPUT).wasm build_web/ \
	        $(OUTPUT).ilk $(OUTPUT).pdb vc140.pdb *.rdi
	@echo "Make build files cleaned"

