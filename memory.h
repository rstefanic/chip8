#include "chip8.h"

typedef struct {
    /* 4k memory */
    char memory[BYTE_SIZE * 4000];
} Memory;
