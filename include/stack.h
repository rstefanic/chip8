#ifndef STACK_H
#define STACK_H

#include "chip8.h"

typedef struct {
    unsigned int buffer[64];
    unsigned char sp; /* stack pointer */
} Stack;

Stack* new_stack();
unsigned int pop(Stack *stack);
void push(Stack *stack, unsigned int val);

#endif /* STACK_H not defined */