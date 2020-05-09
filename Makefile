all: build debug

CC = gcc
CFLAGS = -Wall -Wextra
TERMINAL = gnome-terminal
NCURSES = -lncurses
INCLUDE = -Iinclude
DEBUG = -d
TESTROM = ./roms/pong.ch8

SRC = $(wildcard *.c)
OBJS = $(SRC:%.c=obj/%.o)

obj/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

build: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(NCURSES) -o chip8

debug:
	$(TERMINAL) \
	--profile \
	--working-directory="$(CURDIR)" \
	--geometry=129x32 \
	--title=CHIP8 \
	-- "./chip8" $(DEBUG) $(TESTROM) \

run:
	$(TERMINAL) \
	--profile \
	--working-directory="$(CURDIR)" \
	--geometry=64x32 \
	--title=CHIP8 \
	-- "./chip8" $(TESTROM) \

clean:
	rm -rf obj/*.o
	rm -f chip8
