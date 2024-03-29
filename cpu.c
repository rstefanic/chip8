#include "cpu.h"
#include "font.h"
#include "keypad.h"

CPU *new_cpu(char* program_name)
{
    CPU *cpu;
    cpu = (CPU*)malloc(sizeof(CPU)); 
    if (cpu == NULL) {
        return NULL;
    }

    /* CHIP-8 interpreter is preloaded with sprite data of the 16 hex digits */
    /* The exact memory adddresses where this resides is unspeicifed, but
     * it must be lower than address 0x200 (the PROG_DATA_SEGMENT)
     */
    unsigned char* mem = &(cpu->memory[0]);
    memcpy(mem, &(font[0][0]), 16 * 5);

    // Load the name of the file that contains our program into memory
    void *program_data_segment_start = &(cpu->memory[PROG_DATA_SEGMENT]);
    load_program_into_memory(program_data_segment_start, program_name);

    cpu->keypad = new_keypad();

    // Set all the registers to 0
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
    cpu->pc = PROG_DATA_SEGMENT;
    cpu->i = 0;

    cpu->draw_flag = 0;

    // Setup new stack and frame buffer
    cpu->stack = new_stack();
    if (cpu->stack == NULL) {
        return NULL;
    }

    cpu->fb = new_framebuffer();
    if (cpu->fb == NULL) {
        return NULL;
    }

    // initalize the RNG module
    initialize_rng();

    return cpu;
}

void clean_cpu(CPU *cpu)
{
    stop_listening_for_keypresses(cpu->keypad);
    free(cpu->keypad);
    free(cpu->stack);
    free(cpu->fb);
    free(cpu);
}

void decrement_st(CPU *cpu)
{
    cpu->st--;
}

void decrement_dt(CPU *cpu)
{
    cpu->dt--;
}

void increment_pc(CPU *cpu)
{
    // instructions are read 16-bits at a time
    cpu->pc = cpu->pc + 2;
}

void set_draw_flag(CPU *cpu)
{
    cpu->draw_flag = 1;
}

void clear_draw_flag(CPU *cpu)
{
    cpu->draw_flag = 0;
}

unsigned short fetch(CPU *cpu)
{
    unsigned short op = 0;
    unsigned int pc = cpu->pc;
    unsigned char* memory_location = (cpu->memory + pc);

    // Shifted over 8 bits because
    // the instructions are 16-bit
    op = (*memory_location) << BYTE_SIZE;
    op += *(memory_location + 1);
    return op;
}

Instruction* decode(unsigned short op_code)
{
    Instruction *ins = malloc(sizeof(Instruction));

    if (ins == NULL) {
        return NULL;
    }

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
        ins->dest.addr = op_code & 0x0FFF;
    }
    else if ((op_code & 0x2FFF) == op_code) {
        // (2nnn) CALL addr
        ins->op = CALL_ADDR;
        ins->dest.addr = op_code & 0x0FFF;
    }
    else if ((op_code & 0x3FFF) == op_code) {
        // (3xkk) SE Vx, byte
        int reg = op_code & 0x0F00;
        int val = op_code & 0x00FF;

        ins->op = SE_VX_BYTE;
        ins->dest.reg = reg >> TWO_NIBBLES;
        ins->src.val = val;
    }
    else if ((op_code & 0x4FFF) == op_code) {
        // (4xkk) SNE Vx, byte
        int reg = op_code & 0x0F00;
        int val = op_code & 0x00FF;

        ins->op = SNE_VX_BYTE;
        ins->dest.reg = reg >> TWO_NIBBLES;
        ins->src.val = val;
    }
    else if ((op_code & 0x5FF0) == op_code) {
        // (5xy0) SE Vx, Vy
        int reg1 = op_code & 0x0F00;
        int reg2 = op_code & 0x00F0;

        ins->op = SE_VX_VY;
        ins->dest.reg = reg1 >> TWO_NIBBLES;
        ins->src.reg = reg2 >> ONE_NIBBLE;
    }
    else if ((op_code & 0x6FFF) == op_code) {
        // (6xkk) LD Vx, byte
        int reg = op_code & 0x0F00;
        int val = op_code & 0x00FF;

        ins->op = LD_VX_BYTE;
        ins->dest.reg = reg >> TWO_NIBBLES;
        ins->src.val = val;
    }
    else if ((op_code & 0x7FFF) == op_code) {
        // (7xkk) ADD Vx, byte
        int reg = op_code & 0x0F00;
        int addr = op_code & 0x00FF;

        ins->op = ADD_VX_BYTE;
        ins->dest.reg = reg >> TWO_NIBBLES;
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

        ins->dest.reg = reg1 >> TWO_NIBBLES;
        ins->src.reg = reg2 >> ONE_NIBBLE;
    }
    else if ((op_code & 0x9FF0) == op_code) {
        // (9xy0) SNE Vx, Vy
        int reg1 = op_code & 0x0F00;
        int reg2 = op_code & 0x00F0;

        ins->op = SNE_VX_VY;
        ins->dest.reg = reg1 >> TWO_NIBBLES;
        ins->src.reg = reg2 >> ONE_NIBBLE;
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
        ins->dest.reg = reg >> TWO_NIBBLES;
        ins->src.val = val;
    }
    else if ((op_code & 0xDFFF) == op_code) {
        // (Dxyn) DRW Vx, Vy, nibble
        int reg1 = op_code & 0x0F00;
        int reg2 = op_code & 0x00F0;
        int val = op_code & 0x000F;

        ins->op = DRW_VX_VY_NIB;
        ins->dest.reg = reg1 >> TWO_NIBBLES;
        ins->src.reg = reg2 >> ONE_NIBBLE;
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
        ins->dest.reg = reg1 >> TWO_NIBBLES;
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
        else if ((op_code & 0xFF65) == op_code) {
            // (Fx65) LD Vx, [I]
            ins->op = LD_VX_I;
        }

        int reg1 = op_code & 0x0F00;
        ins->dest.reg = reg1 >> TWO_NIBBLES;
    }

    return ins;
}

