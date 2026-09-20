#include "cpu.h"


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
 * 16-bit addition to HL pair
 * - 0 H C
 * 
 * @param value the word to add to HL
*/
void CPU::ADD_HL(const Word value) {
    uint32_t res = registers.HL.word + value;

    bool carry = (res >> 16) != 0;
    bool half_carry = (((registers.HL.word & 0xFFF) + (value & 0xFFF)) > 0xFFF);

    
    registers.HL.word = (res & 0xFFFF);

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
    int16_t imm = (int16_t)(int8_t)(scratch_register.low);
    Word res = (Word)((int16_t)(registers.SP.word) + imm);

    bool carry = (res & 0xFF) < (registers.SP.word & 0xFF);
    bool half_carry = (res & 0xF) < (registers.SP.word & 0xF);

    update_flag(FLAG_ZERO, false);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, half_carry);
    update_flag(FLAG_CARRY, carry);

    return res;
}



/**
 * Rotate Left
 * (Both circular and non circular)
 * NOTE: RLCA and RLA calls will always reset Zero flag
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
 * Rotate Right
 * (Both circular and non circular)
 * NOTE: RRCA and RRA calls will always reset Zero flag
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