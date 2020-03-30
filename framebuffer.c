#include "framebuffer.h"

Framebuffer* new_framebuffer()
{
    Framebuffer *framebuffer;
    framebuffer = (Framebuffer*)malloc(sizeof(Framebuffer));

    // There's no need to check for NULL here because the
    // checks the return of this function for NULL
    return framebuffer;
}

void draw_buffer(Framebuffer *fb) 
{
    for(int i = 0; i < FRAME_HEIGHT; i++) {
        int offset = FRAME_WIDTH * i;

        for (register int j = 0; j < FRAME_WIDTH; j++) {
            addch(fb->buffer[offset + j]);
        }
    }
}

void clear_buffer(Framebuffer *fb) 
{
    for (int i = 0; i < TOTAL_FRAMEBUFFER_SIZE; i++) {
        fb->buffer[i] = ' ';
    }
}