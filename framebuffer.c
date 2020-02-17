#include "framebuffer.h"
#include "memory.h"
#include "cpu.h"
#include "bcd.h"

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
    box(win, 0, 0);
    refresh();      // Refresh 
    wrefresh(win);  // Refresh the window
    getch();

#if DEBUG
    mvwprintw(win, 1, 1, "DEBUG Decrement");
#else
    mvwprintw(win, 1, 1, "Before Decrement");
#endif

    mvwprintw(win, 2, 1, "DT: %d", cpu->dt);
    mvwprintw(win, 3, 1, "ST: %d", cpu->st);
    mvwprintw(win, 4, 1, "V0: %d", cpu->v0);
    mvwprintw(win, 5, 1, "V1: %d", cpu->v1);

    mvwprintw(win, 7, 1, "V0 hundreds: %d", bcd_hundreds_at(cpu->v0));
    mvwprintw(win, 8, 1, "V0 tens: %d", bcd_tens_at(cpu->v0));
    mvwprintw(win, 9, 1, "V0 ones: %d", bcd_ones_at(cpu->v0));

    mvwprintw(win, 11, 1, "V1 hundreds: %d", bcd_hundreds_at(cpu->v1));
    mvwprintw(win, 12, 1, "V1 tens: %d", bcd_tens_at(cpu->v1));
    mvwprintw(win, 13, 1, "V1 ones: %d", bcd_ones_at(cpu->v1));
    refresh();
    wrefresh(win);
    getch();

    decrement_dt(cpu);
    decrement_st(cpu);
    mvwprintw(win, 1, 1, "After Decrement");
    mvwprintw(win, 2, 1, "DT: %d", cpu->dt);
    mvwprintw(win, 3, 1, "ST: %d", cpu->st);
    refresh();
    wrefresh(win);

    // To disable the buffering of typed characters by the TTY driver
    // cbreak();

    // To suppress the autmoatic echoing of typed characters
    // noecho();

    // refresh the screen to match what's in memory
    getch();
    refresh();

    endwin(); // exists ncurses
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