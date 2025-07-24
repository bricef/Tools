.PHONY: run clean

CFLAGS = $(shell pkg-config --libs --cflags raylib)
CFLAGS += -I./lib/
CFLAGS += -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable


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