#ifndef KEYPAD_H
#define KEYPAD_H

#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define KEYPAD_SIZE 16

typedef struct Keypad {
    pthread_t thread_id;
    char keys[16];
} Keypad;

Keypad *new_keypad();
char poll_keypress(Keypad *keypad, char c);
void clear_keypresses(Keypad *keypad);
void *handle_keypresses(void *ptr);
void listen_for_keypresses(Keypad *keypad);
void stop_listening_for_keypresses(Keypad *keypad);

#endif // KEYPAD_H
