#include <stdlib.h>

#include "chip8.h"
#include "stack.h"

typedef char SoundTimer;

typedef struct {
    Stack* stack;
    char v0;
    char v1;
    char v2;
    char v3;
    char v4;
    char v5;
    char v6;
    char v7;
    char v8;
    char v9;
    char va;
    char vb;
    char vc;
    char vd;
    char ve;
    char vf;
    char st;
    char dt;
} CPU;

CPU *new_cpu();
void decrement_st(CPU *cpu);
void decrement_dt(CPU *cpu);
