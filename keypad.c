#include "keypad.h"
#include <curses.h>
#include <time.h>

Keypad *new_keypad()
{
    Keypad *keypad = (Keypad*)malloc(sizeof(Keypad));
    clear_keypresses(keypad);

    return keypad;
}

char poll_keypress(Keypad *keypad, char c)
{
    return keypad->keys[c];
}

void clear_keypresses(Keypad *keypad)
{
    for (int i = 0; i < KEYPAD_SIZE; i++) {
        keypad->keys[i] = 0;
    }
}

void *handle_keypresses(void *ptr)
{
    Keypad *keypad;
    keypad = (Keypad*)ptr;
    clock_t start = clock();

    for(;;) {
        char ch;
        while ((ch = getch()) != ERR) {
            char keycode = -1;
            switch(ch) {
                // First Row
                case '1': keycode = 0x01; break;
                case '2': keycode = 0x02; break;
                case '3': keycode = 0x03; break;
                case '4': keycode = 0x0C; break;

                // Second Row
                case 'q': keycode = 0x04; break;
                case 'w': keycode = 0x05; break;
                case 'e': keycode = 0x06; break;
                case 'r': keycode = 0x0D; break;

                // Third Row
                case 'a': keycode = 0x07; break;
                case 's': keycode = 0x08; break;
                case 'd': keycode = 0x09; break;
                case 'f': keycode = 0x0E; break;

                // Fourth Row
                case 'z': keycode = 0x0A; break;
                case 'x': keycode = 0x00; break;
                case 'c': keycode = 0x0B; break;
                case 'v': keycode = 0x0F; break;
            }

            if (keycode > -1) {
                keypad->keys[keycode] = 1;
            }
        }

        clock_t now = clock();
        double milliseconds_elapsed = difftime(now, start) / 1000;
        if (milliseconds_elapsed >= 150) {
            clear_keypresses(keypad);
            start = clock();
        }
    }
}

void listen_for_keypresses(Keypad *keypad)
{
    pthread_create(&(keypad->thread_id), NULL, handle_keypresses, keypad);
}

void stop_listening_for_keypresses(Keypad *keypad)
{
    pthread_cancel(keypad->thread_id);
}
