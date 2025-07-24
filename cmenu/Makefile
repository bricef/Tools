.PHONY: run clean all install-deps

# Try to get raylib flags from pkg-config, fallback to manual flags if not found
RAYLIB_PKG_CONFIG := $(shell pkg-config --exists raylib && echo "yes" || echo "no")

ifeq ($(RAYLIB_PKG_CONFIG),yes)
    RAYLIB_FLAGS = $(shell pkg-config --libs --cflags raylib)
else
    RAYLIB_FLAGS = -lraylib
endif

# Always include necessary system libraries
CFLAGS = $(RAYLIB_FLAGS) -lGL -lm -lpthread -ldl -lrt -lX11
CFLAGS += -I./lib/
CFLAGS += -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable

all: target/cmenu

target/cmenu: main.c target
	gcc -o target/cmenu main.c $(CFLAGS) $(LIBFLAGS)	

target/cmenu-debug: main.c target
	gcc -g3 -o target/cmenu-debug main.c $(CFLAGS) $(LIBFLAGS)

debug: target/cmenu-debug
	lldb -f target/cmenu-debug -o run -o exit

run: target/cmenu
	./target/cmenu

clean:
	rm -rf target/

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