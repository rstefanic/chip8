#ifndef STACK_H
#define STACK_H

#include "chip8.h"

typedef struct {
    char buffer[64];
    unsigned char sp; /* stack pointer */
} Stack;

Stack* new_stack();
char pop(Stack *stack);
void push(Stack *stack, char val);

#endif /* STACK_H not defined */