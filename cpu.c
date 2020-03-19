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

unsigned short fetch(CPU *cpu)
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

Instruction* decode(unsigned short op_code)
{
    Instruction *ins = malloc(sizeof(Instruction));

    if (op_code == 0x00E0) {
        ins->op = CLS;
    }
    else if (op_code == 0x00EE) {
        ins->op = RET;
    }

    // From here, we're just checking the upper most nibble to
    // determine the instruction
    else if ((op_code & 0x0FFF) == op_code) {
        // (0nnn) SYS addr
        ins->op = SYS_ADDR;
        ins->dest.addr = op_code & 0x0FFF;
    }
    else if ((op_code & 0x1FFF) == op_code) {
        // (1nnn) JP addr
        ins->op = JP_ADDR;
        ins->dest.addr = op_code & 0x1F;
    }
    else if ((op_code & 0x2FFF) == op_code) {
        // (2nnn) CALL addr
        ins->op = CALL_ADDR;
        ins->dest.addr = op_code & 0x1F;
    }
    else if ((op_code & 0x3FFF) == op_code) {
        // (3xkk) SE Vx, byte
        int reg = op_code & 0x0F00;
        int val = op_code & 0x00FF;

        ins->op = SE_VX_BYTE;
        ins->dest.reg = reg;
        ins->src.val = val;
    }
    else if ((op_code & 0x4FFF) == op_code) {
        // (4xkk) SNE Vx, byte
        int reg = op_code & 0x0F00;
        int val = op_code & 0x00FF;

        ins->op = SNE_VX_BYTE;
        ins->dest.reg = reg;
        ins->src.val = val;
    }
    else if ((op_code & 0x5FFF) == op_code) {
        // (5xy0) SE Vx, Vy
        int reg1 = op_code & 0x0F00;
        int reg2 = op_code & 0x00F0;

        ins->op = SE_VX_VY;
        ins->dest.reg = reg1;
        ins->src.reg = reg2;
    }
    else if ((op_code & 0x6FFF) == op_code) {
        // (6xkk) LD Vx, byte
        int reg = op_code & 0x0F00;
        int val = op_code & 0x00FF;

        ins->op = LD_VX_BYTE;
        ins->dest.reg = reg;
        ins->src.val = val;
    }
    else if ((op_code & 0x7FFF) == op_code) {
        // (7xkk) ADD Vx, byte
        int reg = op_code & 0x0F00;
        int addr = op_code & 0x00FF;

        ins->op = ADD_VX_BYTE;
        ins->dest.reg = reg;
        ins->src.addr = addr;
    }
    else if ((op_code & 0x8FFF) == op_code) {
        if ((op_code & 0xFFF0) == op_code) {
            // (8xy0) LD Vx, Vy
            ins->op = LD_VX_VY;
        }
        else if ((op_code & 0xFFF1) == op_code) {
            // (8xy1) OR Vx, Vy
            ins->op = OR_VX_VY;
        }
        else if ((op_code & 0xFFF2) == op_code) {
            // (8xy2) AND Vx, Vy
            ins->op = AND_VX_VY;
        }
        else if ((op_code & 0xFFF3) == op_code) {
            // (8xy3) XOR Vx, Vy
            ins->op = XOR_VX_VY;
        }
        else if ((op_code & 0xFFF4) == op_code) {
            // (8xy4) ADD Vx, Vy
            ins->op = ADD_VX_VY;
        }
        else if ((op_code & 0xFFF5) == op_code) {
            // (8xy5) SUB Vx, Vy
            ins->op = SUB_VX_VY;
        }
        else if ((op_code & 0xFFF6) == op_code) {
            // (8xy6) SHR Vx, Vy
            ins->op = SHR_VX_VY;
        }
        else if ((op_code & 0xFFF7) == op_code) {
            // (8xy7) SUBN Vx, Vy
            ins->op = SUBN_VX_VY;
        }
        else if ((op_code & 0xFFFE) == op_code) {
            // (8xyE) SHL Vx, Vy
            ins->op = SHL_VX_VY;
        }

        // for all of these op_codes, they all work with
        // two registers; it's just determining which op
        int reg1 = op_code & 0x0F00;
        int reg2 = op_code & 0x00F0;

        ins->dest.reg = reg1;
        ins->src.reg = reg2;
    }
    else if ((op_code & 0x9FF0) == op_code) {
        // (9xy0) SNE Vx, Vy
        int reg1 = op_code & 0x0F00;
        int reg2 = op_code & 0x00F0;

        ins->op = SNE_VX_VY;
        ins->dest.reg = reg1;
        ins->src.reg = reg2;
    }
    else if ((op_code & 0xAFFF) == op_code) {
        // (Annn) LD I, addr
        int addr = op_code & 0x0FFF;

        ins->op = LD_ADDR;
        ins->dest.addr = addr;
    }
    else if ((op_code & 0xBFFF) == op_code) {
        // (Bnnn) JP V0, addr
        int addr = op_code;

        ins->op = JP_ADDR;
        ins->dest.addr = addr;
    }
    else if ((op_code & 0xCFFF) == op_code) {
        // (Cxkk) RND Vx, byte
        int reg = op_code & 0x0F00;
        int val = op_code & 0x00FF;

        ins->op = RND_VX_BYTE;
        ins->dest.reg = reg;
        ins->src.val = val;
    }
    else if ((op_code & 0xDFFF) == op_code) {
        // (Dxyn) DRW Vx, Vy, nibble
        int reg1 = op_code & 0x0F00;
        int reg2 = op_code & 0x00F0;
        int val = op_code & 0x000F;

        ins->op = DRW_VX_VY_NIB;
        ins->dest.reg = reg1;
        ins->src.reg = reg2;
        ins->extra_operand.val = val;
    }
    else if ((op_code & 0xEFFF) == op_code) {
        // (Ex9E) SKP Vx
        if ((op_code & 0xEF9E) == op_code) {
            ins->op = SKP_VX;
        }
        // (ExA1) SKNP Vx
        else if ((op_code & 0xEFA1) == op_code) {
            ins->op = SKNP_VX;
        }

        int reg1 = op_code & 0x0F00;
        ins->dest.reg = reg1;
    }
    else if ((op_code & 0xFFFF) == op_code) {
        if ((op_code & 0xFF07) == op_code) {
            // (Fx07) LD Vx, DT
            ins->op = LD_VX_DT;
        }
        else if ((op_code & 0xFF0A) == op_code) {
            // (Fx0A) LD Vx, K
            ins->op = LD_VX_K;
        }
        else if ((op_code & 0xFF15) == op_code) {
            // (Fx15) LD DT, Vx
            ins->op = LD_DT_VX;
        }
        else if ((op_code & 0xFF18) == op_code) {
            // (Fx18) LD ST, Vx
            ins->op = LD_ST_VX;
        }
        else if ((op_code & 0xFF1E) == op_code) {
            // (Fx1E) ADD I, Vx
            ins->op = ADD_I_VX;
        }
        else if ((op_code & 0xFF29) == op_code) {
            // (Fx29) LD F, Vx
            ins->op = LD_F_VX;
        }
        else if ((op_code & 0xFF33) == op_code) {
            // (Fx33) LD B, Vx
            ins->op = LD_B_VX;
        }
        else if ((op_code & 0xFF55) == op_code) {
            // (Fx55) LD [I], Vx
            ins->op = LD_I_VX;
        }
        else if ((op_code & 0xFF0A) == op_code) {
            // (Fx65) LD Vx, [I]
            ins->op = LD_VX_I;
        }
    }

    return ins;
}

