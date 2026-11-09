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
    cpu->halt = false;
}

bool cpu_step(CPU *cpu) {
    if (cpu->halt == true) {
        return 0;
    }
    int8_t offset;
    uint8_t low, high, val;
    uint8_t z_flag, n_flag, h_flag, c_flag;
    uint8_t old_z, old_n, old_h, old_c;
    uint16_t result ,target, return_address;
    uint32_t bigresult;
    bool is_zero, carry;
    uint8_t cb_opcode;

    uint8_t opcode = read_byte(cpu->PC++);
    //printf("Current CPU: | AF: 0x%04X | BC: 0x%04X | DE: 0x%04X | HL: 0x%04X | SP: 0x%04X | PC: 0x%04X | opcode: 0x%02X\n", cpu->AF, cpu->BC, cpu->DE, cpu->HL, cpu->SP, cpu->PC-1, opcode);

    switch (opcode) {
        // ==========================================
        // 1. SYSTEM & CONTROL INSTRUCTIONS ALL DONE (Don't forget to add Delay Phenomenon bug to EI)
        // ==========================================
        case 0x00:  //NOP
            break;
        case 0x10:  //STOP
            printf("CPU stopped with 0x10 command.");
            return 1;
            break;
        case 0x76:  //HALT
            cpu->halt = true;
            break;
        case 0xF3:  //DI (Disable Interrupts)
            cpu->IME = false;
            break;
        case 0xFB:  //EI (Enable Interrupts)
            cpu->IME = true;
            break;
        case 0xCB:  //PREFIX CB
            cb_opcode = read_byte(cpu->PC++);
            switch (cb_opcode) {
                default:
                    printf("Command doesn't exist on 0xCB prefixed. Opcode: 0x%02X\n", cb_opcode);
                    return 1;
                    break;
            }
            break;

        // ==========================================
        // 2. LOAD / DATA TRANSFER (LD) INSTRUCTIONS ALL DONE
        // ==========================================
        case 0x01:  //LD BC, u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->BC = target;
            break;
        case 0x11:  //LD DE, u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->DE = target;
            break;
        case 0x21:  //LD HL, u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->HL = target;
            break;
        case 0x31:  //LD SP, u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->SP = target;
            break;
        case 0x02:  //LD (BC), A
            write_byte(cpu->BC, cpu->A);
            break;
        case 0x12:  //LD (DE), A
            write_byte(cpu->DE, cpu->A);
            break;
        case 0x22:  //LD (HL+), A
            write_byte(cpu->HL++, cpu->A);
            break;
        case 0x32:  //LD (HL-), A
            write_byte(cpu->HL--, cpu->A);
            break;
        case 0x06:  //LD B, u8
            cpu->B = read_byte(cpu->PC++);
            break;
        case 0x16:  //LD D, u8
            cpu->D = read_byte(cpu->PC++);
            break;
        case 0x26:  //LD H, u8
            cpu->H = read_byte(cpu->PC++);
            break;
        case 0x36:  //LD (HL), u8
            write_byte(cpu->HL, read_byte(cpu->PC++));
            break;
        case 0x08:  //LD (u16), SP
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            write_byte(target, cpu->SP & 0xFF);
            write_byte(target + 1, (cpu->SP >> 8) & 0xFF);
            break;
        case 0x0A:  //LD A, (BC)
            cpu->A = read_byte(cpu->BC);
            break;
        case 0x1A:  //LD A, (DE)
            cpu->A = read_byte(cpu->DE);
            break;
        case 0x2A:  //LD A, (HL+)
            cpu->A = read_byte(cpu->HL++);
            break;
        case 0x3A:  //LD A, (HL-)
            cpu->A = read_byte(cpu->HL--);
            break;
        case 0x0E:  //LD C, u8
            cpu->C = read_byte(cpu->PC++);
            break;
        case 0x1E:  //LD E, u8
            cpu->E = read_byte(cpu->PC++);
            break;
        case 0x2E:  //LD L, u8
            cpu->L = read_byte(cpu->PC++);
            break;
        case 0x3E:  //LD A, u8
            cpu->A = read_byte(cpu->PC++);
            break;
        case 0x40:  //LD B, B
            cpu->B = cpu->B;    //we have come from a while ago to the present
            break;
        case 0x50:  //LD D, B
            cpu->D = cpu->B;
            break;
        case 0x60:  //LD H, B
            cpu->H = cpu->B;
            break;
        case 0x70:  //LD (HL), B
           write_byte(cpu->HL, cpu->B);
            break;
        case 0x41:  //LD B, C
            cpu->B = cpu->C;
            break;
        case 0x51:  //LD D, C
            cpu->D = cpu->C;
            break;
        case 0x61:  //LD H, C
            cpu->H = cpu->C;
            break;
        case 0x71:  //LD (HL), C
            write_byte(cpu->HL, cpu->C);
            break;
        case 0x42:  //LD B, D
            cpu->B = cpu->D;
            break;
        case 0x52:  //LD D, D
            cpu->D = cpu->D;
            break;
        case 0x62:  //LD H, D
            cpu->H = cpu->D;
            break;
        case 0x72:  //LD (HL), D
            write_byte(cpu->HL, cpu->D);
            break;
        case 0x43:  //LD B, E
            cpu->B = cpu->E;
            break;
        case 0x53:  //LD D, E
            cpu->D = cpu->E;
            break;
        case 0x63:  //LD H, E
            cpu->H = cpu->E;
            break;
        case 0x73:  //LD (HL), E
            write_byte(cpu->HL, cpu->E);
            break;
        case 0x44:  //LD B, H
            cpu->B = cpu->H;
            break;
        case 0x54:  //LD D, H
            cpu->D = cpu->H;
            break;
        case 0x64:  //LD H, H
            cpu->H = cpu->H;
            break;
        case 0x74:  //LD (HL), H
            write_byte(cpu->HL, cpu->H);
            break;
        case 0x45:  //LD B, L
            cpu->B = cpu->L;
            break;
        case 0x55:  //LD D, L
            cpu->D = cpu->L;
            break;
        case 0x65:  //LD H, L
            cpu->H = cpu->L;
            break;
        case 0x75:  //LD (HL), L
            write_byte(cpu->HL, cpu->L);
            break;
        case 0x46:  //LD B, (HL)
            cpu->B = read_byte(cpu->HL);
            break;
        case 0x56:  //LD D, (HL)
            cpu->D = read_byte(cpu->HL);
            break;
        case 0x66:  //LD H, (HL)
            cpu->H = read_byte(cpu->HL);
            break;
        case 0x47:  //LD B, A
            cpu->B = cpu->A;
            break;
        case 0x57:  //LD D, A
            cpu->D = cpu->A;
            break;
        case 0x67:  //LD H, A
            cpu->H = cpu->A;
            break;
        case 0x77:  //LD (HL), A
            write_byte(cpu->HL, cpu->A);
            break;
        case 0x48:  //LD C, B
            cpu->C = cpu->B;
            break;
        case 0x58:  //LD E, B
            cpu->E = cpu->B;
            break;
        case 0x68:  //LD L, B
            cpu->L = cpu->B;
            break;
        case 0x78:  //LD A, B
            cpu->A = cpu->B;
            break;
        case 0x49:  //LD C, C
            cpu->C = cpu->C;
            break;
        case 0x59:  //LD E, C
            cpu->E = cpu->C;
            break;
        case 0x69:  //LD L, C
            cpu->L = cpu->C;
            break;
        case 0x79:  //LD A, C
            cpu->A = cpu->C;
            break;
        case 0x4A:  //LD C, D
            cpu->C = cpu->D;
            break;
        case 0x5A:  //LD E, D
            cpu->E = cpu->D;
            break;
        case 0x6A:  //LD L, D
            cpu->L = cpu->D;
            break;
        case 0x7A:  //LD A, D
            cpu->A = cpu->D;
            break;
        case 0x4B:  //LD C, E
            cpu->C = cpu->E;
            break;
        case 0x5B:  //LD E, E
            cpu->E = cpu->E;
            break;
        case 0x6B:  //LD L, E
            cpu->L = cpu->E;
            break;
        case 0x7B:  //LD A, E
            cpu->A = cpu->E;
            break;
        case 0x4C:  //LD C, H
            cpu->C = cpu->H;
            break;
        case 0x5C:  //LD E, H
            cpu->E = cpu->H;
            break;
        case 0x6C:  //LD L, H
            cpu->L = cpu->H;
            break;
        case 0x7C:  //LD A, H
            cpu->A = cpu->H;
            break;
        case 0x4D:  //LD C, L
            cpu->C = cpu->L;
            break;
        case 0x5D:  //LD E, L
            cpu->E = cpu->L;
            break;
        case 0x6D:  //LD L, L
            cpu->L = cpu->L;
            break;
        case 0x7D:  //LD A, L
            cpu->A = cpu->L;
            break;
        case 0x4E:  //LD C, (HL)
            cpu->C = read_byte(cpu->HL);
            break;
        case 0x5E:  //LD E, (HL)
            cpu->E = read_byte(cpu->HL);
            break;
        case 0x6E:  //LD L, (HL)
            cpu->L = read_byte(cpu->HL);
            break;
        case 0x7E:  //LD A, (HL)
            cpu->A = read_byte(cpu->HL);
            break;
        case 0x4F:  //LD C, A
            cpu->C = cpu->A;
            break;
        case 0x5F:  //LD E, A
            cpu->E = cpu->A;
            break;
        case 0x6F:  //LD L, A
            cpu->L = cpu->A;
            break;
        case 0x7F:  //LD A, A
            cpu->A = cpu->A;
            break;
        case 0xE0:  //LD (0xFF00 + u8), A
            target = 0xFF00 + read_byte(cpu->PC++);
            write_byte(target, cpu->A);
            break;
        case 0xF0:  //LD A, (0xFF00 + u8)
            target = 0xFF00 + read_byte(cpu->PC++);
            cpu->A = read_byte(target);
            break;
        case 0xE2:  //LD (0xFF00 + C), A
            target = 0xFF00 + cpu->C;
            write_byte(target, cpu->A);
            break;
        case 0xF2:  //LD A, (0xFF00 + C)
            target = 0xFF00 + cpu->C;
            cpu->A = read_byte(target);
            break;
        case 0xF9:  //LD SP, HL
            cpu->SP = cpu->HL;
            break;
        case 0xEA:  //LD (U16), A
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            write_byte(target, cpu->A);
            break;
        case 0xFA:  //LD A, (u16)
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->A = read_byte(target);
            break;

        // ==========================================
        // 3. ARITHMETIC (INC DONE / DEC DONE) INSTRUCTIONS ALL DONE
        // ==========================================
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
        // 4. ARITHMETICAL & LOGICAL OPERATIONS DONE
        // ==========================================

        case 0x80:  //ADD A, B
            result = cpu->A + cpu->B;
            h_flag = ((cpu->A & 0x0F) + (cpu->B & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A += cpu->B;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x90:  //SUB A, B
            val = cpu->A;
            cpu->A -= cpu->B;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            h_flag = ((val & 0x0F) < (cpu->B & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (val < cpu->B) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xA0:  //AND A, B
            cpu->A &= cpu->B;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            h_flag = 0x20;
            cpu->F |= h_flag;
            break;
        case 0xB0:  //OR A, B
            cpu->A |= cpu->B;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0x81:  //ADD A, C
            result = cpu->A + cpu->C;
            h_flag = ((cpu->A & 0x0F) + (cpu->C & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A += cpu->C;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x91:  //SUB A, C
            val = cpu->A;
            cpu->A -= cpu->C;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            h_flag = ((val & 0x0F) < (cpu->C & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (val < cpu->C) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xA1:  //AND A, C
            cpu->A &= cpu->C;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            h_flag = 0x20;
            cpu->F |= h_flag;
            break;
        case 0xB1:  //OR A, C
            cpu->A |= cpu->C;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0x82:  //ADD A, D
            result = cpu->A + cpu->D;
            h_flag = ((cpu->A & 0x0F) + (cpu->D & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A += cpu->D;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x92:  //SUB A, D
            val = cpu->A;
            cpu->A -= cpu->D;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            h_flag = ((val & 0x0F) < (cpu->D & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (val < cpu->D) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xA2:  //AND A, D
            cpu->A &= cpu->D;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            h_flag = 0x20;
            cpu->F |= h_flag;
            break;
        case 0xB2:  //OR A, D
            cpu->A |= cpu->D;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0x83:  //ADD A, E
            result = cpu->A + cpu->E;
            h_flag = ((cpu->A & 0x0F) + (cpu->E & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A += cpu->E;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x93:  //SUB A, E
            val = cpu->A;
            cpu->A -= cpu->E;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            h_flag = ((val & 0x0F) < (cpu->E & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (val < cpu->E) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xA3:  //AND A, E
            cpu->A &= cpu->E;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            h_flag = 0x20;
            cpu->F |= h_flag;
            break;
        case 0xB3:  //OR A, E
            cpu->A |= cpu->E;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0x84:  //ADD A, H
            result = cpu->A + cpu->H;
            h_flag = ((cpu->A & 0x0F) + (cpu->H & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A += cpu->H;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x94:  //SUB A, H
            val = cpu->A;
            cpu->A -= cpu->H;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            h_flag = ((val & 0x0F) < (cpu->H & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (val < cpu->H) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xA4:  //AND A, H
            cpu->A &= cpu->H;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            h_flag = 0x20;
            cpu->F |= h_flag;
            break;
        case 0xB4:  //OR A, H
            cpu->A |= cpu->H;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0x85:  //ADD A, L
            result = cpu->A + cpu->L;
            h_flag = ((cpu->A & 0x0F) + (cpu->L & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A += cpu->L;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x95:  //SUB A, L
            val = cpu->A;
            cpu->A -= cpu->L;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            h_flag = ((val & 0x0F) < (cpu->L & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (val < cpu->L) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xA5:  //AND A, L
            cpu->A &= cpu->L;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            h_flag = 0x20;
            cpu->F |= h_flag;
            break;
        case 0xB5:  //OR A, L
            cpu->A |= cpu->L;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0x86:  //ADD A, (HL)
            result = cpu->A + read_byte(cpu->HL);
            h_flag = ((cpu->A & 0x0F) + (read_byte(cpu->HL) & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A += read_byte(cpu->HL);
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x96:  //SUB A, (HL)
            val = cpu->A;
            cpu->A -= read_byte(cpu->HL);
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            h_flag = ((val & 0x0F) < (read_byte(cpu->HL) & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (val < read_byte(cpu->HL)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xA6:  //AND A, (HL)
            cpu->A &= read_byte(cpu->HL);
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            h_flag = 0x20;
            cpu->F |= h_flag;
            break;
        case 0xB6:  //OR A, (HL)
            cpu->A |= read_byte(cpu->HL);
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0xC6:  //ADD A, u8
            target = read_byte(cpu->PC++);
            result = cpu->A + target;
            h_flag = ((cpu->A & 0x0F) + (target & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A += target;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xD6:  //SUB A, u8
            target = read_byte(cpu->PC++);
            val = cpu->A;
            cpu->A -= target;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            h_flag = ((val & 0x0F) < (target & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (val < target) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xE6:  //AND A, u8
            target = read_byte(cpu->PC++);
            cpu->A &= target;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            h_flag = 0x20;
            cpu->F |= h_flag;
            break;
        case 0xF6:  //OR A, u8
            target = read_byte(cpu->PC++);
            cpu->A |= target;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0x87:  //ADD A, A
            result = cpu->A + cpu->A;
            h_flag = ((cpu->A & 0x0F) + (cpu->A & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A += cpu->A;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x97:  //SUB A, A
            val = cpu->A;
            cpu->A -= cpu->A;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            h_flag = ((val & 0x0F) < (cpu->A & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (val < cpu->A) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xA7:  //AND A, A
            cpu->A &= cpu->A;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            h_flag = 0x20;
            cpu->F |= h_flag;
            break;
        case 0xB7:  //OR A, A
            cpu->A |= cpu->A;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;

        case 0x88:  //ADC A, B
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A + cpu->B + old_c;
            h_flag = ((cpu->A & 0x0F) + (cpu->B & 0x0F) + old_c > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A = result & 0xFF;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x98:  //SBC A, B
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A - cpu->B - old_c;
            z_flag = ((result & 0xFF) == 0x00) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < ((cpu->B & 0x0F) + old_c)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = ((uint16_t)cpu->A < (uint16_t)(cpu->B + old_c)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            cpu->A = result & 0xFF;
            break;
        case 0xA8:  //XOR A, B
            cpu->A ^= cpu->B;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0xB8:  //CP A, B
            z_flag = (cpu->A == cpu->B) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < (cpu->B & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (cpu->A < cpu->B) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x89:  //ADC A, C
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A + cpu->C + old_c;
            h_flag = ((cpu->A & 0x0F) + (cpu->C & 0x0F) + old_c > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A = result & 0xFF;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x99:  //SBC A, C
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A - cpu->C - old_c;
            z_flag = ((result & 0xFF) == 0x00) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < ((cpu->C & 0x0F) + old_c)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = ((uint16_t)cpu->A < (uint16_t)(cpu->C + old_c)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            cpu->A = result & 0xFF;
            break;
        case 0xA9:  //XOR A, C
            cpu->A ^= cpu->C;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0xB9:  //CP A, C
            z_flag = (cpu->A == cpu->C) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < (cpu->C & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (cpu->A < cpu->C) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x8A:  //ADC A, D
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A + cpu->D + old_c;
            h_flag = ((cpu->A & 0x0F) + (cpu->D & 0x0F) + old_c > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A = result & 0xFF;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x9A:  //SBC A, D
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A - cpu->D - old_c;
            z_flag = ((result & 0xFF) == 0x00) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < ((cpu->D & 0x0F) + old_c)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = ((uint16_t)cpu->A < (uint16_t)(cpu->D + old_c)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            cpu->A = result & 0xFF;
            break;
        case 0xAA:  //XOR A, D
            cpu->A ^= cpu->D;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0xBA:  //CP A, D
            z_flag = (cpu->A == cpu->D) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < (cpu->D & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (cpu->A < cpu->D) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x8B:  //ADC A, E
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A + cpu->E + old_c;
            h_flag = ((cpu->A & 0x0F) + (cpu->E & 0x0F) + old_c > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A = result & 0xFF;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x9B:  //SBC A, E
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A - cpu->E - old_c;
            z_flag = ((result & 0xFF) == 0x00) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < ((cpu->E & 0x0F) + old_c)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = ((uint16_t)cpu->A < (uint16_t)(cpu->E + old_c)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            cpu->A = result & 0xFF;
            break;
        case 0xAB:  //XOR A, E
            cpu->A ^= cpu->E;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0xBB:  //CP A, E
            z_flag = (cpu->A == cpu->E) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < (cpu->E & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (cpu->A < cpu->E) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x8C:  //ADC A, H
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A + cpu->H + old_c;
            h_flag = ((cpu->A & 0x0F) + (cpu->H & 0x0F) + old_c > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A = result & 0xFF;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x9C:  //SBC A, H
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A - cpu->H - old_c;
            z_flag = ((result & 0xFF) == 0x00) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < ((cpu->H & 0x0F) + old_c)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = ((uint16_t)cpu->A < (uint16_t)(cpu->H + old_c)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            cpu->A = result & 0xFF;
            break;
        case 0xAC:  //XOR A, H
            cpu->A ^= cpu->H;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0xBC:  //CP A, H
            z_flag = (cpu->A == cpu->H) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < (cpu->H & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (cpu->A < cpu->H) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x8D:  //ADC A, L
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A + cpu->L + old_c;
            h_flag = ((cpu->A & 0x0F) + (cpu->L & 0x0F) + old_c > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A = result & 0xFF;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x9D:  //SBC A, L
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A - cpu->L - old_c;
            z_flag = ((result & 0xFF) == 0x00) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < ((cpu->L & 0x0F) + old_c)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = ((uint16_t)cpu->A < (uint16_t)(cpu->L + old_c)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            cpu->A = result & 0xFF;
            break;
        case 0xAD:  //XOR A, L
            cpu->A ^= cpu->L;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0xBD:  //CP A, L
            z_flag = (cpu->A == cpu->L) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < (cpu->L & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (cpu->A < cpu->L) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x8E:  //ADC A, (HL)
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A + read_byte(cpu->HL) + old_c;
            h_flag = ((cpu->A & 0x0F) + (read_byte(cpu->HL) & 0x0F) + old_c > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A = result & 0xFF;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x9E:  //SBC A, (HL)
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A - read_byte(cpu->HL) - old_c;
            z_flag = ((result & 0xFF) == 0x00) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < ((read_byte(cpu->HL) & 0x0F) + old_c)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = ((uint16_t)cpu->A < (uint16_t)(read_byte(cpu->HL) + old_c)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            cpu->A = result & 0xFF;
            break;
        case 0xAE:  //XOR A, (HL)
            cpu->A ^= read_byte(cpu->HL);
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0xBE:  //CP A, (HL)
            z_flag = (cpu->A == read_byte(cpu->HL)) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < (read_byte(cpu->HL) & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (cpu->A < read_byte(cpu->HL)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xCE:  //ADC A, u8
            target = read_byte(cpu->PC++);
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A + target + old_c;
            h_flag = ((cpu->A & 0x0F) + (target & 0x0F) + old_c > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A = result & 0xFF;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xDE:  //SBC A, u8
            target = read_byte(cpu->PC++);
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A - target - old_c;
            z_flag = ((result & 0xFF) == 0x00) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < ((target & 0x0F) + old_c)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = ((uint16_t)cpu->A < (uint16_t)(target + old_c)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            cpu->A = result & 0xFF;
            break;
        case 0xEE:  //XOR A, u8
            target = read_byte(cpu->PC++);
            cpu->A ^= target;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0xFE:  //CP A, u8
            target = read_byte(cpu->PC++);
            z_flag = (cpu->A == target) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < (target & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (cpu->A < target) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x8F:  //ADC A, A
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A + cpu->A + old_c;
            h_flag = ((cpu->A & 0x0F) + (cpu->A & 0x0F) + old_c > 0x0F) ? 0x20 : 0x00;
            c_flag = (result > 0xFF) ? 0x10 : 0x00;
            cpu->A = result & 0xFF;
            z_flag = (cpu->A == 0x00) ? 0x80 : 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x9F:  //SBC A, A
            old_c = (cpu->F & 0x10) ? 1 : 0;
            result = cpu->A - cpu->A - old_c;
            z_flag = ((result & 0xFF) == 0x00) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < ((cpu->A & 0x0F) + old_c)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = ((uint16_t)cpu->A < (uint16_t)(cpu->A + old_c)) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            cpu->A = result & 0xFF;
            break;
        case 0xAF:  //XOR A, A
            cpu->A ^= cpu->A;
            cpu->F = 0x00;          //reset all flags
            if (cpu->A == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            break;
        case 0xBF:  //CP A, A
            z_flag = (cpu->A == cpu->A) ? 0x80 : 0x00;
            h_flag = ((cpu->A & 0x0F) < (cpu->A & 0x0F)) ? 0x20 : 0x00;
            n_flag = 0x40;
            c_flag = (cpu->A < cpu->A) ? 0x10 : 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0x09:  //ADD HL, BC
            bigresult = cpu->HL + cpu->BC;
            h_flag = ((cpu->HL & 0x0FFF) + (cpu->BC & 0x0FFF) > 0x0FFF) ? 0x20 : 0x00;
            c_flag = (bigresult > 0xFFFF) ? 0x10 : 0x00;
            cpu->HL += cpu->BC;
            n_flag = 0x00;
            old_z = cpu->F & 0x80;
            cpu->F = old_z | n_flag | h_flag | c_flag;
            break;
        case 0x19:  //ADD HL, DE
            bigresult = cpu->HL + cpu->DE;
            h_flag = ((cpu->HL & 0x0FFF) + (cpu->DE & 0x0FFF) > 0x0FFF) ? 0x20 : 0x00;
            c_flag = (bigresult > 0xFFFF) ? 0x10 : 0x00;
            cpu->HL += cpu->DE;
            n_flag = 0x00;
            old_z = cpu->F & 0x80;
            cpu->F = old_z | n_flag | h_flag | c_flag;
            break;
        case 0x29:  //ADD HL, HL
            bigresult = cpu->HL + cpu->HL;
            h_flag = ((cpu->HL & 0x0FFF) + (cpu->HL & 0x0FFF) > 0x0FFF) ? 0x20 : 0x00;
            c_flag = (bigresult > 0xFFFF) ? 0x10 : 0x00;
            cpu->HL += cpu->HL;
            n_flag = 0x00;
            old_z = cpu->F & 0x80;
            cpu->F = old_z | n_flag | h_flag | c_flag;
            break;
        case 0x39:  //ADD HL, SP
            bigresult = cpu->HL + cpu->SP;
            h_flag = ((cpu->HL & 0x0FFF) + (cpu->SP & 0x0FFF) > 0x0FFF) ? 0x20 : 0x00;
            c_flag = (bigresult > 0xFFFF) ? 0x10 : 0x00;
            cpu->HL += cpu->SP;
            n_flag = 0x00;
            old_z = cpu->F & 0x80;
            cpu->F = old_z | n_flag | h_flag | c_flag;
            break;
        case 0xE8:  //ADD SP, i8
            offset = read_byte(cpu->PC++);
            result = cpu->SP + offset;
            h_flag = ((cpu->SP & 0x0F) + (offset & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = ((cpu->SP & 0xFF) + (offset & 0xFF) > 0xFF) ? 0x10 : 0x00;
            cpu->SP = result;
            z_flag = 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;
        case 0xF8:  //LD HL, SP + i8
            offset = read_byte(cpu->PC++);
            result = cpu->SP + offset;
            h_flag = ((cpu->SP & 0x0F) + (offset & 0x0F) > 0x0F) ? 0x20 : 0x00;
            c_flag = ((cpu->SP & 0xFF) + (offset & 0xFF) > 0xFF) ? 0x10 : 0x00;
            cpu->HL = result;
            z_flag = 0x00;
            n_flag = 0x00;
            cpu->F = z_flag | n_flag | h_flag | c_flag;
            break;

        // ==========================================
        // 5. JUMPS & CALLS (JR DONE / JP DONE / CALL DONE / RET DONE) ALL DONE
        // ==========================================
        case 0x20:  //JR NZ, i8
            offset = (int8_t)read_byte(cpu->PC++);
            is_zero = (cpu->F & 0x80) != 0;
            if (!is_zero) {
                cpu->PC += offset;
            }
            break;
        case 0x30:  //JR NC, i8
            offset = (int8_t)read_byte(cpu->PC++);
            carry = (cpu->F & 0x10) != 0;
            if (!carry) {
                cpu->PC += offset;
            }
            break;
        case 0x18:  //JR i8
            offset = (int8_t)read_byte(cpu->PC++);
            cpu->PC += offset;
            break;
        case 0x28:  //JR Z, i8
            offset = (int8_t)read_byte(cpu->PC++);
            is_zero = (cpu->F & 0x80) != 0;
            if (is_zero) {
                cpu->PC += offset;
            }
            break;
        case 0x38:  //JR C, i8
            offset = (int8_t)read_byte(cpu->PC++);
            carry = (cpu->F & 0x10) != 0;
            if (carry) {
                cpu->PC += offset;
            }
            break;

        case 0xC2:  //JP NZ, u16
            is_zero = (cpu->F & 0x80) != 0;
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            if (!is_zero) {
                cpu->PC = target;
            }
            break;
        case 0xD2:  //JP NC, u16
            carry = (cpu->F & 0x10) != 0;
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            if (!carry) {
                cpu->PC = target;
            }
            break;
        case 0xC3:  //JP u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->PC = target;
            break;
        case 0xCA:  //JP Z, u16
            is_zero = (cpu->F & 0x80) != 0;
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            if (is_zero) {
                cpu->PC = target;
            }
            break;
        case 0xDA:  //JP C, u16
            carry = (cpu->F & 0x10) != 0;
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            if (carry) {
                cpu->PC = target;
            }
            break;
        case 0xE9:  //JP (HL)
            cpu->PC = (cpu->H << 8) | cpu->L;
            break;

        case 0xC4:  //CALL NZ, u16
            is_zero = (cpu->F & 0x80) != 0;
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            if (!is_zero) {
                return_address = cpu->PC;
                cpu->SP--;
                write_byte(cpu->SP, (return_address >> 8) & 0xFF);  //writing high byte
                cpu->SP--;
                write_byte(cpu->SP, return_address & 0xFF);         //writing low byte
                cpu->PC = target;
            }
            break;
        case 0xD4:  //CALL NC, u16
            carry = (cpu->F & 0x10) != 0;
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            if (!carry) {
                return_address = cpu->PC;
                cpu->SP--;
                write_byte(cpu->SP, (return_address >> 8) & 0xFF);  //writing high byte
                cpu->SP--;
                write_byte(cpu->SP, return_address & 0xFF);         //writing low byte
                cpu->PC = target;
            }
            break;
        case 0xCC:  //CALL Z, u16
            is_zero = (cpu->F & 0x80) != 0;
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            if (is_zero) {
                return_address = cpu->PC;
                cpu->SP--;
                write_byte(cpu->SP, (return_address >> 8) & 0xFF);  //writing high byte
                cpu->SP--;
                write_byte(cpu->SP, return_address & 0xFF);         //writing low byte
                cpu->PC = target;
            }
            break;
        case 0xDC:  //CALL C, u16
            carry = (cpu->F & 0x10) != 0;
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            if (carry) {
                return_address = cpu->PC;
                cpu->SP--;
                write_byte(cpu->SP, (return_address >> 8) & 0xFF);  //writing high byte
                cpu->SP--;
                write_byte(cpu->SP, return_address & 0xFF);         //writing low byte
                cpu->PC = target;
            }
            break;
        case 0xCD:  //CALL u16
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

        case 0xC0:  //RET NZ
            is_zero = (cpu->F & 0x80) != 0;
            if (!is_zero) {
                low = read_byte(cpu->SP++);
                high = read_byte(cpu->SP++);
                target = (high << 8) | low;
                cpu->PC = target;
            }
            break;
        case 0xD0:  //RET NC
            carry = (cpu->F & 0x10) != 0;
            if (!carry) {
                low = read_byte(cpu->SP++);
                high = read_byte(cpu->SP++);
                target = (high << 8) | low;
                cpu->PC = target;
            }
            break;
        case 0xC8:  //RET Z
            is_zero = (cpu->F & 0x80) != 0;
            if (is_zero) {
                low = read_byte(cpu->SP++);
                high = read_byte(cpu->SP++);
                target = (high << 8) | low;
                cpu->PC = target;
            }
            break;
        case 0xD8:  //RET C
            carry = (cpu->F & 0x10) != 0;
            if (carry) {
                low = read_byte(cpu->SP++);
                high = read_byte(cpu->SP++);
                target = (high << 8) | low;
                cpu->PC = target;
            }
            break;
        case 0xC9:  //RET
            low = read_byte(cpu->SP++);
            high = read_byte(cpu->SP++);
            target = (high << 8) | low;
            cpu->PC = target;
            break;
        case 0xD9:  //RETI
            low = read_byte(cpu->SP++);
            high = read_byte(cpu->SP++);
            target = (high << 8) | low;
            cpu->PC = target;
            cpu->IME = true;
            break;

        // ==========================================
        // 6. STACK OPERATIONS (PUSH DONE / POP DONE) ALL DONE
        // ==========================================
        case 0xC1:  //POP BC
            cpu->C = read_byte(cpu->SP++);
            cpu->B = read_byte(cpu->SP++);
            break;
        case 0xD1:  //POP DE
            cpu->E = read_byte(cpu->SP++);
            cpu->D = read_byte(cpu->SP++);
            break;
        case 0xE1:  //POP HL
            cpu->L = read_byte(cpu->SP++);
            cpu->H = read_byte(cpu->SP++);
            break;
        case 0xF1:  //POP AF
            cpu->F = read_byte(cpu->SP++) & 0xF0;
            cpu->A = read_byte(cpu->SP++);
            break;

        case 0xC5:  //PUSH BC
            cpu->SP--;
            write_byte(cpu->SP, cpu->B);
            cpu->SP--;
            write_byte(cpu->SP, cpu->C);
            break;
        case 0xD5:  //PUSH DE
            cpu->SP--;
            write_byte(cpu->SP, cpu->D);
            cpu->SP--;
            write_byte(cpu->SP, cpu->E);
            break;
        case 0xE5:  //PUSH HL
            cpu->SP--;
            write_byte(cpu->SP, cpu->H);
            cpu->SP--;
            write_byte(cpu->SP, cpu->L);
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