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
    // testing fetch_op
    unsigned int offset = PROG_DATA_SEGMENT;
    cpu->memory[offset] = 0x11;
    cpu->pc = &(cpu->memory[offset]);
    offset++;
    cpu->memory[offset] = 0x22;
    offset++;
    cpu->i = fetch_op(cpu);

    setup_debug_output(win);
    debug_output(win, cpu);
    getch();

    cpu->memory[offset] = 0x33;
    offset++;
    cpu->memory[offset] = 0xff;
    offset++;
    cpu->i = fetch_op(cpu);

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

    mvwprintw(win, 0, 26, " DEBUG CHIP8 ");

    /* Left Column */
    mvwprintw(win, 2, 2, "V0: 0x%.2x (%d)", cpu->v0, cpu->v0);
    mvwprintw(win, 3, 2, "V1: 0x%.2x (%d)", cpu->v1, cpu->v1);
    mvwprintw(win, 4, 2, "V2: 0x%.2x (%d)", cpu->v2, cpu->v2);
    mvwprintw(win, 5, 2, "V3: 0x%.2x (%d)", cpu->v3, cpu->v3);
    mvwprintw(win, 6, 2, "V4: 0x%.2x (%d)", cpu->v4, cpu->v4);
    mvwprintw(win, 7, 2, "V5: 0x%.2x (%d)", cpu->v5, cpu->v5);
    mvwprintw(win, 8, 2, "V6: 0x%.2x (%d)", cpu->v6, cpu->v6);
    mvwprintw(win, 9, 2, "V7: 0x%.2x (%d)", cpu->v7, cpu->v7);
    mvwprintw(win, 10, 2, "V8: 0x%.2x (%d)", cpu->v8, cpu->v8);
    mvwprintw(win, 11, 2, "V9: 0x%.2x (%d)", cpu->v9, cpu->v9);
    mvwprintw(win, 12, 2, "VA: 0x%.2x (%d)", cpu->va, cpu->va);
    mvwprintw(win, 13, 2, "VB: 0x%.2x (%d)", cpu->vb, cpu->vb);
    mvwprintw(win, 14, 2, "VC: 0x%.2x (%d)", cpu->vc, cpu->vc);
    mvwprintw(win, 15, 2, "VD: 0x%.2x (%d)", cpu->vd, cpu->vd);
    mvwprintw(win, 16, 2, "VE: 0x%.2x (%d)", cpu->ve, cpu->ve);
    mvwprintw(win, 17, 2, "VF: 0x%.2x (%d)", cpu->vf, cpu->vf);

    mvwprintw(win, 19, 2, "ST: 0x%.2x (%d)", cpu->st, cpu->st);
    mvwprintw(win, 20, 2, "DT: 0x%.2x (%d)", cpu->dt, cpu->dt);

    /* Right Column */
    // 32 for half of the framebuffer width
    mvwprintw(win, 2, 32, "PC: 0x%.2x", cpu->pc);
    mvwprintw(win, 3, 32, "IR: 0x%.4x", cpu->i);
    mvwprintw(win, 4, 32, "SP: 0x%.2x (%d)", cpu->stack->sp, cpu->stack->sp);

    mvwprintw(win, 30, 2, "Press any key to continue execution... ");

    refresh();
    wrefresh(win);
}
#endif