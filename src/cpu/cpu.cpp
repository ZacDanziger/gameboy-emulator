#include "cpu.h"
#include <iostream>

/**
 * Reset the CPU to its post Boot ROM state
 */
void CPU::reset() {
    reg_A = 0x11;
    reg_F = 0x80;
    reg_B = 0x00;
    reg_C = 0x00;
    reg_D = 0xFF;
    reg_E = 0x56;
    reg_H = 0x00;
    reg_L = 0x0D;
    reg_SP = 0xFFFE;
    reg_PC = 0x0100;
    
    interrupts_enabled = false;
    ei_pending = false;
    halted = false;
    stopped = false;
    speed_switch_halt = false;
}


/**
 * Perform one loop of the fetch -> decode -> execute cycle
*/
void CPU::step() {
    if (ei_pending) {
        interrupts_enabled = true;
        ei_pending = false;
    }

    while(stopped) {
        if (joypad.any_button_pressed()) {
            stopped = false;
        }
    }

    // TODO: implement halt bug
    int counter = 0;
    while(halted) {
        // if we entered HALT from a STOP call when a speed switch was requested, exit HALT after 0x8000 m-cycles
        if (speed_switch_halt) {
            if (counter == 0x8000) {
                halted = false;
                speed_switch_halt = false;
                counter = 0;
                break;
            }
            counter++;
        }

        if (interrupt.interrupt_pending()) {
            handle_interrupts();
            halted = false;
            break;
        }
        timer.tick();
    }

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


/**
 * Fetch the next instruction and increment PC
 * 
 * @return the value in mem[PC]
*/
Byte CPU::fetch() {
    Byte opcode = memory_bus.read(reg_PC++);
    timer.tick();
    return opcode;
}


/**
 * Fetch the next 16 bits, in little endian format
 * 
 * @return [mem[PC+1] | mem[PC]]
*/
Word CPU::fetch16() {
    Byte low = fetch();
    return ((fetch() << 8) | low);
}

/**
 * read the value of the flag stored in register F
 * 
 * @param flag the flag to be read
*/
bool CPU::get_flag(const Flag flag) const {
    return is_set(reg_F, flag);
}


/**
 * update flag to new_val
 * 
 * @param flag the flag to be updated
 * @param new_val the new value of the flag
 */ 
void CPU::update_flag(const Flag flag, bool new_val) {
    if (new_val) {
        set_bit(reg_F, flag);
    } else {
        reset_bit(reg_F, flag);
    }
}


/**
 * Get the 16-bit represtation of the pair
 * 
 * @return [reg_high | reg_low]
*/
Word CPU::get_pair(const Pair& pair) const {
    return ((*(pair.reg_high) << 8) | *(pair.reg_low));
}


/**
 * Update the value stored in pair to value
 * reg_high = value[15:8]
 * reg_low = value [7:0]
 * 
 * @param pair the pair to be updated
 * @param value the new value to set the pair to
*/
void CPU::set_pair(const Pair &pair, const Word value) {
    *(pair.reg_high) = (value >> 8);
    *(pair.reg_low) = (value & 0xFF);
}


/***
 * Get the value in memory stored in [HL]
 * 
 * @return memory[HL]
*/
Byte CPU::read_hl() const {
    Byte res = memory_bus.read(get_pair(HL));
    timer.tick();
    return res;
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

        reg_A -= adjust;
    } else {
        if (get_flag(FLAG_HALF_CARRY) || ((reg_A & 0xF) > 0x9)) {
            adjust += 0x06;
        }
        if (get_flag(FLAG_CARRY) || (reg_A > 0x99)) {
            adjust += 0x60;
            carry = true;
        }

        reg_A += adjust;
    }

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, carry);
}


