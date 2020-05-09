#include "framebuffer.h"

Framebuffer* new_framebuffer()
{
    Framebuffer *framebuffer;
    framebuffer = (Framebuffer*)calloc(TOTAL_FRAMEBUFFER_SIZE, sizeof(Framebuffer));

    // There's no need to check for NULL here because the
    // checks the return of this function for NULL
    return framebuffer;
}

void draw_buffer(Framebuffer *fb)
{
    clear();

    for(int y = 0; y < FRAME_HEIGHT; y++) {
        for (int x = 0; x < FRAME_WIDTH; x++) {
            if ((fb->buffer[x + (y * FRAME_WIDTH)] & 0x01) == 0x01) {
                attron(COLOR_PAIR(PIXEL_ON));
                addch(' ');
                attroff(COLOR_PAIR(PIXEL_ON));
            }
            else {
                attron(COLOR_PAIR(PIXEL_OFF));
                addch(' ');
                attroff(COLOR_PAIR(PIXEL_OFF));
            }
        }
    }
}

void clear_buffer(Framebuffer *fb) 
{
    for (int i = 0; i < TOTAL_FRAMEBUFFER_SIZE; i++) {
        fb->buffer[i] = 0;
    }
}