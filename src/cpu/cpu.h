#ifndef CPU_H
#define CPU_H

#include "interconnect.h"
#include <array>

// Struct for opcode metadata stored in /instructions.cpp
struct Instruction {
    const char* name;
    uint8_t cycles;
    void (CPU::*exec)();
};

struct Registers {
    // Gameboy's Eight 8-bit registers
    uint8_t a;
    uint8_t f;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;

    // Stack Pointer and Program Counter
    uint16_t sp;
    uint16_t pc;

    /*
    B = 000
    C = 001
    D = 010
    E = 011
    H = 100
    L = 101
    [HL] = 110 - LD [HL], [HL] is actually HALT
    A = 111
    */
    uint8_t *reg_array[8] = {&b, &c, &d, &e, &h, &l, nullptr, &a};
};

class CPU {
    public:
        CPU(const std::vector<uint8_t>& rom_data);

        uint8_t fetch();
        size_t step();

    private:
        Interconnect _memory;
        Registers _registers;

        void handle_interrupts();

        uint8_t get_hl();
        void set_hl(int reg);

        void NOP() {}
        size_t LD_8(int& dest, int src);      
};

#endif