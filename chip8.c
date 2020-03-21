#include "chip8.h"
#include "cpu.h"

#if DEBUG
char debug_print[64];
void setup_debug_output(WINDOW *win);
void debug_output(WINDOW *win, CPU *cpu);
#endif

int main()
{
    CPU *cpu = new_cpu("./roms/add.ch8");

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

    // fetch, decode, execute
    int i = 0;

    sprintf(debug_print, "i: %d", i);
    debug_output(win, cpu);
    getch();

    do {
        cpu->i = fetch(cpu);
        Instruction* ins = decode(cpu->i);
        execute(cpu, ins);

        sprintf(debug_print, "Current Instruction: [%s]", get_op_string(ins->op));
        free(ins);
        debug_output(win, cpu);
        getch();
        i++;
    } while(i < 10);
    
#else
    box(win, 0, 0);
    mvwprintw(win, 1, 1, "CHIP8");
    wrefresh(win);
    getch();
#endif

    endwin(); // exists ncurses

    // Clean up CPU
    // TODO(robert): Move this to the CPU in a CPU clean up function
    free(cpu->stack);
    free(cpu->fb);
    free(cpu);
    return 0;
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
    mvwprintw(win, 6, 32, "MemCheck @ 0x%.2x : 0x%.2x",
        0x00,
        cpu->memory[0x00]);
    mvwprintw(win, 7, 32, "MemCheck @ 0x%.2x : 0x%.2x",
        END_FONT_SEGMENT,
        cpu->memory[END_FONT_SEGMENT]);
    mvwprintw(win, 8, 32, "MemCheck @ 0x%.2x : 0x%.2x",
        PROG_DATA_SEGMENT,
        cpu->memory[PROG_DATA_SEGMENT]);
    mvwprintw(win, 9, 32, "MemCheck @ 0x%.2x : 0x%.2x",
        PROG_DATA_SEGMENT + 1,
        cpu->memory[PROG_DATA_SEGMENT + 1]);
    mvwprintw(win, 11, 32, "REGCHECK: %d", (Register)4);

    mvwprintw(win, 28, 2, "DEBUG_PRINT: %s", debug_print);
    mvwprintw(win, 30, 2, "Press any key to continue execution... ");

    refresh();
    wrefresh(win);
}
#endif