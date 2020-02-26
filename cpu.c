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
    cpu->pc = 0;
    cpu->i = 0;

    cpu->stack = new_stack();

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

void increment_pc(CPU *cpu)
{
    // instructions are read 16-bits at a time
    cpu->pc = cpu->pc + 2;
}

unsigned short fetch_op(CPU *cpu)
{
    unsigned short op = 0;
    unsigned char *pc = cpu->pc;

    // Shifted over 8 bits because
    // the instructions are 16-bit
    op = (*pc) << BYTE_SIZE;
    op += *(pc + 1);
    increment_pc(cpu);
    return op;
}

void load_program_into_memory(void *program_data_segment, char *program_name)
{
    FILE* fp;
    long file_len;

    fp = fopen(program_name, "rb");
    fseek(fp, 0, SEEK_END);
    file_len = ftell(fp);
    rewind(fp);

    fread((unsigned char*)program_data_segment, file_len, 1, fp);
    fclose(fp);
}