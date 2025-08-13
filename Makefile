# Credit to raylib's makefiles for many pointers and ideas

# Tell `make` that these aren't files
.PHONY: all web web-release clean

# =============================================================================
# PLATFORM DETECTION AND SETUP
# =============================================================================

# Detect platform and set variables accordingly
PLATFORM_DESKTOP = 1
ifeq ($(OS),Windows_NT)
    PLATFORM = WINDOWS
    EXTENSION = .exe
    OBJ_EXT = .obj
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = LINUX
    endif
    EXTENSION =
    OBJ_EXT = .o
endif

# =============================================================================
# DEFINE ENVIRONMENT VARIABLES:
# =============================================================================

# Source files to compile
CODE = $(wildcard code/*.c)
HEADERS = $(wildcard code/*.h)

# # Object files for intermediate compilation
OBJS = $(CODE:.c=$(OBJ_EXT))

# Name of output executable
OUTPUT = game

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
# -O2                        # optimization level 2
# -sUSE_GLFW=3              # Use glfw3 library (context/input management)
# -sALLOW_MEMORY_GROWTH=1   # to allow memory resizing -> WARNING: Audio buffers could FAIL!
# -sTOTAL_MEMORY=16777216   # to specify heap memory size (default = 16MB) (67108864 = 64MB)
# -sUSE_PTHREADS=1          # multithreading support
# -sWASM=0                  # disable Web Assembly, emitted by default
# -sASYNCIFY                # lets synchronous C/C++ code interact with asynchronous JS
# -sFORCE_FILESYSTEM=1      # force filesystem to load/save files data
# -sASSERTIONS=1            # enable runtime checks for common memory allocation errors (-O1 and above turn it off)
# -sGL_ENABLE_GET_PROC_ADDRESS # enable using the *glGetProcAddress() family of functions, required for extensions loading
# -sEXPORTED_FUNCTIONS       # export needed functions (only for newer versions of emscripten to help reduce filesize)
# --profiling                # include information for code profiling
# --memory-init-file 0       # to avoid an external memory initialization code file (.mem)
# --preload-file resources   # specify a resources folder for data compilation
# --source-map-base          # allow debugging in browser with source map
WEBFLAGS = -sUSE_GLFW=3 -sFORCE_FILESYSTEM=1 -sASYNCIFY -DPLATFORM_WEB -sEXPORTED_FUNCTIONS=_main,requestFullscreen
# Use shell file for Web compilation page layout
WEBFLAGS += --shell-file code/shell.html

# Local raylib location
RAYLIB_INC = raylib/include
RAYLIB_LIB = raylib/lib

# Include and library flags (more libraries added below)
INCLUDES = -I$(RAYLIB_INC)
LIBS = -lraylib

# =============================================================================
# COMPILER AND LIBRARY SETUP
# =============================================================================

# Detect compiler (clang or gcc expected, but cl.exe works with 'make msvc')
CC ?= gcc

# Platform-specific library flags
ifeq ($(PLATFORM),WINDOWS)
    LIBS += -L$(RAYLIB_LIB)/windows
    LIBS += -lopengl32 -lgdi32 -lwinmm
else ifeq ($(PLATFORM),LINUX)
    LIBS += -lGL -lm -lpthread -ldl -lrt -lX11
endif

# =============================================================================
# MAKEFILE TARGETS
# =============================================================================

# NOTE: make alias syntax reminder
# target: dependency1 dependency2
# % = wildcard match for target and dependency
# $@ = target, $< = dependency1, $^ = all dependencies

all: $(OUTPUT)$(EXTENSION)

# Compile for desktop (default)
$(OUTPUT)$(EXTENSION): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) -DPLATFORM_DESKTOP

# Rule to compile individual .c files to object files
code/%$(OBJ_EXT): code/%.c $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES) -DPLATFORM_DESKTOP

# Use the MSVC C compiler (useful for getting .pdb debug info)
# TODO: maybe auto-detect cl instead and move this into "Platform-specific" section?
msvc: CFLAGS = /Fo:code\\ /Od /W3 /MD /Zi
msvc: LIBS = /link /DEBUG /LIBPATH:".\raylib\lib\windows-msvc" raylib.lib \
    opengl32.lib gdi32.lib winmm.lib user32.lib shell32.lib
msvc:
	cl /Fe:$(OUTPUT)$(EXTENSION) $(CODE) $(CFLAGS) /I".\raylib\include" $(LIBS)

# Compile for Web:
# Use a local copy of raylib's web library in case it isn't available
web: LIBS = -lraylib -L$(RAYLIB_LIB)/web
web: $(CODE) $(HEADERS)
	emcc -o $(OUTPUT).html $(CODE) $(WEBFLAGS) $(INCLUDES) $(LIBS)

# For deploying to GitHub Pages
# See the related workflow file here: .github\workflows\deploy.yaml
web-release: LIBS = -lraylib -L$(RAYLIB_LIB)/web
web-release: $(CODE) $(HEADERS)
	@mkdir -p build_web
	emcc -o build_web/index.html $(CODE) $(WEBFLAGS) -O2 $(INCLUDES) $(LIBS)

# Clean up old build files
clean:
	@rm -rf $(OUTPUT)$(EXTENSION) $(OBJS) \
	$(OUTPUT).html $(OUTPUT).js $(OUTPUT).wasm build_web/ \
	$(OUTPUT).ilk $(OUTPUT).pdb vc140.pdb *.rdi \
	@echo "Make build files cleaned"
