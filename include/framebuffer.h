#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "chip8.h"

#define TOTAL_FRAMEBUFFER_SIZE FRAME_WIDTH * FRAME_HEIGHT

typedef struct {
    char buffer[FRAME_WIDTH * FRAME_HEIGHT];
} Framebuffer;

Framebuffer* new_framebuffer();
void draw_buffer(Framebuffer *fb);
void clear_buffer(Framebuffer *fb);

#endif /* FRAMEBUFFER_H not defined */