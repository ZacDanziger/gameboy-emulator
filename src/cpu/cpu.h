#ifndef CPU_H
#define CPU_H

#include "../timer/timer.h"
#include "../memory/mmu.h"

#include <sstream>

using Flag = Bit;

/**
 * Central Processing Unit
 */
class CPU {
    friend class CPULogger;

    public:
        CPU();

        void init(Timer* timer_ptr, MMU* mmu_ptr);

        void step();
        
        bool is_halted() const { return halted; }
        bool is_stopped() const { return stopped; }

        std::string serial_buffer;  // probably don't leave in
    private:
        MMU *mmu;
        Timer *timer;

        // Pair of 8-bit registers
        struct Pair {
            Byte *reg_high;
            Byte *reg_low;
        };

        Flag FLAG_ZERO       = Bit::Bit7;    // 0b10000000
        Flag FLAG_SUB        = Bit::Bit6;    // 0b01000000
        Flag FLAG_HALF_CARRY = Bit::Bit5;    // 0b00100000
        Flag FLAG_CARRY      = Bit::Bit4;    // 0b00010000

        
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
        bool ei_pending;
        bool halted;
        bool stopped;

        // one of the many possible outcomes of STOP is entering HALT mode
        //      which will automatically exit after 0x8000 m-cycles
        bool stop_call_halt_delay;

        constexpr static Byte reset_vector[8]{
            0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38
        };

        constexpr static Byte interrupt_vector[5]{
            0x40,   // IVT[0] - V Blank
            0x48,   // IVT[1] - LCD
            0x50,   // IVT[2] - Timer 
            0x58,   // IVT[3] - Serial
            0x60    // IVT[4] - Joypad
        };
        
        void reset();
        void handle_interrupts();
        
        // Fetch -> Decode -> Execute loop

        Byte fetch();
        Word fetch16();
        void decode_execute(const Byte opcode);
        void decode_execute_cb();

        // Helper Functions

        bool get_flag(const Flag flag) const;
        void update_flag(const Flag flag, bool new_val);
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
        void HALT();
        void STOP();
        void DI();
        void EI();

        // Jumps and Calls

        void JP(const Address address);
        bool JP_IF(const Flag flag, bool set);
        void JR();
        bool JR_IF(const Flag flag, bool set);
        void CALL();
        bool CALL_IF(const Flag flag, bool set);
        void RST(const Byte offset);
        void RET();
        bool RET_IF(const Flag flag, bool set);

        // Bit Operations

        void BIT(const Byte reg, const Bit bit);
        void SET(Byte& reg, const Bit bit);
        void SET_HL(const Bit bit);
        void RES(Byte& reg, const Bit bit);
        void RES_HL(const Bit bit);

        // 8-bit loads

        void LD(Byte& dest, const Byte value);          // dest <- value
        void LD(Byte& dest, const Address address);     // dest <- memory[address]
        void LD(const Address dest, Byte value);        // memory[address] <- value
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