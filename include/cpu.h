#include <stdlib.h>

#include "chip8.h"
#include "stack.h"

typedef char SoundTimer;

typedef struct {
    Stack* stack;
    unsigned char v0;
    unsigned char v1;
    unsigned char v2;
    unsigned char v3;
    unsigned char v4;
    unsigned char v5;
    unsigned char v6;
    unsigned char v7;
    unsigned char v8;
    unsigned char v9;
    unsigned char va;
    unsigned char vb;
    unsigned char vc;
    unsigned char vd;
    unsigned char ve;
    unsigned char vf;
    unsigned char st;
    unsigned char dt;
    unsigned char pc;
} CPU;

CPU *new_cpu();
void decrement_st(CPU *cpu);
void decrement_dt(CPU *cpu);
