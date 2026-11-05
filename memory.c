#include <stdio.h>
#include "memory.h"

uint8_t memory[65536] = {0};

bool load_rom(char rom_directory[]) {
    FILE *rom = fopen(rom_directory, "rb");
    if (rom == NULL) {
        perror("Error opening rom.");
        return false;
    } 

    size_t bytes_read = fread(memory, sizeof(uint8_t), 32768, rom);
    printf("The size of rom: %zu byte\n", bytes_read);
    fclose(rom);

    return true;
}

uint8_t read_byte(uint16_t address) {
    return memory[address];
}

void write_byte(uint16_t address, uint8_t value) {
    memory[address] = value;

    if (address == 0xFF02 && value == 0x81) {   //It's for Blargg's test roms.
        char c = memory[0xFF01];
        printf("%c", c);
        fflush(stdout); 
        memory[0xFF02] = 0;
    }
}