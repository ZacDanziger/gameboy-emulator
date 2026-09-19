#include "cpu.h"
#include "../gameboy.h"

/**
 * Reset the CPU to its post Boot ROM state
 */
void CPU::reset() {
    registers.reset();
    
    clear_pipeline();

    current_opcode = 0x00;

    source_byte_ptr = nullptr;
    dest_byte_ptr = nullptr;
    source_addr_ptr = nullptr;
    dest_addr_ptr = nullptr;

    scratch_register = {0x0000};

    interrupts_enabled = false;
    ei_pending = false;
    halted = false;
    stopped = false;
    speed_switch_halt = false;
}


void CPU::tick(GameBoy& bus) {
    if (pipeline_index >= pipeline_size) {
        clear_pipeline();

        // M-cycle 1
        fetch(bus);
        decode();
    } else {
        execute_state(pipeline[pipeline_index++], bus);
    }


    // Do as many 0 m-cycle ops as possible, stop before doing an m-cycle op
    while ((pipeline_index < pipeline_size) && (!accesses_memory(pipeline[pipeline_index]))) {
        execute_state(pipeline[pipeline_index++], bus);
    }
}


/**
 * Perform one loop of the fetch -> decode -> execute cycle
*/
void CPU::step() {
    if (ei_pending) {
        interrupts_enabled = true;
        ei_pending = false;
    }

    // TODO: move STOP handling out of CPU and into GameBoy, since STOP is a system-wide instruction
    // while(stopped) {
    //     if (joypad.any_button_pressed()) {
    //         stopped = false;
    //     }
    // }

    // TODO: move HALT handling out of CPU and into GameBoy, since HALT is a system-wide instruction
    // TODO: implement halt bug
    // int counter = 0;
    // while(halted) {
    //     // if we entered HALT from a STOP call when a speed switch was requested, exit HALT after 0x8000 m-cycles
    //     if (speed_switch_halt) {
    //         if (counter == 0x8000) {
    //             halted = false;
    //             speed_switch_halt = false;
    //             counter = 0;
    //             break;
    //         }
    //         counter++;
    //     }

    //     if (interrupt.interrupt_pending()) {
    //         handle_interrupts();
    //         halted = false;
    //         break;
    //     }
    //     timer.tick();
    // }

    handle_interrupts();

    Byte opcode = fetch();
    decode_execute(opcode);
}


/**
 * Handle interrupts
 * If IME is enabled and any requesting interrupts are enabled in the IE register,
 * store the current PC and index into the IVT to get the interrupt handler address
 * NOTE: only calls highest priority interrupt, if any
*/
void CPU::handle_interrupts() {
    // check that interrupts are enabled
    if (!interrupts_enabled) {
        return;
    }


    //check that the specific interrupts that are enabled are requesting an interrupt
    if  (!interrupt.interrupt_pending()) {
        return;
    }

    interrupts_enabled = false;

    // 2 m-cycle delay before calling RST
    //  https://gbdev.io/pandocs/Interrupts.html#interrupt-handling
    timer.tick();
    timer.tick();

    switch(interrupt.acknowledge_interrupt()) {
        case Interrupt::VBlank:
            RST(interrupt_vector[0]);
            return;
        case Interrupt::LCDStat:
            RST(interrupt_vector[1]);
            return;
        case Interrupt::Timer:
            RST(interrupt_vector[2]);
            return;
        case Interrupt::Serial:
            RST(interrupt_vector[3]);
            return;
        case Interrupt::Joypad:
            RST(interrupt_vector[4]);
            return;
        default:
            throw std::runtime_error("How did you get here?");
    }
}


void CPU::fetch(GameBoy& bus) {
    current_opcode = bus.read(registers.PC++);
}



/**
 * read the value of the flag stored in register F
 * 
 * @param flag the flag to be read
*/
bool CPU::get_flag(const Flag flag) const {
    return is_set(registers.F, flag);
}


/**
 * update flag to new_val
 * 
 * @param flag the flag to be updated
 * @param new_val the new value of the flag
 */ 
