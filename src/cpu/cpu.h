#ifndef CPU_H
#define CPU_H

#include "../memory/memory.h"

class CPU {
    public:
        CPU();

        int step();

    private:


        Memory _memory;

        /**
         * Pair of 8-bit registers
        */
        struct Pair {
            Byte *reg_high;
            Byte *reg_low;
        };
        
        // Gameboy's Eight 8-bit registers
        Byte reg_A;   // Accumulator
        Byte reg_F;   // Flags
        Byte reg_B;
        Byte reg_C;
        Byte reg_D;
        Byte reg_E;
        Byte reg_H;
        Byte reg_L;

        Pair AF;
        Pair BC;
        Pair DE;
        Pair HL;

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
        int decode_execute_cb();

        // Helper Functions
        void update_flag(const uint8_t flag, bool new_val);
        bool get_flag(const uint8_t flag) const;        
        Word get_pair(const Pair& pair) const;
        void set_pair(const Pair& pair, const Word value);
        Byte read_hl() const;

        // Control Instructions


        // 8-bit load and arithmetic
        
        void LD(Byte& dest, const Byte value);
        void LD(Byte& dest, const Address address);
        void LD(const Address dest, const Byte value);
        void LDH(const Byte reg, bool into_A);
        void ADD(const Byte value, bool carry);
        void SUB(const Byte value, bool carry);
        void AND(const Byte value);
        void XOR(const Byte value);
        void OR(const Byte value);
        void CP(const Byte value);
        void INC(Byte& reg_value);
        void DEC(Byte& reg_value);
        void SCF();
        void CPL();
        void CCF();
        void RLA(bool circular);
        void RL(Byte& reg, bool circular);
        void RRA(bool circular);
        void RR(Byte& reg, bool circular);
        
        // 16-bit load and arithmetic

        void LD(Word& dest, const Word value);
        void INC(Word& reg_value);
        void INC(Pair pair);

};

#endif // CPU_H