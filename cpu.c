#include <stdio.h>
#include "cpu.h"

void cpu_init(CPU *cpu) {
    cpu->PC = 0x0100;
    cpu->SP = 0xFFFE;
    cpu->AF = 0x0000;
    cpu->BC = 0x0000;
    cpu->DE = 0x0000;
    cpu->HL = 0x0000;
    cpu->IME = false;
}

bool cpu_step(CPU *cpu) {       //I'm going to change bool to void there and in cpu.h when im done working on cpu commands.
    int8_t offset;
    uint8_t low, high, val;
    uint8_t z_flag, n_flag, h_flag, c_flag;
    uint8_t old_z, old_n, old_h, old_c;
    uint16_t target, return_address;
    bool is_zero;

    uint8_t opcode = read_byte(cpu->PC++);
    //printf("Current CPU: | AF: 0x%04X | BC: 0x%04X | DE: 0x%04X | HL: 0x%04X | SP: 0x%04X | PC: 0x%04X | opcode: 0x%02X\n", cpu->AF, cpu->BC, cpu->DE, cpu->HL, cpu->SP, cpu->PC-1, opcode);

    switch (opcode) {
        // ==========================================
        // 1. SYSTEM & CONTROL INSTRUCTIONS
        // ==========================================
        case 0x00:  //NOP
            break;
        case 0xF3:  //DI (Disable Interrupts)
            cpu->IME = false;
            break;

        // ==========================================
        // 2. LOAD / DATA TRANSFER (LD) INSTRUCTIONS
        // ==========================================
        case 0x01:  //LD BC, u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->BC = target;
            break;
        case 0x0E:  //LD C, u8
            cpu->C = read_byte(cpu->PC++);
            break;
        case 0x11:  //LD DE, u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->DE = target;
            break;
        case 0x12:  //LD (DE), A
            write_byte(cpu->DE, cpu->A);
            break;
        case 0x21:  //LD HL, u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->HL = target;
            break;
        case 0x2A:  //LD A, (HL+)
            cpu->A = read_byte(cpu->HL++);
            break;
        case 0x31:  //LD SP, u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->SP = target;
            break;
        case 0x3E:  //LD A, u8
            cpu->A = read_byte(cpu->PC++);
            break;
        case 0x47:  //LD B, A
            cpu->B = cpu->A;
            break;
        case 0x78:  //LD A, B
            cpu->A = cpu->B;
            break;
        case 0x7C:  //LD A, H
            cpu->A = cpu->H;
            break;
        case 0x7D:  //LD A, L
            cpu->A = cpu->L;
            break;
        case 0xE0:  //LD (0xFF00 + u8), A
            target = 0xFF00 + read_byte(cpu->PC++);
            write_byte(target, cpu->A);
            break;
        case 0xEA:  //LD (U16), A
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            write_byte(target, cpu->A);
            break;

        // ==========================================   DONE
        // 3. ARITHMETIC (INC / DEC) INSTRUCTIONS       DONE
        // ==========================================   DONE
        case 0x03:  //INC BC
            cpu->BC++;
            break;
        case 0x13:  //INC DE
            cpu->DE++;
            break;
        case 0x23:  //INC HL
            cpu->HL++;
            break;
        case 0x33:  //INC SP
            cpu->SP++;
            break;

        case 0x04:  //INC B
            cpu->B++;
            z_flag = (cpu->B == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            h_flag = ((cpu->B & 0x0F) == 0x00) ? 0x20 : 0x00;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x14:  //INC D
            cpu->D++;
            z_flag = (cpu->D == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            h_flag = ((cpu->D & 0x0F) == 0x00) ? 0x20 : 0x00;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x24:  //INC H
            cpu->H++;
            z_flag = (cpu->H == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            h_flag = ((cpu->H & 0x0F) == 0x00) ? 0x20 : 0x00;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x34:  //INC (HL)
            val = read_byte(cpu->HL);
            val++;
            write_byte(cpu->HL, val);
            z_flag = (val == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            h_flag = ((val & 0x0F) == 0x00) ? 0x20 : 0x00;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x0C:  //INC C
            cpu->C++;
            z_flag = (cpu->C == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            h_flag = ((cpu->C & 0x0F) == 0x00) ? 0x20 : 0x00;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x1C:  //INC E
            cpu->E++;
            z_flag = (cpu->E == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            h_flag = ((cpu->E & 0x0F) == 0x00) ? 0x20 : 0x00;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x2C:  //INC L
            cpu->L++;
            z_flag = (cpu->L == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            h_flag = ((cpu->L & 0x0F) == 0x00) ? 0x20 : 0x00;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x3C:  //INC A
            cpu->A++;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            h_flag = ((cpu->A & 0x0F) == 0x00) ? 0x20 : 0x00;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;

        case 0x0B:  //DEC BC
            cpu->BC--;
            break;
        case 0x1B:  //DEC DE
            cpu->DE--;
            break;
        case 0x2B:  //DEC HL
            cpu->HL--;
            break;
        case 0x3B:  //DEC SP
            cpu->SP--;
            break;

        case 0x05:  //DEC B
            h_flag = ((cpu->B & 0x0F) == 0x00) ? 0x20 : 0x00;
            cpu->B--;
            z_flag = (cpu->B == 0x00) ? 0x80 : 0x00;
            n_flag = 0x40;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x15:  //DEC D
            h_flag = ((cpu->D & 0x0F) == 0x00) ? 0x20 : 0x00;
            cpu->D--;
            z_flag = (cpu->D == 0x00) ? 0x80 : 0x00;
            n_flag = 0x40;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x25:  //DEC H
            h_flag = ((cpu->H & 0x0F) == 0x00) ? 0x20 : 0x00;
            cpu->H--;
            z_flag = (cpu->H == 0x00) ? 0x80 : 0x00;
            n_flag = 0x40;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x35:  //DEC (HL)
            val = read_byte(cpu->HL);
            h_flag = ((val & 0x0F) == 0x00) ? 0x20 : 0x00;
            val--;
            write_byte(cpu->HL, val);
            z_flag = (val == 0x00) ? 0x80 : 0x00;
            n_flag = 0x40;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x0D:  //DEC C
            h_flag = ((cpu->C & 0x0F) == 0x00) ? 0x20 : 0x00;
            cpu->C--;
            z_flag = (cpu->C == 0x00) ? 0x80 : 0x00;
            n_flag = 0x40;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x1D:  //DEC E
            h_flag = ((cpu->E & 0x0F) == 0x00) ? 0x20 : 0x00;
            cpu->E--;
            z_flag = (cpu->E == 0x00) ? 0x80 : 0x00;
            n_flag = 0x40;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x2D:  //DEC L
            h_flag = ((cpu->L & 0x0F) == 0x00) ? 0x20 : 0x00;
            cpu->L--;
            z_flag = (cpu->L == 0x00) ? 0x80 : 0x00;
            n_flag = 0x40;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;
        case 0x3D:  //DEC A
            h_flag = ((cpu->A & 0x0F) == 0x00) ? 0x20 : 0x00;
            cpu->A--;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x40;
            old_c = cpu->F & 0x10;
            cpu->F = z_flag | n_flag | h_flag | old_c;
            break;

        // ==========================================
        // 4. LOGICAL OPERATIONS (OR / AND / XOR)
        // ==========================================
        case 0xB1:  //OR A, C
            cpu->A |= cpu->C;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;

        // ==========================================
        // 5. JUMPS & CALLS (JR / JP / CALL / RET)
        // ==========================================
        case 0x18:  //JR i8
            offset = (int8_t)read_byte(cpu->PC++);
            cpu->PC += offset;
            break;
        case 0x20:  //JR NZ, i8
            offset = (int8_t)read_byte(cpu->PC++);
            is_zero = (cpu->F & 0x80) != 0;
            if (!is_zero) {
                cpu->PC += offset;
            }
            break;
        case 0xC3:  //JP u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->PC = target;
            break;
        case 0xC9:  //RET
            low = read_byte(cpu->SP++);
            high = read_byte(cpu->SP++);
            target = (high << 8) | low;
            cpu->PC = target;
            break;
        case 0xCD:  //CALL (U16)
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            return_address = cpu->PC;
            cpu->SP--;
            write_byte(cpu->SP, (return_address >> 8) & 0xFF);  //writing high byte
            cpu->SP--;
            write_byte(cpu->SP, return_address & 0xFF);         //writing low byte
            cpu->PC = target;
            break;

        // ==========================================
        // 6. STACK OPERATIONS (PUSH / POP)
        // ==========================================
        case 0xC5:  //PUSH BC
            cpu->SP--;
            write_byte(cpu->SP, cpu->B);
            cpu->SP--;
            write_byte(cpu->SP, cpu->C);
            break;
        case 0xE1:  //POP HL
            cpu->L = read_byte(cpu->SP++);
            cpu->H = read_byte(cpu->SP++);
            break;
        case 0xE5:  //PUSH HL
            cpu->SP--;
            write_byte(cpu->SP, cpu->H);
            cpu->SP--;
            write_byte(cpu->SP, cpu->L);
            break;
        case 0xF1:  //POP AF
            cpu->F = read_byte(cpu->SP++);
            cpu->A = read_byte(cpu->SP++);
            break;
        case 0xF5:  //PUSH AF
            cpu->SP--;
            write_byte(cpu->SP, cpu->A);
            cpu->SP--;
            write_byte(cpu->SP, cpu->F);
            break;

        default:
            printf("Command doesn't exist. Opcode: 0x%02X\n", opcode);
            return 1;
    }

    return 0;
}