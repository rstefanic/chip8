#include "chip8.h"

#define TOTAL_FRAMEBUFFER_SIZE FRAME_WIDTH * FRAME_HEIGHT

typedef struct {
    char buffer[FRAME_WIDTH * FRAME_HEIGHT];
} Framebuffer;


void draw_buffer(Framebuffer *fb);
void clear_buffer(Framebuffer *fb);
