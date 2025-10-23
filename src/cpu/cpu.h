#ifndef CPU_H
#define CPU_H

#include "interconnect.h"

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

    uint8_t &reg(int code) {    // needs testing - not sure I'm dereferencing correctly
        if (code == 6) throw std::runtime_error("Use memory for [HL]");
        return *reg_array[code];
    }

};

class CPU {
    public:
        CPU(const std::vector<uint8_t>& rom_data);

        uint8_t fetch();
        int step();

    private:
        Interconnect _memory;
        Registers _registers;

        bool _zero_flag = false;
        bool _sub_flag = false;
        bool _half_carry_flag = false;
        bool _carry_flag = false;

        void handle_interrupts() {} // needs to be created

        uint8_t get_hl();
        void set_hl(int reg);

        // CPU control
        int nop() {}
        int halt() {}  // needs to be created
        int stop() {}

        // 8-bit transfer and I/O
        int ld_8(int dest, int src);
        int add_8(int src);
        int adc_8(int src);
        int sub_8(int src);
        int sbc_8(int src);
        int and_8(int src);


};

#endif