void CPU::update_flag(const Flag flag, bool new_val) {
    if (new_val) {
        set_bit(registers.F, flag);
    } else {
        reset_bit(registers.F, flag);
    }
}



void CPU::queue_load_r16_imm16(Register16* dest_reg) {
    dest_addr_ptr = dest_reg;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // Low <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // High <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++
}
void CPU::queue_store_a_mem_r16(Register16* dest_addr) {
    dest_addr_ptr = dest_addr;
    source_byte_ptr = &registers.A;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Source_Byte_to_Mem_Dest);

    // HL+
    if (current_opcode == 0x22) {
        push_state(CPUState::INC_Dest_Addr);
    // HL-
    } else if (current_opcode == 0x32) {
        push_state(CPUState::DEC_Dest_Addr);
    }
}
void CPU::queue_load_r8_imm8(Byte* dest_byte) {
    dest_byte_ptr = dest_byte;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Byte);  // dest <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);          // PC++
}
void CPU::queue_store_sp_imm16() {
    source_addr_ptr = &registers.PC;
    dest_addr_ptr = &scratch_register;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // scratch[7:0] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // scratch [15:8] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++
    push_state(CPUState::Make_SP_Source_Addr);          // source_addr = SP

    // M-cycle 4
    push_state(CPUState::Source_Addr_Low_to_Mem_Dest);  // Mem[scratch] <- SP[7:0]
    push_state(CPUState::INC_Dest_Addr);                // scratch++

    // M-cycle 5
    push_state(CPUState::Source_Addr_High_to_Mem_Dest); // Mem[scratch] <- SP[15:8]
}
void CPU::queue_load_a_mem_r16(Register16* source_addr) {
    dest_byte_ptr = &registers.A;
    source_addr_ptr = source_addr;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Byte);

    // HL+
    if (current_opcode == 0x2A) {
        push_state(CPUState::INC_Dest_Addr);
    // HL-
    } else if (current_opcode == 0x3A) {
        push_state(CPUState::DEC_Dest_Addr);
    }
    return;
}
void CPU::queue_load_r8_r16(Byte* dest_byte, Register16* source_addr) {
    dest_byte_ptr = dest_byte;
    source_addr_ptr = source_addr;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Byte);
}
void CPU::queue_load_r16_r8(Register16* dest_addr, Byte* source_byte) {
    dest_addr_ptr = dest_addr;
    source_byte_ptr = source_byte;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Source_Byte_to_Mem_Dest);
}
void CPU::queue_pop_r16(Register16* dest_reg) {
    dest_addr_ptr = dest_reg;
    source_addr_ptr = &registers.SP;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // PC[7:0] <- Mem[SP]
    push_state(CPUState::INC_Source_Addr);              // SP++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // PC[15:8] <- Mem[SP]
    push_state(CPUState::INC_Source_Addr);              // SP++
}
void CPU::queue_push_r16(Register16* source_reg) {
    dest_addr_ptr = &registers.SP;
    source_addr_ptr = source_reg;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::InternalDelay);                // m-cycle 2

    // M-cycle 3
    push_state(CPUState::DEC_Dest_Addr);                // SP--
    push_state(CPUState::Source_Addr_High_to_Mem_Dest); // Mem[SP] <- r16[15:8]

    // M-cycle 4
    push_state(CPUState::DEC_Dest_Addr);                // SP--
    push_state(CPUState::Source_Addr_Low_to_Mem_Dest);  // Mem[SP] <- r16[7:0]
}
void CPU::queue_load_imm16_a() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;
    source_byte_ptr = &registers.A;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // scratch[7:0] <- PC
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // scratch[15:8] <- PC
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 4
    push_state(CPUState::Source_Byte_to_Mem_Dest);      // Mem[scratch] <- A
}
void CPU::queue_load_a_imm16() {
    dest_addr_ptr = &scratch_register;
    dest_byte_ptr = &registers.A;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // scratch[7:0] <- PC
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // scratch[15:8] <- PC
    push_state(CPUState::INC_Source_Addr);              // PC++
    push_state(CPUState::Make_Scratch_Source_Addr);     // source_addr = scratch

    // M-cycle 4
    push_state(CPUState::Mem_Source_to_Dest_Byte);      // A <- Mem[scratch]
}
void CPU::queue_load_mem_hl_imm8() {
    dest_addr_ptr = &registers.HL;
    dest_byte_ptr = &scratch_register.low;
    source_addr_ptr = &registers.PC;
    source_byte_ptr = &scratch_register.low;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Byte);      // scratch[7:0] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 3
    push_state(CPUState::Source_Byte_to_Mem_Dest);      // Mem[HL] <- scratch[7:0]
}

