#ifndef CPU_H
#define CPU_H

#include "interconnect.h"

struct Registers {
    // Gameboy's Eight 8-bit registers
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
    uint8_t h;
    uint8_t l;

    // Stack Pointer and Program Counter
    uint16_t sp;
    uint16_t pc;
};

class CPU {
    public:
        CPU(const std::vector<uint8_t>& rom_data);

        uint8_t next_byte();
        size_t step();

    private:
        Interconnect _memory;
        Registers _registers;

        void handle_interrupts();
};

#endif