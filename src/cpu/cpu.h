#ifndef CPU_H
#define CPU_H

#include "../memory/memory.h"

class CPU {
    public:
        CPU();
        CPU(const std::vector<uint8_t>& rom_data);

        int step();

    private:
        Memory _memory;
        
        // Gameboy's Eight 8-bit registers
        uint8_t reg_A;
        uint8_t reg_F;
        uint8_t reg_B;
        uint8_t reg_C;
        uint8_t reg_D;
        uint8_t reg_E;
        uint8_t reg_H;
        uint8_t reg_L;

        // Stack Pointer and Program Counter
        uint16_t reg_SP;
        uint16_t reg_PC;

        const uint8_t FLAG_ZERO       = 0b10000000;
        const uint8_t FLAG_SUB        = 0b01000000;
        const uint8_t FLAG_HALF_CARRY = 0b00100000;
        const uint8_t FLAG_CARRY      = 0b00010000;

        void handle_interrupts() {} // needs to be created

        uint8_t fetch();
        int decode_execute(uint8_t opcode);

        void update_flag(uint8_t flag, bool new_val);

        uint8_t get_hl();
        void set_hl(int reg);

        int parse_op_code;

        // CPU control
        int nop() {}
        int halt() {}  // needs to be created
        int stop() {}  // needs to be created

        // 8-bit transfer and I/O
        int ld_8(uint8_t& dest, uint8_t value);
        int ld_8(uint8_t& dest, uint16_t address);
        int add_8(int reg, bool carry);
        int sub_8(int reg, bool carry);
        int and_8(int reg);
        int xor_8(int reg);
        int or_8(int reg);
        int cp_8(int reg);
        



};

#endif