void CPU::queue_inc_dec_mem_hl() {
    dest_addr_ptr = &registers.HL;
    dest_byte_ptr = &scratch_register.low;
    source_addr_ptr = &registers.HL;
    source_byte_ptr = &scratch_register.low;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Byte);      // scratch[7:0] <- Mem[HL]

    if (current_opcode == 0x34) {
        push_state(CPUState::INC_Dest_Byte);            // scratch[7:0]++
    } else {
        push_state(CPUState::DEC_Dest_Byte);            // scratch[7:0]--
    }

    // M-cycle 3
    push_state(CPUState::Source_Byte_to_Mem_Dest);      // Mem[HL] <- scratch[7:0]
}


void CPU::queue_jr() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;
    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // scratch[7:0] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++
    push_state(CPUState::JR);                           // PC += (int16_t)scratch

    // M-cycle 3
    push_state(CPUState::InternalDelay);
}
void CPU::queue_jr_cond() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // scratch[7:0] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++
    push_state(CPUState::CheckCond);                    // If COND:
    push_state(CPUState::JR);                           // PC += (int16_t)scratch

    // M-cycle 3
    push_state(CPUState::InternalDelay);
}
void CPU::queue_ret() {
    dest_addr_ptr = &registers.PC;
    source_addr_ptr = &registers.SP;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // PC[7:0] <- Mem[SP]
    push_state(CPUState::INC_Source_Addr);              // SP++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // PC[15:8] <- Mem[SP]
    push_state(CPUState::INC_Source_Addr);              // SP++

    // M-cycle 4
    push_state(CPUState::InternalDelay);
}
void CPU::queue_reti() {
    dest_addr_ptr = &registers.PC;
    source_addr_ptr = &registers.SP;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // PC[7:0] <- Mem[SP]
    push_state(CPUState::INC_Source_Addr);              // SP++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // PC[15:8] <- Mem[SP]
    push_state(CPUState::INC_Source_Addr);              // SP++

    // M-cycle 4
    push_state(CPUState::InternalDelay);
    push_state(CPUState::EI_Immediate);                 // IME = 1
}
void CPU::queue_ret_cond() {
    dest_addr_ptr = &registers.PC;
    source_addr_ptr = &registers.SP;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::InternalDelay);
    push_state(CPUState::CheckCond);

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // PC[7:0] <- Mem[SP]
    push_state(CPUState::INC_Source_Addr);              // SP++

    // M-cycle 4
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // PC[15:8] <- Mem[SP]
    push_state(CPUState::INC_Source_Addr);              // SP++

    // M-cycle 5
    push_state(CPUState::InternalDelay);
}
void CPU::queue_jp() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // scratch[7:0] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // scratch[15:8] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 4
    push_state(CPUState::InternalDelay);
    push_state(CPUState::JP);                           // PC <- scratch
}
void CPU::queue_jp_hl() {
    dest_addr_ptr = &registers.PC;
    source_addr_ptr = &registers.HL;

    // M-cycle 1
    push_state(CPUState::Source_Addr_to_Dest_Addr);
}
void CPU::queue_jp_cond() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // scratch[7:0] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // scratch[15:8] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++
    push_state(CPUState::CheckCond);                    // If COND:

    // M-cycle 4
    push_state(CPUState::InternalDelay);
    push_state(CPUState::JP);                           // PC <- scratch
}
void CPU::queue_call() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // scratch[7:0] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // scratch[15:8] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 4
    push_state(CPUState::InternalDelay);
    push_state(CPUState::Make_SP_Dest_Addr);            // dest_addr = SP
    push_state(CPUState::DEC_Dest_Addr);                // SP--

    // M-cycle 5
    push_state(CPUState::Source_Addr_High_to_Mem_Dest); // Mem[SP] <- PC[15:8]
    push_state(CPUState::DEC_Dest_Addr);                // SP--

    // M-cycle 6
    push_state(CPUState::Source_Addr_Low_to_Mem_Dest);  // Mem[SP] <- PC[7:0]
    push_state(CPUState::JP);                           // PC <- scratch
}
void CPU::queue_call_cond() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Addr_Low);  // scratch[7:0] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++

    // M-cycle 3
    push_state(CPUState::Mem_Source_to_Dest_Addr_High); // scratch[15:8] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++
    push_state(CPUState::CheckCond);                    // If COND:

    // M-cycle 4
    push_state(CPUState::InternalDelay);                
    push_state(CPUState::Make_SP_Dest_Addr);            // dest_addr = SP
    push_state(CPUState::DEC_Dest_Addr);                // SP--

    // M-cycle 5
    push_state(CPUState::Source_Addr_High_to_Mem_Dest); // Mem[SP] <- PC[15:8]
    push_state(CPUState::DEC_Dest_Addr);                // SP--

    // M-cycle 6
    push_state(CPUState::Source_Addr_Low_to_Mem_Dest);  // Mem[SP] <- PC[7:0]
    push_state(CPUState::JP);                           // PC <- scratch
}
void CPU::queue_rst() {
    dest_addr_ptr = &registers.SP;
    source_addr_ptr = &registers.PC;

    // calculate jump address from current_opcode
    // https://gbdev.io/pandocs/CPU_Instruction_Set.html#block-3
    scratch_register.word = (Address)(current_opcode & 0x38);

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::InternalDelay);
    push_state(CPUState::DEC_Dest_Addr);                // SP--

    // M-cycle 3
    push_state(CPUState::Source_Addr_High_to_Mem_Dest); // Mem[SP] <- PC[15:8]
    push_state(CPUState::DEC_Dest_Addr);                // SP--

    // M-cycle 4
    push_state(CPUState::Source_Addr_Low_to_Mem_Dest);  // Mem[SP] <- PC[7:0]
    push_state(CPUState::JP);                           // PC <- scratch
}

