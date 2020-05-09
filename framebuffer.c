#include "framebuffer.h"

Framebuffer* new_framebuffer()
{
    Framebuffer *framebuffer;
    framebuffer = (Framebuffer*)calloc(TOTAL_FRAMEBUFFER_SIZE, sizeof(Framebuffer));

    // There's no need to check for NULL here because the
    // checks the return of this function for NULL
    return framebuffer;
}

void draw_buffer(WINDOW *win, Framebuffer *fb)
{
    werase(win);

    for (int y = 0; y < FRAME_HEIGHT; y++) {
        for (int x = 0; x < FRAME_WIDTH; x++) {
            if ((fb->buffer[x + (y * FRAME_WIDTH)] & 0x01) == 0x01) {
                wattron(win, COLOR_PAIR(PIXEL_ON));
                waddch(win, ' ');
                wattroff(win, COLOR_PAIR(PIXEL_ON));
            }
            else {
                wattron(win, COLOR_PAIR(PIXEL_OFF));
                waddch(win, ' ');
                wattroff(win, COLOR_PAIR(PIXEL_OFF));
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