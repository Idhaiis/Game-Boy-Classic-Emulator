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

void cpu_step(CPU *cpu) {
    uint8_t opcode = read_byte(cpu->PC);
    printf("PC: 0x%04X, opcode: 0x%02X\n", cpu->PC, opcode);
    cpu->PC++;
}