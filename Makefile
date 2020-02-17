all: build run

OBJS = framebuffer.c \
	stack.c \
	cpu.c \
	bcd.c

CC = gcc
CFLAGS = -Wall -Wextra
TERMINAL = gnome-terminal
NCURSES = -lncurses
INCLUDE = -Iinclude
DEBUG = -DDEBUG=1

build-debug:
	$(CC) \
	$(CFLAGS) \
	$(INCLUDE) \
	$(OBJS) \
	$(NCURSES) \
	$(DEBUG) \
	-o chip8-debug

build:
	$(CC) \
	$(CFLAGS) \
	$(INCLUDE) \
	$(OBJS) \
	$(NCURSES) \
	-o chip8

run-debug:
	$(TERMINAL) \
	--profile \
	--working-directory="$(CURDIR)" \
	--geometry=64x32 \
	--title=CHIP8 \
	-- "./chip8-debug" \

run:
	$(TERMINAL) \
	--profile \
	--working-directory="$(CURDIR)" \
	--geometry=64x32 \
	--title=CHIP8 \
	-- "./chip8" \

clean:
	rm chip8
