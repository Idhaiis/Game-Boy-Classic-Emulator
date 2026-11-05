#include <stdio.h>
#include "memory.h"
#include "main.h"
#include "cpu.h"

int main() {

    CPU cpu;
    
    load_rom("gb-test-roms-master/cpu_instrs/individual/01-special.gb");

    cpu_init(&cpu);
    
    while (cpu_step(&cpu) == 0) {
    }

    return 0;
}