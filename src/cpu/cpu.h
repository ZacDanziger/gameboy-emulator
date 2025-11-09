#ifndef CPU_H
#define CPU_H

#include "../memory/memory.h"
#include <sstream>
#include <iostream>

// Set to true in debugging mode
const bool DEBUG = true;

const uint8_t FLAG_ZERO       = 0b10000000;
const uint8_t FLAG_SUB        = 0b01000000;
const uint8_t FLAG_HALF_CARRY = 0b00100000;
const uint8_t FLAG_CARRY      = 0b00010000;

class CPU {
    public:
        CPU();

        void load(const Address address, const std::string& filename);
        void load_rom(const std::string& filename);
        int step();
        void write_registers();
        void set_logfile_suffix(const std::string suffix);

        bool is_halted() { return halted; }

    private:
        std::string _logfile;

        Memory _memory;

        // Pair of 8-bit registers
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

        bool interrupts_enabled;    // IME
        bool halted;

        constexpr static Byte interrupt_vector[13]{
            0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, // IVT[0:7] - Various resets
            0x40,   // IVT[8]  - V Blank
            0x48,   // IVT[9]  - LCD
            0x50,   // IVT[10] - Timer 
            0x58,   // IVT[11] - Serial
            0x60    // IVT[12] - Joypad
        };


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

        void SWAP(Byte& value);
        void SWAP_HL();
        void DAA();
        void CPL();
        void CCF();
        void SCF();
        void HALT();    // not implemented
        void STOP();    // not implemented
        void DI();
        void EI();

        // Jumps and Calls

        void JP(const Address address);
        bool JP_COND(const uint16_t flag, bool set);
        void JR();
        bool JR_COND(const uint16_t flag, bool set);
        void CALL();
        bool CALL_COND(const uint16_t flag, bool set);
        void RST(const Byte offset);
        void RET();
        bool RET_COND(const uint16_t flag, bool set);

        // Bit Operations

        void BIT(int pos, const Byte reg);
        void SET(int pos, Byte& reg);
        void SET_HL(int pos);
        void RES(int pos, Byte& reg);
        void RES_HL(int pos);

        // 8-bit loads

        void LD(Byte& dest, const Byte value);          // dest <- value
        void LD(Byte& dest, const Address address);     // dest <- memory[address]
        void LD(const Address dest, const Byte value);  // memory[address] <- value
        void LDH(const Byte reg, bool into_A);

        // 16-bit loads

        void LD(Word& dest, const Word value);          // dest <- value
        void LD(Pair& pair, const Word value);          // [high | low] <- [value 15:8 | value 7:0]
        void write_SP(const Address address);
        void PUSH(const Pair& pair);
        void PUSH_PC();
        void POP(Pair& pair);

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

        void ADD_HL(const Word value);
        Word ADD_SP();
        void INC(Word& reg);
        void INC(Pair& pair);
        void DEC(Word& reg);
        void DEC(Pair& pair);

        // Rotates and Shifts

        void RLA(bool circular);
        void RL(Byte& reg, bool circular);
        void RL_HL(bool circular);
        void RRA(bool circular);
        void RR(Byte& reg, bool circular);
        void RR_HL(bool circular);
        void SLA(Byte& reg);
        void SLA_HL();
        void SRA(Byte& reg);
        void SRA_HL();
        void SRL(Byte& reg);
        void SRL_HL();
};

#endif // CPU_H