void execute(CPU* cpu, Instruction* instruction)
{
    Op op = instruction->op;

    switch(op) {
        case CLS: {
            clear_buffer(cpu->fb);
            increment_pc(cpu);
            set_draw_flag(cpu);
            break;
        }
        case RET: {
            int ret_address = pop(cpu->stack);
            cpu->pc = ret_address;
            increment_pc(cpu);
            break;
        }
        case JP_ADDR: {
            int addr = instruction->dest.addr;
            cpu->pc = addr;
            break;
        }
        case CALL_ADDR: {
            // push the current location onto the stack
            unsigned int ret_address = cpu->pc;
            push(cpu->stack, ret_address);

            // jump to subroutine location
            unsigned int subroutine_addr = instruction->dest.addr;
            cpu->pc = subroutine_addr;
            break;
        }
        case SE_VX_BYTE: {
            int vx = get_register(cpu, instruction->dest.reg);
            int comparison_val = instruction->src.val;

            // If they're equal, skip the next instruction
            if (vx == comparison_val) {
                increment_pc(cpu);
            }

            increment_pc(cpu);
            break;
        }
        case SNE_VX_BYTE: {
            int vx = get_register(cpu, instruction->dest.reg);
            int comparison_val = instruction->src.val;

            // If they're not equal, skip the next instruction
            if (vx != comparison_val) {
                increment_pc(cpu);
            }

            increment_pc(cpu);
            break;
        }
        case SE_VX_VY: {
            int vx = get_register(cpu, instruction->dest.reg);
            int vy = get_register(cpu, instruction->src.reg);

            if (vx == vy) {
                increment_pc(cpu);
            }

            increment_pc(cpu);
            break;
        }
        case LD_VX_BYTE: {
            int val = instruction->src.val;
            set_register(cpu, instruction->dest.reg, val);
            increment_pc(cpu);
            break;
        }
        case ADD_VX_BYTE: {
            int current_val_at_vx = get_register(cpu, instruction->dest.reg);
            int new_val = current_val_at_vx + (instruction->src.val);

            set_register(cpu, instruction->dest.reg, new_val);
            increment_pc(cpu);
            break;
        }
        case LD_VX_VY: {
            int vy = get_register(cpu, instruction->src.reg);
            set_register(cpu, instruction->dest.reg, vy);
            increment_pc(cpu);
            break;
        }
        case OR_VX_VY: {
            int vx = get_register(cpu, instruction->dest.reg);
            int vy = get_register(cpu, instruction->src.reg);

            int result = vx | vy;
            set_register(cpu, instruction->dest.reg, result);
            increment_pc(cpu);
            break;
        }
        case AND_VX_VY: {
            int vx = get_register(cpu, instruction->dest.reg);
            int vy = get_register(cpu, instruction->src.reg);

            int result = vx & vy;
            set_register(cpu, instruction->dest.reg, result);
            increment_pc(cpu);
            break;
        }
        case XOR_VX_VY: {
            int vx = get_register(cpu, instruction->dest.reg);
            int vy = get_register(cpu, instruction->src.reg);

            int result = vx ^ vy;
            set_register(cpu, instruction->dest.reg, result);
            increment_pc(cpu);
            break;
        }
        case ADD_VX_VY: {
            int vx = get_register(cpu, instruction->dest.reg);
            int vy = get_register(cpu, instruction->src.reg);

            int result = vx + vy;
            set_register(cpu, instruction->dest.reg, result);
            increment_pc(cpu);
            break;
        }
        case SUB_VX_VY: {
            int vx = get_register(cpu, instruction->dest.reg);
            int vy = get_register(cpu, instruction->src.reg);

            // Set VF = NOT borrow of VX from VY
            set_register(cpu, VF, vx > vy);

            int result = vx - vy;
            set_register(cpu, instruction->dest.reg, result);
            increment_pc(cpu);
            break;
        }
        case SHR_VX_VY: {
            int vx = get_register(cpu, instruction->dest.reg);

            // Set VF based on whether or not the LSB on VX is set
            set_register(cpu, VF, (vx & 1));

            int result = vx >> 1;
            set_register(cpu, instruction->dest.reg, result);
            increment_pc(cpu);
            break;
        }
        case SUBN_VX_VY: {
            int vx = get_register(cpu, instruction->dest.reg);
            int vy = get_register(cpu, instruction->src.reg);

            // Set VF = NOT borrow of VY from VX
            set_register(cpu, VF, vy > vx);

            int result = vy - vx;
            set_register(cpu, instruction->dest.reg, result);
            increment_pc(cpu);
            break;
        }
        case SHL_VX_VY: {
            int vx = get_register(cpu, instruction->dest.reg);

            // Set VF based on whether or not the MSB on VX is set
            set_register(cpu, VF, (vx & 0x80));

            int result = vx << 1;
            set_register(cpu, instruction->dest.reg, result);
            increment_pc(cpu);
            break;
        }
        case SNE_VX_VY: {
            int vx = get_register(cpu, instruction->dest.reg);
            int vy = get_register(cpu, instruction->src.reg);

            if (vx != vy) {
                increment_pc(cpu);
            }

            increment_pc(cpu);
            break;
        }
        case LD_ADDR: {
            int addr = instruction->dest.addr;
            cpu->i = addr;
            increment_pc(cpu);
            break;
        }
        case JP_V0_ADDR: {
            int v0 = get_register(cpu, V0);
            int addr = instruction->dest.addr;

            cpu->pc = addr + v0;
            break;
        }
        case RND_VX_BYTE: {
            int rand = rng();
            int val = rand & (instruction->src.val);

            set_register(cpu, instruction->dest.reg, val);
            increment_pc(cpu);
            break;
        }
        case DRW_VX_VY_NIB: {
            int vx = get_register(cpu, instruction->dest.reg);
            int vy = get_register(cpu, instruction->src.reg);
            int nibble = instruction->extra_operand.val;

            // Set VF to 0 by default
            set_register(cpu, VF, 0);

            for(int n = 0; n < nibble; n++) {
                int current_byte = cpu->memory[(cpu->i) + n];

                for(int bit_pos = 0; bit_pos < BYTE_SIZE; bit_pos++) {
                    int value = current_byte & (1 << (7 - bit_pos)) ? 0x01 : 0x00;

                    int x = (vx + bit_pos) % FRAME_WIDTH;
                    int y = (vy + n) % FRAME_HEIGHT;

                    cpu->fb->buffer[x + (y * FRAME_WIDTH)] ^= value;

                    int collision = cpu->fb->buffer[x + (y * FRAME_WIDTH)] & value;

                    if (collision) {
                        set_register(cpu, VF, 1);
                    }
                }
            }

            set_draw_flag(cpu);
            increment_pc(cpu);
            break;
        }
        case SKP_VX: {
            int vx = get_register(cpu, instruction->dest.reg);

            char key_is_pressed = poll_keypress(cpu->keypad, vx);
            if (key_is_pressed) {
                increment_pc(cpu);
            }

            increment_pc(cpu);
            break;
        }
        case SKNP_VX: {
            int vx = get_register(cpu, instruction->dest.reg);

            char key_is_pressed = poll_keypress(cpu->keypad, vx);
            if (!key_is_pressed) {
                increment_pc(cpu);
            }

            increment_pc(cpu);
            break;
        }
        case LD_VX_DT: {
            int dt = cpu->dt;
            set_register(cpu, instruction->dest.reg, dt);
            increment_pc(cpu);
            break;
        }
        case LD_VX_K: {
            // Since we only increment the PC when a key is
            // pressed, execution effectively stops until
            // a key is pressed.

            // Spin through the keys until we find a keypress.
            for(int i = 0; i < KEYPAD_SIZE; i++) {
                if (poll_keypress(cpu->keypad, i)) {
                    set_register(cpu, instruction->dest.reg, i);
                    increment_pc(cpu);
                }
            }

            break;
        }
        case LD_DT_VX: {
            int vx = get_register(cpu, instruction->dest.reg);
            set_register(cpu, DT, vx);
            increment_pc(cpu);
            break;
        }
        case LD_ST_VX: {
            int vx = get_register(cpu, instruction->dest.reg);
            set_register(cpu, ST, vx);
            increment_pc(cpu);
            break;
        }
        case ADD_I_VX: {
            int i = cpu->i;
            int vx = get_register(cpu, instruction->dest.reg);
            int result = i + vx;

            set_register(cpu, I, result);
            increment_pc(cpu);
            break;
        }
        case LD_F_VX: {
            int vx = get_register(cpu, instruction->dest.reg);

            // We just need to multiply 5 by VX in order to get
            // this value. This is because all the font data
            // is stored in the first 80 bytes of memory.
            int result = vx * 5;

            set_register(cpu, I, result);
            increment_pc(cpu);
            break;
        }
        case LD_B_VX: {
            int vx = get_register(cpu, instruction->dest.reg);
            int i = cpu->i;

            cpu->memory[i] = bcd_hundreds_at(vx);
            cpu->memory[i + 1] = bcd_tens_at(vx);
            cpu->memory[i + 2] = bcd_ones_at(vx);
            increment_pc(cpu);
            break;
        }
        case LD_I_VX: {
            Register vx = instruction->dest.reg;
            int i = cpu->i;
            int registers_moved = 0;

            // Loop through the registers storing
            // them into Memory[I] from V0 to VX
            for (Register r = V0; r <= vx; r++, registers_moved++) {
                cpu->memory[i] = get_register(cpu, r);
                i++;
            }

            cpu->i += registers_moved + 1;
            increment_pc(cpu);
            break;
        }
        case LD_VX_I: {
            Register vx = instruction->dest.reg;
            int i = cpu->i;
            int registers_moved = 0;

            // Fills V0 to VX with values from memory
            // starting at address I.
            for (Register r = V0; r <= vx; r++, registers_moved++) {
                int value = cpu->memory[i];
                set_register(cpu, r, value);
                i++;
            }

            cpu->i += registers_moved + 1;
            increment_pc(cpu);
            break;
        }
        default:
            break;
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
        case V6: return cpu->v6;
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
        case V6: cpu->v6 = val; break;
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

char* get_op_string(Op op)
{
    switch(op) {
        case SYS_ADDR: return "SYS_ADDR";
        case CLS: return "CLS";
        case RET: return "RET";
        case JP_ADDR: return "JP_ADDR";
        case CALL_ADDR: return "CALL_ADDR";
        case SE_VX_BYTE: return "SE_VX_BYTE";
        case SNE_VX_BYTE: return "SNE_VX_BYTE";
        case SE_VX_VY: return "SE_VX_VY";
        case LD_VX_BYTE: return "LD_VX_BYTE";
        case ADD_VX_BYTE: return "ADD_VX_BYTE";
        case LD_VX_VY: return "LD_VX_VY";
        case OR_VX_VY: return "OR_VX_VY";
        case AND_VX_VY: return "AND_VX_VY";
        case XOR_VX_VY: return "XOR_VX_VY";
        case ADD_VX_VY: return "ADD_VX_VY";
        case SUB_VX_VY: return "SUB_VX_VY";
        case SHR_VX_VY: return "SHR_VX_VY";
        case SUBN_VX_VY: return "SUBN_VX_VY";
        case SHL_VX_VY: return "SHL_VX_VY";
        case SNE_VX_VY: return "SNE_VX_VY";
        case LD_ADDR: return "LD_ADDR";
        case JP_V0_ADDR: return "JP_V0_ADDR";
        case RND_VX_BYTE: return "RND_VX_BYTE";
        case DRW_VX_VY_NIB: return "DRW_VX_VY_NIB";
        case SKP_VX: return "SKP_VX";
        case SKNP_VX: return "SKNP_VX";
        case LD_VX_DT: return "LD_VX_DT";
        case LD_VX_K: return "LD_VX_K";
        case LD_DT_VX: return "LD_DT_VX";
        case LD_ST_VX: return "LD_ST_VX";
        case ADD_I_VX: return "ADD_I_VX";
        case LD_F_VX: return "LD_F_VX";
        case LD_B_VX: return "LD_B_VX";
        case LD_I_VX: return "LD_I_VX";
        case LD_VX_I: return "LD_VX_I";
        default: return "UNKNOWN OP";
    }
}
