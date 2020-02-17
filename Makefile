all: build run

OBJS = framebuffer.c \
	stack.c \
	cpu.c

CFLAGS = -Wall -Wextra
TERMINAL = gnome-terminal
NCURSES = -lncurses

build:
	gcc $(CFLAGS) $(OBJS) $(NCURSES) -o chip8

run:
	$(TERMINAL) \
	--profile \
	--working-directory="$(CURDIR)" \
	--geometry=64x32 \
	--title=CHIP8 \
	-- "./chip8" \

clean:
	rm chip8
