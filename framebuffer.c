#include "framebuffer.h"

Framebuffer* new_framebuffer()
{
    Framebuffer *framebuffer;
    framebuffer = (Framebuffer*)malloc(sizeof(Framebuffer));

    // There's no need to check for NULL here because the
    // checks the return of this function for NULL
    return framebuffer;
}

void clear_buffer(Framebuffer *fb) 
{
    for (int i = 0; i < TOTAL_FRAMEBUFFER_SIZE; i++) {
        fb->buffer[i] = 0;
    }
}