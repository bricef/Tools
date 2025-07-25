.PHONY: run clean all install-deps debug

# Try to get raylib flags from pkg-config, fallback to manual flags if not found
RAYLIB_PKG_CONFIG := $(shell pkg-config --exists raylib && echo "yes" || echo "no")

ifeq ($(RAYLIB_PKG_CONFIG),yes)
    RAYLIB_FLAGS = $(shell pkg-config --libs --cflags raylib)
else
    RAYLIB_FLAGS = -lraylib
endif

# Detect OS
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
    ifeq ($(DETECTED_OS),Darwin)
        DETECTED_OS := MacOS
    endif
    ifeq ($(DETECTED_OS),Linux)
        DETECTED_OS := Linux
    endif
endif

# OS specific flags
ifeq ($(DETECTED_OS),Windows)
    PLATFORM_FLAGS = -lopengl32 -lgdi32 -lwinmm
else ifeq ($(DETECTED_OS),MacOS) 
    PLATFORM_FLAGS = -framework OpenGL 
else ifeq ($(DETECTED_OS),Linux)
    PLATFORM_FLAGS = -lGL -lm -lpthread -ldl -lrt -lX11
endif


# Always include necessary system libraries
CFLAGS = $(RAYLIB_FLAGS) $(PLATFORM_FLAGS)
CFLAGS += -I./lib/ -I./lib/args/src/ -I./build/
CFLAGS += -Wall -Wextra -Werror --pedantic -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable


FILES = main.c config.c state.c utils.c

DEPS = lib/args/src/args.c


all: target/cmenu

target/cmenu: $(FILES) target fonts
	gcc -o $@ $(FILES) $(DEPS) $(CFLAGS) $(LIBFLAGS)	

target/cmenu-debug: $(FILES) target fonts
	gcc -g3 -o $@ $(FILES) $(DEPS) $(CFLAGS) $(LIBFLAGS)

debug: target/cmenu-debug
	lldb -f target/cmenu-debug -o run -o exit

fonts: build/font_mecha.h

build/font_%.h: fonts/%.png build 
	xxd -i < ${<} > $@.xxd
	NAME=font_$(subst fonts/,,$(basename ${<}));\
	echo "const unsigned char $$NAME []= {" > build/prefix; \
	echo "};" > build/suffix
	cat build/prefix ${@}.xxd build/suffix > $@
	rm build/prefix build/suffix build/*.xxd

run: target/cmenu
	./target/cmenu

clean:
	rm -rf target/
	rm -rf build/

build:
	mkdir -p build

target:
	mkdir -p target

install-deps:
	@echo "Installing raylib..."
	@if command -v pacman >/dev/null 2>&1; then \
		sudo pacman -S raylib; \
	elif command -v apt-get >/dev/null 2>&1; then \
		sudo apt-get install libraylib-dev; \
	elif command -v brew >/dev/null 2>&1; then \
		brew install raylib; \
	else \
		echo "Please install raylib manually for your system"; \
	fi