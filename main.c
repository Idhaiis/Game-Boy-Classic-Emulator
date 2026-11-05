#include <stdio.h>
#include "memory.h"
#include "main.h"

int main() {

    load_rom("gb-test-roms-master/cpu_instrs/individual/01-special.gb");
    
    for (uint16_t i = 0x0100; i < 0x0104; i++) {
        printf("%X ", read_byte(i));
    }

    return 0;
}