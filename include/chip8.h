#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FRAME_WIDTH 64
#define FRAME_HEIGHT 32

/* Size in bits */
#define BYTE_SIZE 8
#define END_FONT_SEGMENT 0x080
#define PROG_DATA_SEGMENT 0x200
#define MAX_MEMORY 0xFFF
