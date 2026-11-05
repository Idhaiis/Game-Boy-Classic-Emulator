#include <stdio.h>
#include <stdint.h>
#include "main.h"

int main() {
    uint8_t memory[65536] = {0};
    FILE *rom;
    rom = fopen("gb-test-roms-master/cpu_instrs/individual/01-special.gb", "rb");
    if (rom == NULL) {
        perror("Error opening rom.");
        return 1;
    } 

    size_t bytes_read = fread(memory, sizeof(uint8_t), 32768, rom);
    printf("The size of rom: %zu byte\n", bytes_read);

    fclose(rom);

    for (int i = 0x0100; i < 0x0104; i++) {
        printf("%02X ", memory[i]);
    }

    return 0;
}