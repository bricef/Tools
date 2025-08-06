.PHONY: run clean all install-deps debug valgrind valgrind-quick valgrind-errors leaks asan-test

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


TTF_FILES = $(wildcard fonts/*.ttf)
FONT_FILES = $(patsubst fonts/%.ttf,build/font_%.h,$(TTF_FILES))
COMPRESSED_FONT_FILES = $(patsubst fonts/%.ttf,build/compressed_font_%.h,$(TTF_FILES))


# Always include necessary system libraries
CFLAGS = $(RAYLIB_FLAGS) $(PLATFORM_FLAGS)
CFLAGS += -I./lib/ -I./lib/args/src/ -I./build/ -I./lib/lz4/lib/
CFLAGS += -Wall -Wextra -Werror --pedantic -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable

DEPFLAGS = 

OBJECT_FILES = build/args.o
STATIC_LIBS = lib/lz4/lib/liblz4.a

COMPRESS_CMD = lz4 -1

FILES = main.c config.c state.c utils.c press.c

DEPS = lib/args/src/args.c


all: target/cmenu

build/args.o: lib/args/src/args.c
	gcc -c -o $@ $(DEPFLAGS) $<

lib/lz4/lib/liblz4.a:
	make -C lib/lz4	

target/cmenu: $(FILES) target fonts $(OBJECT_FILES) $(STATIC_LIBS)
	gcc -o $@ $(FILES) $(OBJECT_FILES) $(STATIC_LIBS) $(CFLAGS) $(LIBFLAGS)	

target/cmenu-debug: $(FILES) target fonts $(OBJECT_FILES) $(STATIC_LIBS)
	gcc -g3 -O0 -DDEBUG -o $@ $(FILES) $(OBJECT_FILES) $(STATIC_LIBS) $(CFLAGS) $(LIBFLAGS)

target/cmenu-asan: $(FILES) target fonts $(OBJECT_FILES) $(STATIC_LIBS)
	gcc -g3 -O0 -fsanitize=address -fno-omit-frame-pointer -o $@ $(FILES) $(OBJECT_FILES) $(STATIC_LIBS) $(CFLAGS) $(LIBFLAGS)

debug: target/cmenu-debug
	lldb -f target/cmenu-debug  -o "process launch -i tests/basic.txt" -o exit

valgrind: target/cmenu-debug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt --suppressions=valgrind.supp ./target/cmenu-debug <tests/basic.txt

valgrind-quick: target/cmenu-debug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --suppressions=valgrind.supp ./target/cmenu-debug <tests/basic.txt

valgrind-errors: target/cmenu-debug
	valgrind --tool=memcheck --track-origins=yes --suppressions=valgrind.supp ./target/cmenu-debug <tests/basic.txt

leaks: target/cmenu-debug tests/basic.txt
	MallocStackLogging=1 leaks --atExit -- ./target/cmenu-debug  <tests/basic.txt 

asan-test: target/cmenu-asan tests/basic.txt
	./target/cmenu-asan <tests/basic.txt

fonts: $(FONT_FILES) $(COMPRESSED_FONT_FILES)

build/font_%.h: fonts/%.ttf build 
	cat ${<} | xxd -i > $@.xxd
	NAME=font_$(subst fonts/,,$(basename ${<}));\
	echo "const unsigned char $$NAME []= {" > build/prefix; \
	echo "};" > build/suffix
	cat build/prefix ${@}.xxd build/suffix > $@
	rm build/prefix build/suffix build/*.xxd

build/compressed_font_%.h: fonts/%.ttf build 
	cat ${<} | ${COMPRESS_CMD} | xxd -i > $@.xxd
	NAME=compressed_font_$(subst fonts/,,$(basename ${<}));\
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