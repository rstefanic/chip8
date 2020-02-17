typedef struct {
    char buffer[64];
    char* sp; /* stack pointer */
    char* pc; /* program counter */
} Stack;

char get_current_value(Stack *stack);
void increment_sp(Stack *stack);
void decrement_sp(Stack *stack);
char* get_pc(Stack *stack);