void execute(CPU* cpu, Instruction* instruction)
{
    Op op = instruction->op;

    if (op == ADD_VX_BYTE) {
        int current_val_at_vx = get_register(cpu, instruction->dest.reg);
        int new_val = current_val_at_vx + (instruction->src.val);

        set_register(cpu,
            instruction->dest.reg,
            new_val
        );
    }
}

int get_register(CPU* cpu, Register reg)
{
    switch(reg) {
        case V0: return cpu->v0;
        case V1: return cpu->v1;
        case V2: return cpu->v2;
        case V3: return cpu->v3;
        case V4: return cpu->v4;
        case V5: return cpu->v5;
        case V6: return cpu->v1;
        case V7: return cpu->v7;
        case V8: return cpu->v8;
        case V9: return cpu->v9;
        case VA: return cpu->va;
        case VB: return cpu->vb;
        case VC: return cpu->vc;
        case VD: return cpu->vd;
        case VE: return cpu->ve;
        case VF: return cpu->vf;
        case ST: return cpu->st;
        case DT: return cpu->dt;
        case I: return cpu->i;
        default: return 0;
    }
}

void set_register(CPU* cpu, Register reg, int val)
{
    switch(reg) {
        case V0: cpu->v0 = val; break;
        case V1: cpu->v1 = val; break;
        case V2: cpu->v2 = val; break;
        case V3: cpu->v3 = val; break;
        case V4: cpu->v4 = val; break;
        case V5: cpu->v5 = val; break;
        case V6: cpu->v1 = val; break;
        case V7: cpu->v7 = val; break;
        case V8: cpu->v8 = val; break;
        case V9: cpu->v9 = val; break;
        case VA: cpu->va = val; break;
        case VB: cpu->vb = val; break;
        case VC: cpu->vc = val; break;
        case VD: cpu->vd = val; break;
        case VE: cpu->ve = val; break;
        case VF: cpu->vf = val; break;
        case ST: cpu->st = val; break;
        case DT: cpu->dt = val; break;
        case I: cpu->i = val; break;
    }
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