void CPU::queue_load_r8_r8(Byte* dest_byte, Byte* source_byte) {
    dest_byte_ptr = dest_byte;
    source_byte_ptr = source_byte;

    // M-cycle 1
    push_state(CPUState::Source_Byte_to_Dest_Byte);
}
void CPU::queue_load_sp_hl() {
    source_addr_ptr = &registers.HL;
    dest_addr_ptr = &registers.SP;

    // M-cycle 1
    push_state(CPUState::Source_Addr_to_Dest_Addr);     // SP <- HL

    // M-cycle 2
    push_state(CPUState::InternalDelay);
}

void CPU::queue_alu_op(Byte* source_byte) {
    source_byte_ptr = source_byte;

    // M-cycle 1
    push_state(CPUState::EXEC_ALU);
}
void CPU::queue_alu_imm8() {
    source_addr_ptr = &registers.PC;
    source_byte_ptr = &scratch_register.low;
    dest_byte_ptr = &scratch_register.low;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_state(CPUState::Mem_Source_to_Dest_Byte);      // scratch[7:0] <- Mem[PC]
    push_state(CPUState::INC_Source_Addr);              // PC++
    push_state(CPUState::EXEC_ALU);                     // Do ALU op on imm8
}
void CPU::queue_inc_r16(Register16* dest_reg) {
    dest_addr_ptr = dest_reg;

    // M-cycle 1
    push_state(CPUState::INC_Dest_Addr);

    // M-cycle 2
    push_state(CPUState::InternalDelay);
}
void CPU::queue_dec_r16(Register16* dest_reg) {
    dest_addr_ptr = dest_reg;

    // M-cycle 1
    push_state(CPUState::DEC_Dest_Addr);

    // M-cycle 2
    push_state(CPUState::InternalDelay);
}
void CPU::queue_inc_r8(Byte* dest_byte) {
    dest_byte_ptr = dest_byte;

    // M-cycle 1
    push_state(CPUState::INC_Dest_Byte);
}
void CPU::queue_dec_r8(Byte* dest_byte) {
    dest_byte_ptr = dest_byte;

    // M-cycle 1
    push_state(CPUState::DEC_Dest_Byte);
}
void CPU::queue_add_hl_r16(Register16* source_reg) {
    source_addr_ptr = source_reg;

    // M-cycle 1
    push_state(CPUState::ADD_HL);

    // M-cycle 2
    push_state(CPUState::InternalDelay);
}


