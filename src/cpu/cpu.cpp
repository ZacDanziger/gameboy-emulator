#include "cpu.h"

/**
 * Z - Zero Flag
 * N - Subtraction Flag
 * H - Half-Carry Flag
 * C - Carry Flag
 * 
 * 0 - unset
 * 1 - set
 * - - no change
*/

CPU::CPU() : 
    _memory(), 
    reg_A(0),
    reg_F(0),
    reg_B(0),
    reg_C(0),
    reg_D(0),
    reg_E(0),
    reg_H(0),
    reg_L(0),
    AF{&reg_A, &reg_F},
    BC{&reg_B, &reg_C},
    DE{&reg_D, &reg_E},
    HL{&reg_H, &reg_L},
    reg_SP(0),
    reg_PC(0) 
{} 


/**
 * Perform the fetch, decode, execute loop
 * --- NOT TESTED ---
 * 
 * @return the number of T-states taken in the loop (always some multiple of 4)
*/
int CPU::step() {
    // handle_interrupts();    // not created yet

    Byte opcode = fetch();
    return decode_execute(opcode);
}


/**
 * Fetch the next instruction and increment PC
 * 
 * @return the value in memory[PC]
*/
Byte CPU::fetch() {
    return _memory.read(reg_PC++);
}


/**
 * Fetch the next 16 bits, in little endian format
 * 
 * @return [low | high]
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
bool CPU::get_flag(const uint8_t flag) const {
    return ((reg_F & flag) == flag);
}


/**
 * update flag to new_val
 * 
 * @param flag the flag to be updated
 * @param new_val the new value of the flag
 */ 
void CPU::update_flag(const uint8_t flag, bool new_val) {
    if (new_val) {
        reg_F |= flag;
    } else {
        reg_F &= ~(flag);
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
 * --- NOT TESTED ---
 * 
 * @return memory[HL]
*/
Byte CPU::read_hl() const {
    return _memory.read(get_pair(HL));
}


/**
 * Complement Accumulator (register A)
 * --- NOT TESTED ---
 * - 1 1 -
*/
void CPU::CPL() {
    reg_A = ~reg_A;

    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, true);
}


/**
 * Complement Carry Flag
 * --- NOT TESTED ---
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
 * --- NOT TESTED ---
 * - 0 0 1
*/
void CPU::SCF() {
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, true);
}


/**
 * 8-bit load
 * dest = value
 * - - - -
 * 
 * @param dest the address of the destination register or location in memory
 * @param value the value to load into the destination register
*/
void CPU::LD(Byte& dest, const Byte value) {
    dest = value;
}


/**
 * 8-bit load
 * dest = memory[address]
 * - - - -
 * 
 * @param dest the address of the destination register or location in memory
 * @param address the address of the value to load into the destination register
*/
void CPU::LD(Byte& dest, const Address address) {
    dest = _memory.read(address);
}


/**
 * 8-bit load
 * memory[address] = value
 * - - - -
 * 
 * @param address the address of the destination in memory
 * @param value the value to load into the destination
*/
void CPU::LD(const Address address, const Byte value) {
    _memory.write(address, value);
}


/**
 * 8-bit load high
 * - - - -
 * 
 * @param value the value to load into the destination
 * @param into_A true if loading memory value into A, false if loading A into memory
*/
void CPU::LDH(const Byte value, bool into_A) {
    if (into_A) {
        // memory[$FF00 + n] = reg_A
        reg_A = _memory.read(IO_START + value);
    } else {
        // reg_A = memory[$FF00 + n]
        _memory.write(IO_START + value, reg_A);
    }
}

