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

        void handle_interrupts(); // not implemented

        // Fetch -> Decode -> Execute cycle

        Byte fetch();
        Word fetch16();
        int decode_execute(Byte opcode);
        int decode_execute_cb();

        // Helper Functions

        bool get_flag(const uint8_t flag) const;        
        void update_flag(const uint8_t flag, bool new_val);
        Word get_pair(const Pair& pair) const;
        void set_pair(const Pair& pair, const Word value);
        Byte read_hl() const;

        // Control and Miscellaneous Instructions

        void SWAP();    // not implemented
        void DAA();     // not implemented
        void CPL();
        void CCF();
        void SCF();
        void HALT();    // not implemented
        void STOP();    // not implemented
        void DI();      // not implemented
        void EI();      // not implemented

        // 8-bit loads
        
        void LD(Byte& dest, const Byte value);          // dest <- value
        void LD(Byte& dest, const Address address);     // dest <- memory[address]
        void LD(const Address dest, const Byte value);  // memory[address] <- value
        void LDH(const Byte reg, bool into_A);

        // 16-bit loads

        void LD(Word& dest, const Word value);          // dest <- value
        void LD(Pair& pair, const Word value);          // [high | low] <- [value 15:8 | value 7:0]
        void write_SP(const Address address);
        void PUSH(const Pair& pair);    // not implemented
        void POP(Pair& pair);           // not implemented

        // 8-bit arithmetic

        void ADD(const Byte value, bool carry);
        void SUB(const Byte value, bool carry);
        void AND(const Byte value);
        void XOR(const Byte value);
        void OR(const Byte value);
        void CP(const Byte value);
        void INC(Byte& reg);
        void INC_HL();
        void DEC(Byte& reg);
        void DEC_HL();


        // 16-bit arithmetic

        void ADD(const Word value);     // HL += value
        Word ADD();                     // SP + e8 
        void INC(Word& reg);
        void INC(Pair& pair);
        void DEC(Word& reg);
        void DEC(Pair& pair);


        void RLA(bool circular);
        void RL(Byte& reg, bool circular);
        void RRA(bool circular);
        void RR(Byte& reg, bool circular);
        


};

#endif // CPU_H