/* --------------------
 * Z - Zero Flag
 * N - Subtraction Flag
 * H - Half-Carry Flag
 * C - Carry Flag
 * 
 * 0 - reset
 * 1 - set
 * - - no change
 * ---------------------
*/ 


/**
 * Swap the upper and lower nibbles of the Byte passed in (usually a register)
 * Z 0 0 0
 * 
 * @param value the address of the Byte to be swapped
 * 
*/
void CPU::SWAP(Byte& value) {
    value = ((value & 0x0F) << 4) | ((value & 0xF0) >> 4);

    update_flag(FLAG_ZERO, value == 0x00);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, false);
}

/**
 * Swap the upper and lower nibbles of memory[HL]
 * Z 0 0 0
*/
void CPU::SWAP_HL() {
    Byte value = read_hl();
    SWAP(value);
    LD(get_pair(HL), value);
}


/**
 * Decimal Adjust Accumulator
 * Z - 0 C
*/
void CPU::DAA() {
    Byte adjust = 0x00;
    bool sub = get_flag(FLAG_SUB);
    bool carry = false;

    if (sub) {
        if (get_flag(FLAG_HALF_CARRY)) {
            adjust += 0x06;
        }
        if (get_flag(FLAG_CARRY)) {
            adjust += 0x60;
            carry = true;
        }

        registers.A -= adjust;
    } else {
        if (get_flag(FLAG_HALF_CARRY) || ((registers.A & 0xF) > 0x9)) {
            adjust += 0x06;
        }
        if (get_flag(FLAG_CARRY) || (registers.A > 0x99)) {
            adjust += 0x60;
            carry = true;
        }

        registers.A += adjust;
    }

    update_flag(FLAG_ZERO, registers.A == 0);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, carry);
}


/**
 * Complement Accumulator (register A)
 * - 1 1 -
*/
void CPU::CPL() {
    registers.A = ~registers.A;

    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, true);
}


/**
 * Complement Carry Flag
 * - 0 0 C
*/
void CPU::CCF() {
    bool old_val = get_flag(FLAG_CARRY);

    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, (1 - old_val));
}


/**
 * Set Carry Flag
 * - 0 0 1
*/
void CPU::SCF() {
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, true);
}


/**
 * Halts the program, still listening for interrupts
 * - - - -
*/
void CPU::HALT() {
    halted = true;
}


/**
 * Stops the program
 * - - - -
 * 
 * https://gbdev.io/pandocs/Reducing_Power_Consumption.html#the-bizarre-case-of-the-game-boy-stop-instruction-before-even-considering-timing
 */
void CPU::STOP() {
    // check if a button is being pressed
    if (joypad.any_button_pressed()) {
        if (interrupt.interrupt_pending()) {
            // stop is a 1 byte opcode, mode doesn't change, DIV is not reset
            return;
        } 

        // stop is a 2 byte opcode, HALT mode is entered, DIV is not reset
        reg_PC++;
        halted = true;
        return;
    }

    // check if a speed switch is requested
    if (is_set(memory_bus.read(KEY1_SPD_REGISTER), Bit::Bit0)) {
        if (interrupt.interrupt_pending()) {
            if (interrupts_enabled) {
                // stop is a 1 byte opcode, mode doesn't change, DIV is reset, CPU speed switches
                memory_bus.write(DIV_REGISTER, 0x00);

                // true if currently double speed, false if currently normal speed
                bool current_speed = timer.get_double_speed();

                // change current speed
                timer.set_double_speed(!current_speed);

                // clear the switch armed bit in KEY1
                memory_bus.write(KEY1_SPD_REGISTER, 0x00);
            }

            // CPU glitches non-deterministically
            // I'm just going to return here and not worry about that
            return;
        }

        // stop is a 2 byte opcode, HALT mode is entered, DIV is reset, CPU speed switches
        reg_PC++;
        halted = true;
        speed_switch_halt = true;
        memory_bus.write(DIV_REGISTER, 0x00);

        // true if currently double speed, false if currently normal speed
        bool current_speed = timer.get_double_speed();

        // change current speed
        timer.set_double_speed(!current_speed);
        return;
    }

    if (interrupt.interrupt_pending()) {
        // stop is a 1 byte opcode, STOP mode is entered, DIV is reset
        stopped = true;
        memory_bus.write(DIV_REGISTER, 0x00);
        return;
    }

    // stop is a 2 byte opcode, STOP mode is entered, DIV is reset
    reg_PC++;
    stopped = true;
    memory_bus.write(DIV_REGISTER, 0x00);
}