/**
 * Complement Accumulator (register A)
 * - 1 1 -
*/
void CPU::CPL() {
    reg_A = ~reg_A;

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
 * Jump -> PC = value
 * - - - -
 * 
 * @param address the address in memory to jump PC to
*/
void CPU::JP(const Address address) {
    reg_PC = address;
}


/**
 * Conditional Jump - check that flag is either set or unset, depending on set param
 *     if yes, then JP(imm16), otherwise increment PC by 2 and return
 * - - - -
 * 
 * @param flag the flag to check the value of in Register F
 * @param set the boolean value to check the flag against
 * @return true if a jump occured, false otherwise
*/
bool CPU::JP_IF(const Flag flag, bool set) {
    Address destination = fetch16();
    if (get_flag(flag) == set) {
        JP(destination);
        timer.tick();
        return true;
    }

    return false;
}


/**
 * Relative Jump -> PC += (signed) imm value
 * - - - -
*/
void CPU::JR() {
    int16_t imm = (int16_t)(int8_t)(fetch());
    reg_PC = (Word)((int16_t)(reg_PC) + imm);
    timer.tick();
}


/**
 * Conditional Relative Jump - check that flag is either set or unset, depending on set param
 *     if yes, then JR(), otherwise increment PC by 1 and return
 * - - - -
 * 
 * @param flag the flag to check the value of in Register F
 * @param set the boolean value to check the flag against
 * @return true if a jump occured, false otherwise
*/
bool CPU::JR_IF(const Flag flag, bool set) {
    if (get_flag(flag) == set) {
        JR();
        return true;
    }
    // Account for the byte that would have been (signed) imm8
    reg_PC += 1;
    timer.tick();
    return false;
}


/**
 * Push PC onto stack and set PC = imm16
 * - - - -
*/
void CPU::CALL() {  
    Address destination = fetch16();
    PUSH_PC();
    reg_PC = destination;
    timer.tick();
}


/**
 * Conditional Call - check that flag is either set or unset, depending on set param
 *     if yes, then CALL(), otherwise increment PC by 2 and return
 * - - - -
 * 
 * @param flag the flag to check the value of in Register F
 * @param set the boolean value to check the flag against
 * @return true if a call occured, false otherwise
*/
bool CPU::CALL_IF(const Flag flag, bool set) {
    if (get_flag(flag) == set) {
        CALL();
        return true;
    }

    // Account for the two bytes that would have been imm16
    reg_PC += 2;
    timer.tick();
    timer.tick();
    return false;
}


/**
 * Reset, pushing PC onto the stack and jumping to 0x0000 + offset
 * 
 * @param offset the 8-bit representation of the address to jump PC to
*/
void CPU::RST(const Byte offset) {
    PUSH_PC();
    reg_PC = 0x0000 + offset;
    timer.tick();
}


/**
 * Return (pop PC from the stack)
 * - - - -
*/
void CPU::RET() {
    Byte low = memory_bus.read(reg_SP);
    reg_SP++;
    timer.tick();

    reg_PC = ((memory_bus.read(reg_SP) << 8) | low);
    reg_SP++;
    timer.tick();

    timer.tick();
}

/**
 * Conditional Return - check that flag is either set or unset, depending on set param
 *     if yes, then RET(), otherwise return
 * - - - -
 * 
 * @param flag the flag to check the value of in Register F
 * @param set the boolean value to check the flag against
 * @return true if a return occured, false otherwise
*/
bool CPU::RET_IF(const Flag flag, bool set) {
    timer.tick();  
    if (get_flag(flag) == set) {
        RET();
        return true;
    }

    return false;
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
 * 8-bit load
 * dest = value
 * - - - -
 * 
 * @param dest the destination register or value in memory to be changed
 * @param value the value to load into the destination
*/
void CPU::LD(Byte& dest, const Byte value) {
    dest = value;
}


/**
 * 8-bit load
 * dest = memory[address]
 * - - - -
 * 
 * @param dest the destination register or value in memory to be changed
 * @param address the address of the value to load into the destination
*/
void CPU::LD(Byte& dest, const Address address) {
    dest = memory_bus.read(address);
    timer.tick();
}


/**
 * 8-bit load
 * memory[address] = value
 * - - - -
 * 
 * @param address the address of the destination in memory
 * @param value the value to load into the destination
*/
void CPU::LD(const Address address, Byte value) {
    memory_bus.write(address, value);
    timer.tick();
}


/**
 * 8-bit load high
 * - - - -
 * 
 * @param value the value to load into the destination
 * @param into_A true if loading memory value into A, false if loading A into memory
*/
void CPU::LDH(const Byte value, bool into_A) {
    Address address = IO_START + value;
    if (into_A) {
        // reg_A = memory[$FF00 + n]  
        LD(reg_A, address);
    } else {
        // memory[$FF00 + n] = reg_A
        LD(address, reg_A);
    }
}


/***
 * 16-bit load
 * dest = value
 * - - - -
 * 
 * @param dest the 16-bit register to update
 * @param value the value to load into the register
*/
void CPU::LD(Word& dest, const Word value) {
    dest = value;
}


/***
 * 16-bit load
 * [high | low] = [value 15:8 | value 7:0]
 * - - - -
 * 
 * @param pair the pair to be updated
 * @param value the value to load into the pair
*/
void CPU::LD(Pair& pair, const Word value) {
    set_pair(pair, value);
}


/***
 * 16-bit load (write)
 * memory[address] = SP[7:0], memory[address+1] = SP[15:8]
 * - - - -
 * 
 * @param address the address in memory to hold least significant byte of SP
*/
void CPU::write_SP(const Address address) {
    memory_bus.write(address, (reg_SP & 0xFF));        // low byte
    timer.tick();
    memory_bus.write((address + 1), (reg_SP >> 8));    // high byte
    timer.tick();
}


/***
 * Push pair onto the stack, little-endian
 * - - - -
 * 
 * @param pair the pair to be pushed to the stack
*/
void CPU::PUSH(const Pair& pair) {
    reg_SP--;
    memory_bus.write(reg_SP, *(pair.reg_high));
    timer.tick();
    
    reg_SP--;
    memory_bus.write(reg_SP, *(pair.reg_low));
    timer.tick();

    timer.tick();
}


/***
 * Push PC onto the stack, little-endian
 * - - - -
*/
void CPU::PUSH_PC() {
    reg_SP--;
    memory_bus.write(reg_SP, (reg_PC >> 8));   // high byte
    timer.tick();

    reg_SP--;
    memory_bus.write(reg_SP, (reg_PC & 0xFF));   // low byte
    timer.tick();
}


/***
 * Pop pair from the stack, little-endian
 * - - - -
 * 
 * @param pair the pair to be popped from the stack
*/
void CPU::POP(Pair& pair) {
    *(pair.reg_low) = memory_bus.read(reg_SP);
    reg_SP++;
    timer.tick();

    *(pair.reg_high) = memory_bus.read(reg_SP);
    reg_SP++;
    timer.tick();
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
    uint16_t res = reg_A + value;

    if (carry) {
        // update carry to the value stored in reg_F
        carry = get_flag(FLAG_CARRY) ? 1 : 0;
        res += carry;
    }

    // check for carries from bit [3] to bit [4]
    bool half_carry = ((reg_A & 0xF) + (value & 0xF) + carry) > 0xF;

    // update reg_A to bits [7:0] of res
    reg_A = res & 0xFF;

    update_flag(FLAG_ZERO, reg_A == 0); // true if bits [7:0] of res are 0 (i.e. including wrappings)
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
    uint16_t res = reg_A - value;

    if (carry) {
        // update carry to the value stored in reg_F
        carry = get_flag(FLAG_CARRY) ? 1 : 0;
        res -= carry;
    }

    bool half_carry = ((value & 0xF) + carry) > (reg_A & 0xF);   // check for borrows from bit [4]
    // update carry (again) to whether a borrow occured from bit [8]
    carry = ((value + carry) > reg_A);

    reg_A = res & 0xFF;

    update_flag(FLAG_ZERO, reg_A == 0);
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
    reg_A &= value;

    update_flag(FLAG_ZERO, reg_A == 0);
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
    reg_A ^= value;

    update_flag(FLAG_ZERO, reg_A == 0);
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
    reg_A |= value;

    update_flag(FLAG_ZERO, reg_A == 0);
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
    uint16_t temp = reg_A - value;

    update_flag(FLAG_ZERO, (temp & 0xFF) == 0);
    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, (value & 0xF) > (reg_A & 0xF));
    update_flag(FLAG_CARRY, value > reg_A);
}


/**
 * 8-bit increment
 * Z 0 H -
 * 
 * @param reg the register value to be incremented
*/
void CPU::INC(Byte &reg) {
    bool half_carry = (reg & 0xF) == 0xF;
    reg += 1;

    update_flag(FLAG_ZERO, reg == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);
}


/**
 * Increment value stored in memory[HL]
 * Z 0 H -
 *
*/
void CPU::INC_HL() {
    // Get memory[HL]
    Byte value = memory_bus.read(get_pair(HL));
    timer.tick();

    // Increment memory[HL] and set flags
    INC(value);

    // Update memory[HL] to incremented value
    memory_bus.write(get_pair(HL), value);
    timer.tick();
}


/**
 * 8-bit decrement
 * Z 1 H -
 * 
 * @param reg the register value to be decremented
*/
void CPU::DEC(Byte &reg) {
    bool half_carry = (reg & 0xF) == 0;
    reg -= 1;

    update_flag(FLAG_ZERO, reg == 0);
    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, half_carry);
}


