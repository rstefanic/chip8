#include "chip8.h"

typedef struct {
    char buffer[64];
    unsigned char sp; /* stack pointer */
    unsigned char pc; /* program counter */
} Stack;

Stack* new_stack();
char pop(Stack *stack);
void push(Stack *stack, char val);
char get_pc(Stack *stack);