/**
 * Disable interrupts
 * - - - -
*/
void CPU::DI() {
    interrupts_enabled = false;
}


/**
 * Enable interrupts 1 m-cycle after this is executed
 * - - - -
*/
void CPU::EI() {
    ei_pending = true;
}


/**
 * Check whether bit[pos] has been set, update Z to match
 * Z 0 1 -
 * 
 * @param pos the bit position to check, in range [0, 7]
 * @param reg the register to check the bit of
*/
void CPU::BIT(const Byte reg, const Bit bit) {
    bool set = is_set(reg, bit);

    update_flag(FLAG_ZERO, (1 - set));
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, true);
}


/**
 * Sets registers's bit[pos] 
 * - - - -
 * 
 * @param pos the bit position to set, in range [0, 7]
 * @param reg the register to set the bit of
*/
void CPU::SET(Byte& reg, const Bit bit) {
    set_bit(reg, bit);
}


/**
 * Set memory[HL]'s bit[pos]
 * - - - -
 * 
 * @param pos the bit position to set in memory[HL]
*/
void CPU::SET_HL(const Bit bit) {
    Byte value = read_hl();
    SET(value, bit);
    LD(get_pair(HL), value);
}


/**
 * Resets register's bit[pos]
 * - - - -
 * 
 * @param pos the bit position to reset, in range [0, 7]
 * @param reg the register to reset the bit of
*/
void CPU::RES(Byte& reg, const Bit bit) {
    reset_bit(reg, bit);
}


/**
 * Reset memory[HL]'s bit[pos]
 * - - - -
 * 
 * @param pos the bit position to reset in memory[HL]
*/
void CPU::RES_HL(const Bit bit) {
    Byte value = read_hl();
    RES(value, bit);
    LD(get_pair(HL), value);
}



/**
 * 8-bit addition with and without carry
 * Z 0 H C
 * 
 * @param value the byte to be added to the accumulator (reg A) (usually a register, though can be imm value)
 * @param carry whether or not to add the carry flag to result, true if ADC, false if ADD
*/
void CPU::ADD(const Byte value, bool carry) {
    // do math in 16-bit to check for carrys on bit [8]
    uint16_t res = registers.A + value;

    if (carry) {
        // update carry to the value stored in reg_F
        carry = get_flag(FLAG_CARRY) ? 1 : 0;
        res += carry;
    }

    // check for carries from bit [3] to bit [4]
    bool half_carry = ((registers.A & 0xF) + (value & 0xF) + carry) > 0xF;

    // update A to bits [7:0] of res
    registers.A = res & 0xFF;

    update_flag(FLAG_ZERO, registers.A == 0); // true if bits [7:0] of res are 0 (i.e. including wrappings)
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);   
    update_flag(FLAG_CARRY, (res >> 8) != 0);  // check bit [8] of res for carry
}


