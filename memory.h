#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdbool.h>

extern uint8_t memory[65536];

bool load_rom(char rom_directory[]);
uint8_t read_byte(uint16_t address);
void write_byte(uint16_t address, uint8_t value);

#endif 
