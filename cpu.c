#include <stdio.h>
#include "cpu.h"

void cpu_init(CPU *cpu) {
    cpu->PC = 0x0100;
    cpu->SP = 0xFFFE;
    cpu->AF = 0x0000;
    cpu->BC = 0x0000;
    cpu->DE = 0x0000;
    cpu->HL = 0x0000;
}

bool cpu_step(CPU *cpu) {       //I'm going to change bool to void there and in cpu.h when im done working on cpu commands.
    uint8_t low, high;
    uint16_t target;

    uint8_t opcode = read_byte(cpu->PC++);
    printf("Current CPU: | AF: 0x%04X | BC: 0x%04X | DE: 0x%04X | HL: 0x%04X | SP: 0x%04X | PC: 0x%04X | opcode: 0x%02X\n", cpu->AF, cpu->BC, cpu->DE, cpu->HL, cpu->SP, cpu->PC-1, opcode);

    switch (opcode) {
        case 0x00:  //NOP
            break;
        case 0xC3:  //JP u16 
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->PC = target;
            break;
        case 0x21:  //LD HL, u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->HL = target;
            break;
        case 0x47:  //LD B, A
            cpu->B = cpu->A;
            break;
        case 0x11: //LD DE, u16
            low = read_byte(cpu->PC++);
            high = read_byte(cpu->PC++);
            target = (high << 8) | low;
            cpu->DE = target;
            break;
        case 0x0E:  //LD C, u8
            cpu->C = read_byte(cpu->PC++);
            break;
        case 0x2A:  //LD A, (HL+)
            cpu->A = read_byte(cpu->HL++);
            break;
        case 0x12:  //LD (DE), A
            write_byte(cpu->DE, cpu->A);
            break;
        case 0x1C:  //INC E
            cpu->E++;
            if (cpu->E == 0x00) {   //if result is 0, make zero flag= 1
                cpu->F |= 0x80;
            }
            else {                  //else make zero flag = 0
                cpu->F &= 0x70;
            }
            break;
        case 0x20:  //JR NZ, i8
            int8_t target = (int8_t)read_byte(cpu->PC++);
            bool is_zero = (cpu->F & 0x80) != 0;
            if (!is_zero) {
                cpu->PC += target;
            }
            break;
        default:
            printf("Command doesn't exist. Opcode: 0x%02X\n", opcode);
            return 1;
    }

    return 0;
}