/**
 * 8-bit subtraction with and without carry
 * Z 1 H C
 * 
 * @param value the byte to be subtracted from the accumulator (reg A) (usually a register, though can be imm value)
 * @param carry whether or not to subtract the carry flag from result, true if SBC, false if SUB
*/
void CPU::SUB(const Byte value, bool carry) {
    // do math in 16-bit to check for carrys on bit [8]
    uint16_t res = registers.A - value;

    if (carry) {
        // update carry to the value stored in reg_F
        carry = get_flag(FLAG_CARRY) ? 1 : 0;
        res -= carry;
    }

    bool half_carry = ((value & 0xF) + carry) > (registers.A & 0xF);   // check for borrows from bit [4]
    // update carry (again) to whether a borrow occured from bit [8]
    carry = ((value + carry) > registers.A);

    registers.A = res & 0xFF;

    update_flag(FLAG_ZERO, registers.A == 0);
    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, half_carry);
    update_flag(FLAG_CARRY, carry);
}


/**
 * 8-bit bitwise and
 * Z 0 1 0
 * 
 * @param value the byte to be and'd with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::AND(const Byte value) {
    registers.A &= value;

    update_flag(FLAG_ZERO, registers.A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, true);
    update_flag(FLAG_CARRY, false);  
}


/**
 * 8-bit bitwise xor
 * Z 0 0 0
 * 
 * @param value the byte to be xor'd with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::XOR(const Byte value) {
    registers.A ^= value;

    update_flag(FLAG_ZERO, registers.A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, false);
}


/**
 * 8-bit bitwise or
 * Z 0 0 0
 * 
 * @param value the byte to be or'd with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::OR(const Byte value) {
    registers.A |= value;

    update_flag(FLAG_ZERO, registers.A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, false);
}


/**
 * 8-bit compare
 * Z 1 H C
 * 
 * @param value the byte to be compared with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::CP(const Byte value) {
    // do math in 16-bit to check for carrys on bit [8]
    uint16_t temp = registers.A - value;

    update_flag(FLAG_ZERO, (temp & 0xFF) == 0);
    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, (value & 0xF) > (registers.A & 0xF));
    update_flag(FLAG_CARRY, value > registers.A);
}


/**
 * 8-bit increment
 * Z 0 H -
 * 
 * @param reg the register value to be incremented
*/
void CPU::INC(Byte *reg) {
    bool half_carry = ((*reg) & 0xF) == 0xF;
    *reg += 1;

    update_flag(FLAG_ZERO, (*reg) == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);
}


/**
 * 8-bit decrement
 * Z 1 H -
 * 
 * @param reg the register value to be decremented
*/
void CPU::DEC(Byte *reg) {
    bool half_carry = ((*reg) & 0xF) == 0;
    *reg -= 1;

    update_flag(FLAG_ZERO, (*reg) == 0);
    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, half_carry);
}


/**
 * 16-bit addition
 * Adds signed 8-bit imm value to SP
 * 0 0 H C
 * 
 * @return SP + e8
*/
Word CPU::ADD_SP() {
    // Get signed one byte immediate value
    int16_t imm = (int16_t)(int8_t)(fetch());
    Word res = (Word)((int16_t)(reg_SP) + imm);

    timer.tick();

    bool carry = (res & 0xFF) < (reg_SP & 0xFF);
    bool half_carry = (res & 0xF) < (reg_SP & 0xF);

    update_flag(FLAG_ZERO, false);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);
    update_flag(FLAG_CARRY, carry);

    return res;
}



/**
 * Rotate Left, Register A
 * (Both circular and non circular)
 * Differs from RL(A) in that the Zero flag is always reset
 * 0 0 0 C 
 * 
 * @param circular true if opcode is RLCA, false if RLA
*/
void CPU::RLA(bool circular) {
    bool bit_7 = ((registers.A & 0x80) >> 7);
    registers.A = ((registers.A & 0x7F) << 1);
    if (circular) {
        // RLCA
        registers.A |= bit_7;
    } else {
        // RLA
        registers.A |= get_flag(FLAG_CARRY);
    }
    

    update_flag(FLAG_ZERO, false);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, bit_7);
}


/**
 * Rotate Left
 * (Both circular and non circular)
 * Z 0 0 C
 * 
 * @param reg the register to be rotated
 * @param circular true if opcode is RLC, false if RL
*/
void CPU::RL(Byte &reg, bool circular) {
    bool bit_7 = (reg >> 7);
    reg = (reg << 1);
    if (circular) {
        // RLC
        reg |= bit_7;
    } else {
        // RL
        reg |= get_flag(FLAG_CARRY);
    }

    update_flag(FLAG_ZERO, (reg == 0));
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, bit_7);
}


