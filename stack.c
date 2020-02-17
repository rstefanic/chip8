#include "stack.h"

char get_current_value(Stack *stack) 
{
    return *(stack->sp);
}

void increment_sp(Stack *stack) 
{
    stack->sp++;
}

void decrement_sp(Stack *stack)
{
    stack->sp--;
}

char* get_pc(Stack *stack)
{
    return stack->pc;
}