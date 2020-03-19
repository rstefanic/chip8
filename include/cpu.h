#include <stdlib.h>

#include "chip8.h"
#include "stack.h"

typedef enum {
    SYS_ADDR,
    CLS,
    RET,
    JP_ADDR,
    CALL_ADDR,
    SE_VX_BYTE,
    SNE_VX_BYTE,
    SE_VX_VY,
    LD_VX_BYTE,
    ADD_VX_BYTE,
    LD_VX_VY,
    OR_VX_VY,
    AND_VX_VY,
    XOR_VX_VY,
    ADD_VX_VY,
    SUB_VX_VY,
    SHR_VX_VY,
    SUBN_VX_VY,
    SHL_VX_VY,
    SNE_VX_VY,
    LD_ADDR,
    JP_V0_ADDR,
    RND_VX_BYTE,
    DRW_VX_VY_NIB,
    SKP_VX,
    SKNP_VX,
    LD_VX_DT,
    LD_VX_K,
    LD_DT_VX,
    LD_ST_VX,
    ADD_I_VX,
    LD_F_VX,
    LD_B_VX,
    LD_I_VX,
    LD_VX_I
} Op;

typedef enum {
    V0, V1, V2, V3, V4, V5, V6, V7, V8, V9,
    VA, VB, VC, VD, VE, VF, ST, DT, I
} Register;

typedef union {
    Register reg;
    int val;
    short addr;
} Operand;

typedef struct {
    Op op;
    Operand dest;
    Operand src;
    Operand extra_operand;
} Instruction;

typedef char SoundTimer;

typedef struct {
    // Contains the stack + stack pointer
    Stack* stack;

    // 0xFFF is the size of the memory
    // 0x000 to 0x080: reserved for font
    // 0x200 to 0xFFF: program data
    unsigned char memory[MAX_MEMORY];

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
    unsigned char *pc;
    unsigned short i;
} CPU;

CPU *new_cpu();
void decrement_st(CPU *cpu);
void decrement_dt(CPU *cpu);
unsigned short fetch(CPU *cpu);
Instruction* decode(unsigned short op_code);
void execute(CPU* cpu, Instruction* instruction);
int get_register(CPU* cpu, Register reg);
void set_register(CPU* cpu, Register reg, int val);
void load_program_into_memory(void *program_data_segment, char *program_name);
