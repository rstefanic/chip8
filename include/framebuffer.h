#include "chip8.h"
#include "cpu.h"

#define TOTAL_FRAMEBUFFER_SIZE FRAME_WIDTH * FRAME_HEIGHT

typedef struct {
    char buffer[FRAME_WIDTH * FRAME_HEIGHT];
} Framebuffer;


void draw_buffer(Framebuffer *fb);
void clear_buffer(Framebuffer *fb);

#if DEBUG
void setup_debug_output(WINDOW *win);
void debug_output(WINDOW *win, CPU *cpu);
#endif
