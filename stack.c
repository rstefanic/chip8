#include "stack.h"

Stack* new_stack()
{
    Stack *stack;
    stack = (Stack*)malloc(sizeof(Stack));

    if (stack == NULL) {
        return NULL;
    }

    stack->sp = 0;
    return stack;
}

char pop(Stack *stack)
{
    return stack->buffer[(stack->sp)--];
}

void push(Stack *stack, char val)
{
    stack->buffer[stack->sp] = val;
    (stack->sp)++;
}