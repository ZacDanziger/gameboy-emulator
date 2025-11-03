#ifndef CPU_H
#define CPU_H

#include "../memory/memory.h"

class CPU {
    public:
        CPU();

        int step();

    private:
        Memory _memory;
        
        // Gameboy's Eight 8-bit registers
        Byte reg_A;   // Accumulator
        Byte reg_F;   // Flags
        Byte reg_B;
        Byte reg_C;
        Byte reg_D;
        Byte reg_E;
        Byte reg_H;
        Byte reg_L;

        // 16-bit Stack Pointer and Program Counter
        Word reg_SP;
        Word reg_PC;

        const uint8_t FLAG_ZERO       = 0b10000000;
        const uint8_t FLAG_SUB        = 0b01000000;
        const uint8_t FLAG_HALF_CARRY = 0b00100000;
        const uint8_t FLAG_CARRY      = 0b00010000;

        void handle_interrupts(); // needs to be created

        // Fetch -> Decode -> Execute cycle
        Byte fetch();
        int decode_execute(Byte opcode);

        // Helper Functions
        void update_flag(uint8_t flag, bool new_val);
        Byte get_hl();
        void set_hl(const Byte reg_value);

        // 8-bit transfer and I/O
        void ld_8(Byte &reg, Byte value);
        void ld_8(Byte &reg, Address address);
        void add_8(const Byte value, bool carry);
        void sub_8(const Byte value, bool carry);
        void and_8(const Byte value);
        void xor_8(const Byte value);
        void or_8(const Byte value);
        void cp_8(const Byte value);
        void inc_8(Byte &reg_value);
        void dec_8(Byte &reg_value);
        void scf();
        void cpl();
        void ccf();
        

};

#endif // CPU_H