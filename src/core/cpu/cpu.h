#ifndef CPU_H
#define CPU_H

#include <functional>

#include "../types.h"
#include "../memory_map.h"

#include "../interrupt/interrupt_controller.h"
#include "../bus/memory_bus.h"

#include "microops.h"
#include "cpu_registers.h"

constexpr int MAX_QUEUE_SIZE = 16;    // current highest queue size required = 12
constexpr uint16_t HALT_TIMER_DELAY = 0x77FE; // 0x8000 m-cycles - 0x0802 m-cycles that CPU does not tick during speed switch

using Flag = Bit;

constexpr Flag FLAG_ZERO       = Bit::Bit7;    // 0b10000000
constexpr Flag FLAG_SUB        = Bit::Bit6;    // 0b01000000
constexpr Flag FLAG_HALF_CARRY = Bit::Bit5;    // 0b00100000
constexpr Flag FLAG_CARRY      = Bit::Bit4;    // 0b00010000

/**
 * Central Processing Unit
 */
class CPU {
    public:
        CPU(InterruptController& interrupt_controller, MemoryBus& memory_bus):
            interrupt(interrupt_controller),
            bus(memory_bus),
            registers(),

            microcode_queue{},
            queue_size(0),
            queue_index(0),

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
            halt_countdown_timer(0x0000),
            speed_switch_pending(false)
        {}

        void reset();
        
        void tick();
        
        Word get_pc() const { return registers.PC.word; } 

        bool is_halted() const { return halted; }
        void clear_halted() { halted = false; }
        bool is_stopped() const { return stopped; }
        void clear_stopped() { stopped = false; }

        bool is_speed_switch_pending() const { return speed_switch_pending; }
        void clear_speed_switch_pending() { speed_switch_pending = false; }
    private:
        InterruptController& interrupt;
        MemoryBus& bus;
        Registers registers;

        MicroOp microcode_queue[MAX_QUEUE_SIZE];
        uint8_t queue_size;
        uint8_t queue_index;

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
        uint16_t halt_countdown_timer;
        bool speed_switch_pending;
        
        void handle_interrupts();
        
        // Fetch -> Decode -> Execute loop

        void fetch();
        void decode();
        void decode_cb();
        void execute_microop(const MicroOp microop);
        
        bool accesses_memory(const MicroOp microop) {return microop <= MicroOp::WriteMemFromSourceHigh; }
        
        // Queue Management
        void push_microop(const MicroOp microop);
        void clear_queue() { queue_size = 0; queue_index = 0; }

        // Helper Functions

        bool get_flag(const Flag flag) const;
        void update_flag(const Flag flag, bool new_val);

        void execute_stop();

        /** -------------------
         * Instruction Queues
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
        void queue_load_mem_hl_imm8();
        void queue_load_high_a_imm8();
        void queue_load_high_imm8_a();
        void queue_load_high_a_c();
        void queue_load_high_c_a();
        void queue_prefix_cb();
        void queue_interrupt(const Byte interrupt_vector);

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
        void queue_add_sp();
        void queue_load_hl_sp_e8();

        // Math & Logic operations

        void queue_alu_op(Byte* source_byte);
        void queue_alu_op_mem_hl();
        void queue_af_op();
        void queue_alu_imm8();
        void queue_inc_r16(Register16* dest_reg);
        void queue_dec_r16(Register16* dest_reg);
        void queue_inc_r8(Byte* dest_byte);
        void queue_dec_r8(Byte* dest_byte);
        void queue_add_hl_r16(Register16* source_reg);
        void queue_inc_dec_mem_hl();
        void queue_cb_op(Byte* dest_byte);
        void queue_cb_op_mem_hl();
        void queue_cb_bit_mem_hl();

        // Control and Miscellaneous Instructions

        void DAA();
        void CPL();
        void CCF();
        void SCF();

        void DI();
        void EI();

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

        // CB operations

        void RL(Byte& reg, bool circular);
        void RR(Byte& reg, bool circular);
        void SLA(Byte& reg);
        void SRA(Byte& reg);
        void SRL(Byte& reg);
        void SWAP(Byte& value);
        void BIT(const Byte reg, const Bit bit);
        void SET(Byte& reg, const Bit bit);
        void RES(Byte& reg, const Bit bit);
};

#endif // CPU_H