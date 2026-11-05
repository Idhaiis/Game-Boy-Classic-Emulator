#include <stdio.h>
#include "memory.h"
#include "main.h"
#include "cpu.h"

int main() {

    CPU cpu;
    
    load_rom("gb-test-roms-master/Games/Tetris.gb");

    cpu_init(&cpu);
    
    for (int i = 0; i<10; i++) {
        cpu_step(&cpu);
    }
    
    for (uint16_t i = 0x0000; i < 0x7FFF; i++) {
        printf("%X ", read_byte(i));
    }

    return 0;
}