/**
 * Rotate memory[HL] Left
 * Z 0 0 C
 * 
 * @param circular true for RLC, false for RL
*/
void CPU::RL_HL(bool circular) {
    Byte value = read_hl();
    RL(value, circular);
    LD(get_pair(HL), value);
}


/**
 * Rotate Right, Register A
 * (Both circular and non circular)
 * Differs from RR(A) in that the Zero flag is always reset
 * 0 0 0 C 
 * 
 * @param circular true if opcode is RRCA, false if RRA
*/
void CPU::RRA(bool circular) {
    bool bit_0 = registers.A & 0b1;
    registers.A = (registers.A >> 1);
    if (circular) {
        // RRCA
        registers.A = (registers.A & ~(1 << 7) | (bit_0 << 7));
    } else {
        // RRA
        registers.A = (registers.A & ~(1 << 7) | (get_flag(FLAG_CARRY) << 7));
    }

    update_flag(FLAG_ZERO, false);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, bit_0);
}


/**
 * Rotate Right
 * (Both circular and non circular)
 * Z 0 0 C
 * 
 * @param reg the register to be rotated
 * @param circular true if opcode is RRC, false if RR
*/
void CPU::RR(Byte &reg, bool circular) {
    bool bit_0 = reg & 0b1;
    reg = (reg >> 1);
    if (circular) {
        // RRC
        reg = (reg & ~(1 << 7) | (bit_0 << 7));
    } else {
        // RR
        reg = (reg & ~(1 << 7) | (get_flag(FLAG_CARRY) << 7));
    }

    update_flag(FLAG_ZERO, (reg == 0));
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, bit_0);
}

/**
 * Rotate memory[HL] right
 * Z 0 0 C
 * 
 * @param circular true for RRC, false for RR
*/
void CPU::RR_HL(bool circular) {
    Byte value = read_hl();
    RR(value, circular);
    LD(get_pair(HL), value);
}


/**
 * Shift reg Left Arithmetically
 * C <- b7 <- ... <- b0 <- 0
 * Z 0 0 C
 * 
 * @param reg the register containing the value to shift
*/
void CPU::SLA(Byte& reg) {
    bool bit_7 = (reg >> 7);
    reg = (reg << 1);

    update_flag(FLAG_ZERO, reg == 0x00);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, bit_7);
}


/**
 * Shift memory[HL] Left Arithmetically
 * Z 0 0 C
*/
void CPU::SLA_HL() {
    Byte value = read_hl();
    SLA(value);
    LD(get_pair(HL), value);
}


/**
 * Shift reg Right Arithmetically
 * b7 -> ... -> b0 -> C (b7 stays the same)
 * Z 0 0 C
 * 
 * @param reg the register containing the value to shift
*/
void CPU::SRA(Byte& reg) {
    bool bit_0 = reg & 0b1;
    bool bit_7 = (reg >> 7);
    reg = (reg >> 1);
    reg |= (bit_7 << 7);

    update_flag(FLAG_ZERO, reg == 0x00);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, bit_0);
}


/**
 * Shift memory[HL] Right Arithmetically
 * Z 0 0 C
*/
void CPU::SRA_HL() {
    Byte value = read_hl();
    SRA(value);
    LD(get_pair(HL), value);
}


/**
 * Shift reg Right Logically
 * 0 -> b7 -> ... -> b0 -> C
 * Z 0 0 C
 * 
 * @param reg the register containing the value to shift
*/
void CPU::SRL(Byte& reg) {
    bool bit_0 = reg & 0b1;
    reg = (reg >> 1);

    update_flag(FLAG_ZERO, reg == 0x00);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, bit_0);
}


/**
 * Shift memory[HL] Right Logically
 * Z 0 0 C
*/
void CPU::SRL_HL() {
    Byte value = read_hl();
    SRL(value);
    LD(get_pair(HL), value);
}