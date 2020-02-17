#include "cpu.h"

CPU *new_cpu() 
{
    CPU *cpu;
    cpu = (CPU*)malloc(sizeof(CPU)); 
    cpu->v0 = 0;
    cpu->v1 = 0;
    cpu->v2 = 0;
    cpu->v3 = 0;
    cpu->v4 = 0;
    cpu->v5 = 0;
    cpu->v6 = 0;
    cpu->v7 = 0;
    cpu->v8 = 0;
    cpu->v9 = 0;
    cpu->va = 0;
    cpu->vb = 0;
    cpu->vc = 0;
    cpu->vd = 0;
    cpu->ve = 0;
    cpu->vf = 0;
    cpu->st = 0;
    cpu->dt = 0;

    return cpu;
}

void decrement_st(CPU *cpu)
{
    cpu->dt--;
}

void decrement_dt(CPU *cpu)
{
    cpu->st--;
}