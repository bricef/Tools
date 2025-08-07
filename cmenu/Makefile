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

BUILD_DIR = build
TARGET_DIR = ${BUILD_DIR}/target

CC = gcc

# Always include necessary system libraries
CFLAGS = $(RAYLIB_FLAGS) $(PLATFORM_FLAGS)
CFLAGS += -I./lib/ -I./lib/args/src/ -I./${BUILD_DIR}/ -I./lib/lz4/lib/
CFLAGS += -Wall -Wextra -Werror --pedantic -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable

DEPFLAGS = 

OBJECT_FILES = ${BUILD_DIR}/args.o
STATIC_LIBS = lib/lz4/lib/liblz4.a

COMPRESS_CMD = lz4 -c -f -12

FILES = main.c config.c state.c utils.c press.c

DEPS = lib/args/src/args.c


all: ${TARGET_DIR}/cmenu

${BUILD_DIR}/args.o: lib/args/src/args.c
	$(CC) -c -o $@ $(DEPFLAGS) $<

lib/lz4/lib/liblz4.a:
	make -C lib/lz4	

${TARGET_DIR}/cmenu: $(FILES) target fonts $(OBJECT_FILES) $(STATIC_LIBS)
	$(CC) -O3 -o $@ $(FILES) $(OBJECT_FILES) $(STATIC_LIBS) $(CFLAGS) $(LIBFLAGS)	

${TARGET_DIR}/cmenu-debug: $(FILES) target fonts $(OBJECT_FILES) $(STATIC_LIBS)
	$(CC) -g3 -O0 -DDEBUG -o $@ $(FILES) $(OBJECT_FILES) $(STATIC_LIBS) $(CFLAGS) $(LIBFLAGS)

${TARGET_DIR}/cmenu-asan: $(FILES) target fonts $(OBJECT_FILES) $(STATIC_LIBS)
	$(CC) -g3 -O0 -fsanitize=address -fno-omit-frame-pointer -o $@ $(FILES) $(OBJECT_FILES) $(STATIC_LIBS) $(CFLAGS) $(LIBFLAGS)

debug: ${TARGET_DIR}/cmenu-debug
	lldb -f ${TARGET_DIR}/cmenu-debug  -o "process launch -i tests/basic.txt" -o exit

valgrind: ${TARGET_DIR}/cmenu-debug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt --suppressions=valgrind.supp ${TARGET_DIR}/cmenu-debug <tests/basic.txt

valgrind-quick: ${TARGET_DIR}/cmenu-debug
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --suppressions=valgrind.supp ./${TARGET_DIR}/cmenu-debug <tests/basic.txt

valgrind-errors: ${TARGET_DIR}/cmenu-debug
	valgrind --tool=memcheck --track-origins=yes --suppressions=valgrind.supp ${TARGET_DIR}/cmenu-debug <tests/basic.txt

leaks: ${TARGET_DIR}/cmenu-debug tests/basic.txt
	MallocStackLogging=1 leaks --atExit -- ${TARGET_DIR}/cmenu-debug  <tests/basic.txt 

asan-test: ${TARGET_DIR}/cmenu-asan tests/basic.txt
	${TARGET_DIR}/cmenu-asan <tests/basic.txt

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
	echo "// Compressed font for ${<}" > build/prefix; \
	echo "// Compressed with lz4 -c -f -12" >> build/prefix; \
	echo "const unsigned char $$NAME []= {" >> build/prefix; \
	echo "};" > build/suffix
	cat build/prefix ${@}.xxd build/suffix > $@
	rm build/prefix build/suffix build/*.xxd

run: ${TARGET_DIR}/cmenu
	${TARGET_DIR}/cmenu

clean:
	rm -rf ${TARGET_DIR}/
	rm -rf ${BUILD_DIR}/

build:
	mkdir -p ${BUILD_DIR}

target:
	mkdir -p ${TARGET_DIR}

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