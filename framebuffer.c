#include "framebuffer.h"

int main()
{
    CPU *cpu = new_cpu();

    // Dummy Values
    cpu->v0 = 125;
    cpu->v1 = 200;

    initialize_rng();

    initscr(); // set up memory and clears the screen for ncurses

    /* Put the terminal into raw mode; characters typed are immediately
     * pass through to the user program. This also means that interrupt,
     * quit, suspend, and flow control characters are passed through
     * uninterrupted */
    raw(); 

    int start_x = 0;
    int start_y = 0;

    WINDOW *win = newwin(FRAME_HEIGHT, FRAME_WIDTH, start_y, start_x);

#if DEBUG
    setup_debug_output(win);
    debug_output(win, cpu);
    getch();

    decrement_dt(cpu);
    decrement_st(cpu);
    cpu->v8 = rng();
    debug_output(win, cpu);
    getch();
#else
    box(win, 0, 0);
    mvwprintw(win, 1, 1, "CHIP8");
    wrefresh(win);
    getch();
#endif

    endwin(); // exists ncurses
    free(cpu->stack);
    free(cpu);
    return 0;
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

#if DEBUG
void setup_debug_output(WINDOW *win)
{
    box(win, 0, 0);
    refresh();
    wrefresh(win);
}

void debug_output(WINDOW *win, CPU *cpu)
{
    werase(win);
    box(win, 0, 0);

    mvwprintw(win, 0, 26, "DEBUG CHIP8");

    /* Left Column */
    mvwprintw(win, 2, 2, "V0: %d", cpu->v0);
    mvwprintw(win, 3, 2, "V1: %d", cpu->v1);
    mvwprintw(win, 4, 2, "V2: %d", cpu->v2);
    mvwprintw(win, 5, 2, "V3: %d", cpu->v3);
    mvwprintw(win, 6, 2, "V4: %d", cpu->v4);
    mvwprintw(win, 7, 2, "V5: %d", cpu->v5);
    mvwprintw(win, 8, 2, "V6: %d", cpu->v6);
    mvwprintw(win, 9, 2, "V7: %d", cpu->v7);
    mvwprintw(win, 10, 2, "V8: %d", cpu->v8);
    mvwprintw(win, 11, 2, "V9: %d", cpu->v9);
    mvwprintw(win, 12, 2, "VA: %d", cpu->va);
    mvwprintw(win, 13, 2, "VB: %d", cpu->vb);
    mvwprintw(win, 14, 2, "VC: %d", cpu->vc);
    mvwprintw(win, 15, 2, "VD: %d", cpu->vd);
    mvwprintw(win, 16, 2, "VE: %d", cpu->ve);
    mvwprintw(win, 17, 2, "VF: %d", cpu->vf);

    mvwprintw(win, 19, 2, "ST: %d", cpu->st);
    mvwprintw(win, 20, 2, "DT: %d", cpu->dt);

    /* Right Column */
    // 32 for half of the framebuffer width
    mvwprintw(win, 2, 32, "SP: %d", cpu->stack->sp);
    mvwprintw(win, 3, 32, "PC: %d", cpu->stack->pc);

    refresh();
    wrefresh(win);
}
#endif