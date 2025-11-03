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
        Word fetch16();
        int decode_execute(Byte opcode);

        // Helper Functions
        void update_flag(uint8_t flag, bool new_val);
        Word pair(const Byte reg1, const Byte reg2);
        Byte get_hl();
        void set_hl(const Byte reg_value);

        
        
        // Control Instructions

        void nop();
        
        // 16-bit load and arithmetic

        void ld(Word &dest, const Word value);
        void inc(Word &reg_value);

        // 8-bit load and arithmetic
        
        void LD(Byte &dest, const Byte value);
        void LD(Byte &dest, const Address address);
        void LD(const Address dest, const Byte value);
        void LDH(Byte &dest, const Byte reg);
        void ADD(const Byte value, bool carry);
        void SUB(const Byte value, bool carry);
        void AND(const Byte value);
        void XOR(const Byte value);
        void OR(const Byte value);
        void CP(const Byte value);
        void INC(Byte &reg_value);
        void DEC(Byte &reg_value);
        void SCF();
        void CPL();
        void CCF();
        

};

#endif // CPU_H