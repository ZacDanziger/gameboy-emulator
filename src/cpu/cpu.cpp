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
    reg_SP(0),
    reg_PC(0) {} 

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
 * Get the value in memory stored in [HL]
 * 
 * @return the value in memory[HL]
*/
Byte CPU::get_hl() {
    Address address =  (reg_H << 8) | reg_L;
    return _memory.read(address);
}

/**
 * Update the value in memory stored in [HL]
 * 
 * @param value the new value to be stored in memory[HL]
 */
void CPU::set_hl(const Byte value) {
    Address address = (reg_H << 8) | reg_L;
    _memory.write(address, value);
}

/**
 * 8-bit load [LD] 
 * register[dest] = value
 * Z N H C
 * - - - -
 * 
 * @param reg the address of the destination register
 * @param value the value to load into the destination register
*/
void CPU::ld_8(Byte& reg, Byte value) {
    reg = value;
}

/**
 * 8-bit load [LD]
 * register[dest] = memory[address]
 * Z N H C
 * - - - -
 * 
 * @param reg the address of the destination register
 * @param address the address of the value to load into the destination register
*/
void CPU::ld_8(Byte& reg, Address address) {
    reg = _memory.read(address);
}

/**
 * 8-bit addition [ADD] [ADC]
 * *** NEED TO ADD ADD HL and ADC HL ***
 * --- NOT TESTED ---
 * Z N H C
 * Z 0 H C
 * 
 * @param value the byte to be added to the accumulator (reg A) (usually a register, though can be imm value)
 * @param carry whether or not to add the carry flag to result, true if ADC, false if ADD
*/
void CPU::add_8(const Byte value, bool carry) {
    // do math in 16-bit to check for carrys on bit [8]
    uint16_t res = reg_A + value;

    if (carry) {
        // update carry to the value stored in reg_F
        carry = ((reg_F & FLAG_CARRY) == FLAG_CARRY) ? 1 : 0;
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
 * 8-bit subtraction [SUB] [SBC]
 * *** NEED TO ADD SUB HL and SBC HL ***
 * --- NOT TESTED ---
 * Z N H C
 * Z 1 H C
 * 
 * @param value the byte to be subtracted from the accumulator (reg A) (usually a register, though can be imm value)
 * @param carry whether or not to subtract the carry flag from result, true if SBC, false if SUB
*/
void CPU::sub_8(const Byte value, bool carry) {
    // do math in 16-bit to check for carrys on bit [8]
    uint16_t res = reg_A - value;

    if (carry) {
        // update carry to the value stored in reg_F
        carry = ((reg_F & FLAG_CARRY) == FLAG_CARRY) ? 1 : 0;
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
 * 8-bit bitwise and [AND]
 * --- NOT TESTED ---
 * Z N H C
 * Z 0 1 0
 * 
 * @param value the byte to be and'd with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::and_8(const Byte value) {
    // line may cause issues:
    reg_A &= value;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, true);
    update_flag(FLAG_CARRY, false);  
}

/**
 * 8-bit bitwise xor [XOR]
 * --- NOT TESTED ---
 * Z N H C
 * Z 0 0 0
 * 
 * @param value the byte to be xor'd with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::xor_8(const Byte value) {
    // line may cause issues:
    reg_A ^= value;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, false);
}

/**
 * 8-bit bitwise or [OR]
 * --- NOT TESTED ---
 * Z N H C
 * Z 0 0 0
 * 
 * @param value the byte to be or'd with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::or_8(const Byte value) {
    // line may cause issues:
    reg_A |= value;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, false);
}

/**
 * 8-bit compare [CP]
 * --- NOT TESTED ---
 * Z N H C
 * 1 1 0 0
 * 
 * @param value the byte to be or'd with the accumulator (reg A) (usually a register, though can be imm value)
*/
void CPU::cp_8(const Byte value) {
    // do math in 16-bit to check for carrys on bit [8]
    uint16_t temp = reg_A - value;

    update_flag(FLAG_ZERO, (temp & 0xFF) == 0);
    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, (value & 0xF) > (reg_A & 0xF));
    update_flag(FLAG_CARRY, value > reg_A);
}

/**
 * 8-bit increment [INC]
 * --- NOT TESTED ---
 * Z N H C
 * Z 0 H -
 * 
 * @param reg_value the register value to be incremented
*/
void CPU::inc_8(Byte &reg_value) {
    bool half_carry = (reg_value & 0xF) == 0xF;
    reg_value += 1;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);
}

/**
 * 8-bit decrement [DEC]
 * --- NOT TESTED ---
 * Z N H C
 * Z 1 H -
 * 
 * @param reg_value the register value to be decremented
*/
void CPU::dec_8(Byte &reg_value) {
    bool half_carry = (reg_value & 0xF) == 0;
    reg_value -= 1;

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, half_carry);
}

/**
 * Complement Accumulator (register A) [CPL]
 * --- NOT TESTED ---
 * Z N H C
 * - 1 1 -
*/
void CPU::cpl() {
    reg_A = ~reg_A;

    update_flag(FLAG_SUB, true);
    update_flag(FLAG_HALF_CARRY, true);
}

/**
 * Complement Carry Flag [CCF]
 * --- NOT TESTED ---
 * Z N H C
 * - 0 0 C
*/
void CPU::ccf() {
    bool old_val = ((reg_F & FLAG_CARRY) == FLAG_CARRY);

    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, false);
    update_flag(FLAG_CARRY, (1 - old_val));
}