/***
 * 16-bit load
 * dest = value
 * - - - -
 * 
 * @param dest the address of the 16-bit register to update
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
    _memory.write(address, (reg_SP & 0xFF));        // low byte
    _memory.write((address + 1), (reg_SP >> 8));    // high byte
}


/**
 * 8-bit addition with and without carry
 * --- NOT TESTED ---
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
 * --- NOT TESTED ---
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
 * --- NOT TESTED ---
 * Z 0 1 0
 * 
 * @param value the byte to be and'd with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::AND(const Byte value) {
    // line may cause issues:
    reg_A &= value;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, true);
    update_flag(FLAG_CARRY, false);  
}


/**
 * 8-bit bitwise xor
 * --- NOT TESTED ---
 * Z 0 0 0
 * 
 * @param value the byte to be xor'd with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::XOR(const Byte value) {
    // line may cause issues:
    reg_A ^= value;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, false);
}


/**
 * 8-bit bitwise or
 * --- NOT TESTED ---
 * Z 0 0 0
 * 
 * @param value the byte to be or'd with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::OR(const Byte value) {
    // line may cause issues:
    reg_A |= value;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, false);
}


/**
 * 8-bit compare
 * --- NOT TESTED ---
 * Z 1 H C
 * 
 * @param value the byte to be or'd with the accumulator (reg A) (usually a register, though can be imm value)
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
 * --- NOT TESTED ---
 * Z 0 H -
 * 
 * @param reg the register value to be incremented
*/
void CPU::INC(Byte &reg) {
    bool half_carry = (reg & 0xF) == 0xF;
    reg += 1;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);
}
/**
 * Increment value stored in memory[HL]
 * --- NOT TESTED ---
 * Z 0 H -
 *
*/
void CPU::INC_HL() {
    // Get memory[HL]
    Byte value = _memory.read(get_pair(HL));

    // Increment memory[HL] and set flags
    INC(value);

    // Update memory[HL] to incremented value
    _memory.write(get_pair(HL), value);
}

/**
 * 8-bit decrement
 * --- NOT TESTED ---
 * Z 1 H -
 * 
 * @param reg the register value to be decremented
*/
void CPU::DEC(Byte &reg) {
    bool half_carry = (reg & 0xF) == 0;
    reg -= 1;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, half_carry);
}

/***
 * Decrement value stored in memory[HL]
 * --- NOT TESTED ---
 * Z 1 H -
*/
void CPU::DEC_HL() {
    // Get memory[HL]
    Byte value = _memory.read(get_pair(HL));

    // Decrement memory[HL] and set flags
    DEC(value);

    // Update memory[HL] to decremented value
    _memory.write(get_pair(HL), value);
}

/**
 * 16-bit addition
 * --- NOT TESTED ---
 * - 0 H C
 * 
 * @param value the word to add to HL
*/
void CPU::ADD(const Word value) {
    Word old_val = get_pair(HL);
    uint32_t res = old_val + value;

    bool carry = (res >> 16) != 0;
    bool half_carry = (((old_val & 0xFFF) + (value & 0xFFF)) > 0xFFF);

    set_pair(HL, (res & 0xFFFF));

    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);
    update_flag(FLAG_CARRY, carry);
}

/**
 * 16-bit addition
 * Adds signed 8-bit imm value to SP
 * --- NOT TESTED ---
 * 0 0 H C
 * 
 * @return SP + e8
*/
Word CPU::ADD() {
    // Get signed one byte immediate value
    int8_t imm = static_cast<int8_t>(fetch());
    int32_t temp = static_cast<int32_t>(reg_SP) + imm;

    bool carry = (temp >> 16) & 0b1;
    bool half_carry = (((imm & 0xF) + (reg_SP & 0xF)) > 0xF);

    Word res = static_cast<Word>(temp & 0xFF);

    update_flag(FLAG_ZERO, false);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);
    update_flag(FLAG_CARRY, carry);

    return res;
}
/***
 * 16-bit increment
 * --- NOT TESTED ---
 * - - - -
 * 
 * @param reg the 16-bit register to be incremented
*/
void CPU::INC(Word& reg) {
    reg++;
}

/**
 * 16-bit increment
 * --- NOT TESTED ---
 * - - - -
 * 
 * @param pair the register pair to be incremented
*/
void CPU::INC(Pair& pair) {
    (*(pair.reg_low))++;
    if (*(pair.reg_low) == 0) {
        (*(pair.reg_high))++;
    }
}

/***
 * 16-bit decrement
 * --- NOT TESTED ---
 * - - - -
 * 
 * @param reg the 16-bit register to be decremented
*/
void CPU::DEC(Word& reg) {
    reg--;
}

/***
 * 16-bit decrement
 * --- NOT TESTED ---
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
 * (Why does this one have to be different?)
 * --- NOT TESTED ---
 * 0 0 0 C 
 * 
 * @param circular true if opcode is RLCA, false if RLA
*/
void CPU::RLA(bool circular) {
    bool bit_7 = (reg_A >> 7);
    reg_A = (reg_A << 1);
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
 * --- NOT TESTED ---
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
 * Rotate Right, Register A
 * (Both circular and non circular)
 * (Why does this one have to be different?)
 * --- NOT TESTED ---
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
 * --- NOT TESTED ---
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