/***
 * Decrement value stored in memory[HL]
 * Z 1 H -
*/
void CPU::DEC_HL() {
    // Get memory[HL]
    Byte value = memory_bus.read(get_pair(HL));
    timer.tick();

    // Decrement memory[HL] and set flags
    DEC(value);

    // Update memory[HL] to decremented value
    memory_bus.write(get_pair(HL), value);
    timer.tick();
}


/**
 * 16-bit addition to HL pair
 * - 0 H C
 * 
 * @param value the word to add to HL
*/
void CPU::ADD_HL(const Word value) {
    Word old_val = get_pair(HL);
    uint32_t res = old_val + value;

    bool carry = (res >> 16) != 0;
    bool half_carry = (((old_val & 0xFFF) + (value & 0xFFF)) > 0xFFF);

    
    set_pair(HL, (res & 0xFFFF));
    timer.tick();

    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);
    update_flag(FLAG_CARRY, carry);
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


/***
 * 16-bit increment
 * - - - -
 * 
 * @param reg the 16-bit register to be incremented
*/
void CPU::INC(Word& reg) {
    reg++;
    timer.tick();
}


/**
 * 16-bit increment
 * - - - -
 * 
 * @param pair the register pair to be incremented
*/
void CPU::INC(Pair& pair) {
    (*(pair.reg_low))++;
    if (*(pair.reg_low) == 0x00) {
        (*(pair.reg_high))++;
    }
}


/***
 * 16-bit decrement
 * - - - -
 * 
 * @param reg the 16-bit register to be decremented
*/
void CPU::DEC(Word& reg) {
    reg--;
    timer.tick();
}


/***
 * 16-bit decrement
 * - - - -
 * 
 * @param pair the register pair to be decremented
*/
void CPU::DEC(Pair& pair) {
    (*(pair.reg_low))--;
    if (*(pair.reg_low) == 0xFF) {
        (*(pair.reg_high))--;
    }
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
    bool bit_7 = ((reg_A & 0x80) >> 7);
    reg_A = ((reg_A & 0x7F) << 1);
    if (circular) {
        // RLCA
        reg_A |= bit_7;
    } else {
        // RLA
        reg_A |= get_flag(FLAG_CARRY);
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
    bool bit_0 = reg_A & 0b1;
    reg_A = (reg_A >> 1);
    if (circular) {
        // RRCA
        reg_A = (reg_A & ~(1 << 7) | (bit_0 << 7));
    } else {
        // RRA
        reg_A = (reg_A & ~(1 << 7) | (get_flag(FLAG_CARRY) << 7));
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