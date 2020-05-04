#ifndef CHIP8_H
#define CHIP8_H

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Framebuffer sizes */
#define FRAME_WIDTH 64
#define FRAME_HEIGHT 32

/* Memory Sizes */
#define BYTE_SIZE 8
#define END_FONT_SEGMENT 0x080
#define PROG_DATA_SEGMENT 0x200
#define MAX_MEMORY 0xFFF

/* Used for bit manipulation on the CPU */
#define ONE_NIBBLE 4
#define TWO_NIBBLES 8

#define NO_KEYBOARD_INPUT 0x10

/* Constant Strings */
#define INVALID_ARG_MSG "ERR: Emulator requires a program to load"
#define COULD_NOT_ALLOCATE_CPU_MEM "ERR: Could not allocate CPU memory"
#define DEBUG_ARG_FLAG "-d"

#endif /* CHIP8_H not defined */