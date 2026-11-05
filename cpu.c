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
    uint8_t opcode = read_byte(cpu->PC);
    printf("PC: 0x%04X, opcode: 0x%02X\n", cpu->PC, opcode);
    //printf("Current CPU: \nAF: 0x%04X\nBC: 0x%04X\nDE: 0x%04X\nHL: 0x%04X\nSP: 0x%04X\nPC: 0x%04X\n", cpu->AF, cpu->BC, cpu->DE, cpu->HL, cpu->SP, cpu->PC);

    switch (opcode) {
        case 0x00:  //NOP
            break;
        default:
            printf("Command doesn't exist. Opcode: 0x%02X\n", opcode);
            return 1;
    }

    cpu->PC++;

    return 0;
}