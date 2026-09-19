#ifndef CPU_H
#define CPU_H

#include <functional>

#include "../types.h"
#include "cpu_state.h"
#include "cpu_registers.h"

class GameBoy;

static constexpr int MAX_PIPELINE_SIZE = 12;    // may need to update as time goes on

using Flag = Bit;

static constexpr Flag FLAG_ZERO       = Bit::Bit7;    // 0b10000000
static constexpr Flag FLAG_SUB        = Bit::Bit6;    // 0b01000000
static constexpr Flag FLAG_HALF_CARRY = Bit::Bit5;    // 0b00100000
static constexpr Flag FLAG_CARRY      = Bit::Bit4;    // 0b00010000

/**
 * Central Processing Unit
 */
class CPU {
    public:
    //test
        CPU() :
            registers(),

            pipeline{},
            pipeline_size(0),
            pipeline_index(0),

            current_opcode(0x00),

            source_byte_ptr(nullptr),
            dest_byte_ptr(nullptr),
            source_addr_ptr(nullptr),
            dest_addr_ptr(nullptr),

            scratch_register(),

            interrupts_enabled(false),
            ei_pending(false),
            halted(false),
            stopped(false),
            speed_switch_halt(false)
        {}

        void reset();
        
        void tick(GameBoy& bus);
        void step();
        
        bool is_halted() const { return halted; }
        bool is_stopped() const { return stopped; }

    private:
        CPUState pipeline[MAX_PIPELINE_SIZE];
        uint8_t pipeline_size;
        uint8_t pipeline_index;

        Registers registers;

        Byte current_opcode;

        Byte* source_byte_ptr;
        Byte* dest_byte_ptr;
        Register16* source_addr_ptr;
        Register16* dest_addr_ptr;

        Register16 scratch_register;

        bool interrupts_enabled;    // IME
        bool ei_pending;
        bool halted;
        bool stopped;

        // one of the many possible outcomes of STOP is entering HALT mode
        //      which will automatically exit after 0x8000 m-cycles
        bool speed_switch_halt;

        constexpr static Byte interrupt_vector[5]{
            0x40,   // IVT[0] - V Blank
            0x48,   // IVT[1] - LCD
            0x50,   // IVT[2] - Timer 
            0x58,   // IVT[3] - Serial
            0x60    // IVT[4] - Joypad
        };
        
        void handle_interrupts();
        
        // Fetch -> Decode -> Execute loop

        void fetch(GameBoy& bus);
        void decode();
        void decode_cb();
        void execute_state(const CPUState state, GameBoy& bus);
        
        Byte* get_register_by_id(uint8_t id);
        Register16* get_register16_by_id(uint8_t id);
        bool accesses_memory(const CPUState state) {return state <= CPUState::Source_Addr_High_to_Mem_Dest; }
        
        void push_state(const CPUState state) { pipeline[pipeline_index++] = state; }
        void clear_pipeline() { pipeline_size = 0; pipeline_index = 0; }

        // Helper Functions

        bool get_flag(const Flag flag) const;
        void update_flag(const Flag flag, bool new_val);
        Byte read_hl() const;

        /** -------------------
         * Instruction Decoders
         * ------------------- */ 

        // Memory Accesses

        void queue_load_r16_imm16(Register16* dest_reg);
        void queue_store_a_mem_r16(Register16* dest_addr);
        void queue_load_r8_imm8(Byte* dest_byte);
        void queue_store_sp_imm16();
        void queue_load_a_mem_r16(Register16* source_addr);
        void queue_load_r8_r16(Byte* dest_byte, Register16* source_addr);
        void queue_load_r16_r8(Register16* dest_addr, Byte* source_byte);
        void queue_pop_r16(Register16* dest_reg);
        void queue_push_r16(Register16* source_reg);
        void queue_load_imm16_a();
        void queue_load_a_imm16();
        void load_mem_hl_imm8();

        // Jumps and Discontinuities

        void queue_jr();
        void queue_jr_cond();
        void queue_ret();
        void queue_reti();
        void queue_ret_cond();
        void queue_jp();
        void queue_jp_hl();
        void queue_jp_cond();
        void queue_call();
        void queue_call_cond();
        void queue_rst();

        // Register to Register

        void queue_load_r8_r8(Byte* dest_byte, Byte* source_byte);
        void queue_load_sp_hl();

        // Math & Logic operations

        void queue_alu_op(Byte* source_byte);
        void queue_alu_imm8();
        void queue_inc_r16(Register16* dest_reg);
        void queue_dec_r16(Register16* dest_reg);
        void queue_inc_r8(Byte* dest_byte);
        void queue_dec_r8(Byte* dest_byte);
        void queue_add_hl_r16(Register16* source_reg);
        void queue_inc_dec_mem_hl();

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

        // Bit Operations

        void BIT(const Byte reg, const Bit bit);
        void SET(Byte& reg, const Bit bit);
        void SET_HL(const Bit bit);
        void RES(Byte& reg, const Bit bit);
        void RES_HL(const Bit bit);

        // 8-bit arithmetic

        void ADD(const Byte value, bool carry);
        void SUB(const Byte value, bool carry);
        void AND(const Byte value);
        void XOR(const Byte value);
        void OR(const Byte value);
        void CP(const Byte value);
        void INC(Byte* reg);
        void DEC(Byte* reg);

        // 16-bit arithmetic

        void ADD_HL(const Word value);
        Word ADD_SP();

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