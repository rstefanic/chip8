#include "chip8.h"
#include "cpu.h"
#include "keypad.h"

char debug_print[64];
void debug_output(WINDOW *win, CPU *cpu);
int valid_keyboard_input(char c);

int main(int argc, char** argv)
{
    int debug = 0;
    int argp = 1;

    if (argc < 2 || argc > 3) {
        printf(INVALID_ARG_MSG);
        return -1;
    }

    if (strcmp(argv[argp], DEBUG_ARG_FLAG) == 0) {
         debug = 1;
         argp++;
    }

    if (argv[argp] == NULL) {
        printf(INVALID_ARG_MSG);
        return -1;
    }

    const char * const program_name = argv[argp];
    CPU *cpu = new_cpu(program_name);

    if (cpu == NULL) {
        printf(COULD_NOT_ALLOCATE_CPU_MEM);
        return -1;
    }

    initscr(); // set up memory and clears the screen for ncurses

    /* Put the terminal into raw mode; characters typed are immediately
     * pass through to the user program. This also means that interrupt,
     * quit, suspend, and flow control characters are passed through
     * uninterrupted */
    raw(); 

    int main_start_x = 0;
    int main_start_y = 0;
    int debug_start_x = 65;
    int debug_start_y = 0;

    WINDOW *main_win = newwin(FRAME_HEIGHT, FRAME_WIDTH, main_start_y, main_start_x);
    WINDOW *debug_win = newwin(FRAME_HEIGHT, FRAME_WIDTH, debug_start_y, debug_start_x);

    refresh();
    wrefresh(main_win);

    if (debug) {
        box(debug_win, 0, 0);
        refresh();
        wrefresh(debug_win);
    }

    curs_set(FALSE);
    noecho();
    nodelay(stdscr, TRUE);

    if (has_colors() == FALSE) {
        endwin();
        printf("ERR: This terminal does not support colors.");
        exit(1);
    }

    start_color();
    init_pair(PIXEL_OFF, COLOR_WHITE, COLOR_BLACK);
    init_pair(PIXEL_ON, COLOR_WHITE, COLOR_GREEN);

    listen_for_keypresses(cpu->keypad);

    unsigned int clock_cycles = 0;
    struct timespec clock_cycle_timer;
    clock_gettime(CLOCK_REALTIME, &clock_cycle_timer);

    for(;;) {
        clock_cycles++;

        // To simluate running the emulator at 500 Hz, we need to check every
        // 8 clock cycles to see how long the execute took so we can sleep
        // for 1/60th of a second so it feels like a 60 Hz timer cycle.
        if ((clock_cycles % 8) == 0) {

            // Find how long execute time was between the last and current cycle.
            struct timespec now;
            clock_gettime(CLOCK_REALTIME, &now);
            struct timespec sleep_time = {
                .tv_sec = 0,
                .tv_nsec = ONE_SIXTIETH_OF_A_SECOND_IN_NS - (clock_cycle_timer.tv_nsec - now.tv_nsec)
            };

            // Sleep to make up the difference
            nanosleep(&sleep_time, NULL);

            // Reset clock_cycle_timer
            clock_gettime(CLOCK_REALTIME, &clock_cycle_timer);

            // Handle the CPU processes that only occur 60 times
            // per second such as decrementing the timers and
            // drawing the window buffer if the flag's set
            if (cpu->dt > 0) {
                decrement_dt(cpu);
            }

            if (cpu->st > 0) {
                beep();
                decrement_st(cpu);
            }

            if (cpu->draw_flag) {
                draw_buffer(main_win, cpu->fb);
                clear_draw_flag(cpu);
                wrefresh(main_win);
            }
        }

        unsigned short op_code = fetch(cpu);
        Instruction* ins = decode(op_code);

        if (ins == NULL) {
            printf("ERR: could not allocate memory for instruction");
            return -1;
        }

        if (debug) {
            sprintf(debug_print, "Current Instruction: [%s]", get_op_string(ins->op));
        }

        execute(cpu, ins);
        free(ins);

        if (debug) {
            debug_output(debug_win, cpu);
            wrefresh(debug_win);
        }
    }

    // Teardown
    endwin();
    clean_cpu(cpu);
    return 0;
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

    /* Keypad */
    mvwprintw(win, 14, 32, "Keypad");

    /* First Row */
    mvwprintw(win, 16, 32, "%c", cpu->keypad->keys[0x01] ? '1' : '-');
    mvwprintw(win, 16, 34, "%c", cpu->keypad->keys[0x02] ? '2' : '-');
    mvwprintw(win, 16, 36, "%c", cpu->keypad->keys[0x03] ? '3' : '-');
    mvwprintw(win, 16, 38, "%c", cpu->keypad->keys[0x0C] ? '4' : '-');

    /* Second Row */
    mvwprintw(win, 18, 32, "%c", cpu->keypad->keys[0x04] ? 'Q' : '-');
    mvwprintw(win, 18, 34, "%c", cpu->keypad->keys[0x05] ? 'W' : '-');
    mvwprintw(win, 18, 36, "%c", cpu->keypad->keys[0x06] ? 'E' : '-');
    mvwprintw(win, 18, 38, "%c", cpu->keypad->keys[0x0D] ? 'R' : '-');

    /* Third Row */
    mvwprintw(win, 20, 32, "%c", cpu->keypad->keys[0x07] ? 'A' : '-');
    mvwprintw(win, 20, 34, "%c", cpu->keypad->keys[0x08] ? 'S' : '-');
    mvwprintw(win, 20, 36, "%c", cpu->keypad->keys[0x09] ? 'D' : '-');
    mvwprintw(win, 20, 38, "%c", cpu->keypad->keys[0x0E] ? 'F' : '-');

    /* Fourth Row */
    mvwprintw(win, 22, 32, "%c", cpu->keypad->keys[0x0A] ? 'Z' : '-');
    mvwprintw(win, 22, 34, "%c", cpu->keypad->keys[0x00] ? 'X' : '-');
    mvwprintw(win, 22, 36, "%c", cpu->keypad->keys[0x0B] ? 'C' : '-');
    mvwprintw(win, 22, 38, "%c", cpu->keypad->keys[0x0F] ? 'V' : '-');

    mvwprintw(win, 28, 2, "DEBUG_PRINT: %s", debug_print);
}

int valid_keyboard_input(char c)
{
    switch(c) {
        case '1':
        case '2':
        case '3':
        case '4':
        case 'q':
        case 'w':
        case 'e':
        case 'r':
        case 'a':
        case 's':
        case 'd':
        case 'f':
        case 'z':
        case 'x':
        case 'c':
        case 'v':
            return 1;
        default:
            return 0;
    }
}
