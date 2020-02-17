#include "framebuffer.h"

int main()
{
    CPU *cpu = new_cpu();

    // Dummy Values
    cpu->v0 = 125;
    cpu->v1 = 200;

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

    mvwprintw(win, 0, 26, "DEBUG CHIP8");

    /* Left Column */
    mvwprintw(win, 2, 1, "V0: %d", cpu->v0);
    mvwprintw(win, 3, 1, "V1: %d", cpu->v1);
    mvwprintw(win, 4, 1, "V2: %d", cpu->v2);
    mvwprintw(win, 5, 1, "V3: %d", cpu->v3);
    mvwprintw(win, 6, 1, "V4: %d", cpu->v4);
    mvwprintw(win, 7, 1, "V5: %d", cpu->v5);
    mvwprintw(win, 8, 1, "V6: %d", cpu->v6);
    mvwprintw(win, 9, 1, "V7: %d", cpu->v7);
    mvwprintw(win, 10, 1, "V8: %d", cpu->v8);
    mvwprintw(win, 11, 1, "V9: %d", cpu->v9);
    mvwprintw(win, 12, 1, "VA: %d", cpu->va);
    mvwprintw(win, 13, 1, "VB: %d", cpu->vb);
    mvwprintw(win, 14, 1, "VC: %d", cpu->vc);
    mvwprintw(win, 15, 1, "VD: %d", cpu->vd);
    mvwprintw(win, 16, 1, "VE: %d", cpu->ve);
    mvwprintw(win, 17, 1, "VF: %d", cpu->vf);

    mvwprintw(win, 19, 1, "ST: %d", cpu->st);
    mvwprintw(win, 20, 1, "DT: %d", cpu->dt);

    /* Right Column */
    // 32 for half of the framebuffer width
    mvwprintw(win, 2, 32, "SP: %d", cpu->stack->sp);
    mvwprintw(win, 3, 32, "PC: %d", cpu->stack->pc);

    refresh();
    wrefresh(win);
}
#endif