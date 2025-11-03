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
 * 
 * @return the number of T-states taken in the loop (always some multiple of 4)
*/
int CPU::step() {
    handle_interrupts();    // not created yet

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
 * read the value of the flag stored in register F
 * 
 * @param flag the flag to be read
*/
bool CPU::get_flag(const uint8_t flag) const {
    return ((reg_F & flag) == flag);
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

Byte CPU::read_hl() const {
    return _memory.read(get_pair(HL));
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
 * @param reg_value the register value to be incremented
*/
void CPU::INC(Byte &reg_value) {
    bool half_carry = (reg_value & 0xF) == 0xF;
    reg_value += 1;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);
}


/**
 * 8-bit decrement
 * --- NOT TESTED ---
 * Z 1 H -
 * 
 * @param reg_value the register value to be decremented
*/
void CPU::DEC(Byte &reg_value) {
    bool half_carry = (reg_value & 0xF) == 0;
    reg_value -= 1;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, half_carry);
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
 * Rotate Left, Register A
 * (Both circular and non circular)
 * (Why does this one have to be different?)
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