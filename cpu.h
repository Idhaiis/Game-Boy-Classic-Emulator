#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "memory.h"

typedef struct {    
    union {
        struct {
            uint8_t F;  //High byte
            uint8_t A;  //Low byte
        };
        uint16_t AF;
    };
    union {
        struct
        {
            uint8_t C; 
            uint8_t B; 
        };
        uint16_t BC;
    };
    union 
    {
        struct {
            uint8_t E;
            uint8_t D;
        };
        uint16_t DE;
    };
    union 
    {
        struct {
            uint8_t L;
            uint8_t H;
        };
        uint16_t HL;
    };
    uint16_t SP;
    uint16_t PC;
    bool IME;   //Interrupt Master Enable
} CPU;

void cpu_init(CPU *cpu);
bool cpu_step(CPU *cpu);

#endif