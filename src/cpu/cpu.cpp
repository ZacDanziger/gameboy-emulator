#include "cpu.h"

CPU::CPU(const std::vector<uint8_t>& rom_data) : _memory(rom_data), {0} {} 

int CPU::step() {
    handle_interrupts();    // not created yet

    uint8_t opcode = fetch();
    return decode_execute(opcode);
}

// May need wrapping add
uint8_t CPU::fetch() {
    return _memory.read(reg_PC++);
}

int CPU::decode_execute(uint8_t opcode) {
    switch (opcode) {
    }

    // LD 8-bit
    if (opcode >= 0x40 && opcode < 0x80) {
        int source = opcode & 0b111;
        int destination = (opcode >> 3) & 0b111;

        return ld_8(destination, source);
    }

    switch (opcode >> 4) {
    case 0x8:   // ADD and ADC
        int reg = opcode & 0b111;
        bool carry = ((opcode >> 3) & 1);

        return add_8(reg, carry);
    case 0x9:   // SUB and SBC
        int reg = opcode & 0b111;
        bool carry = ((opcode >> 3) & 1);

        return sub_8(reg, carry);
    case 0xA:   // AND and XOR
        int reg = opcode & 0b111;

        if ((opcode >> 3) & 1) {
            return xor_8(reg);
        } else {
            return and_8(reg);
        }
    case 0xB:   // OR and CP
        int reg = opcode & 0b111;

        if ((opcode >> 3) & 1) {
            return or_8(reg);
        } else {
            return cp_8(reg);
        }
    }
}

// needs testing
void CPU::update_flag(uint8_t flag, bool new_val) {
    if (new_val) {
        .reg_F |= flag;
    } else {
        reg_F &= ~(flag);
    }
}

// Get the value in memory stored in [HL]
uint8_t CPU::get_hl() {
    int address =  (reg_H << 8) | reg_L;
    return _memory.read(address);
}

// Set the value in memory stored in [HL] to the value stored in register reg
void CPU::set_hl(int reg) {
    uint16_t address = (reg_H << 8) | reg_L;
    // uint8_t data = reg(reg);
    _memory.write(address, data);
}

int CPU::ld_8(int dest, int src) {
    if (dest == 6) {
        set_hl(src);
        return 8;
    }

    if (src == 6) {
        reg(dest) = get_hl();
        return 8;
    }
    reg(dest) = reg(src);
    return 4;
}

int CPU::add_8(uint8_t value, bool carry) {
    .reg_A += value;
    if (carry) {
        .reg_A += (.reg_F & FLAG_CARRY) ? 1 : 0;
    }

    update_flag(FLAG_ZERO, reg_A == 0);
    update_flag(FLAG_SUB, false);
    update_flag(FLAG_HALF_CARRY, (((reg_A & 0xF) + (value & 0xF) & 0x10) == 0x10));
    update_flag(FLAG_CARRY, reg_A == 0x00);

    return hl ? 8 : 4;
}

int CPU::sub_8(int src, bool carry) {
    int val = 0;
    bool hl = false;

    if (src == 6) {
        val = get_hl();
        hl = true;
    } else {
        val = .reg(src);
    }

    reg_A -= val;
    if (carry) {
        reg_A -= get_flag(CARRY);
    }

    update_flag(ZERO, reg_A == 0);
    update_flag(SUB, true);
    update_flag(HALF_CARRY, (((reg_A & 0xF) - (val & 0xF) & 0x10) == 0x10));
    update_flag(CARRY, (reg_A >> 7) & 1);    // check if MSB is 1, indicating a negative result

    return hl ? 8 : 4;
}

int CPU::and_8(int src) {
    int val = 0;
    bool hl = false;

    if (src == 6) {
        val = get_hl();
        hl = true;
    } else {
        val = reg(src);
    }

    reg_A = .reg_A & val;

    update_flag(ZERO, reg_A == 0);
    update_flag(SUB, false);
    update_flag(HALF_CARRY, true);
    update_flag(CARRY, false);

    return hl ? 8 : 4;
}

int CPU::xor_8(int src) {
    int val = 0;
    bool hl = false;

    if (src == 6) {
        val = get_hl();
        hl = true;
    } else {
        val = .reg(src);
    }

    .reg_A = .reg_A ^ val;

    update_flag(ZERO, .reg_A == 0);
    update_flag(SUB, false);
    update_flag(HALF_CARRY, false);
    update_flag(CARRY, false);

    return hl ? 8 : 4;
}

int CPU::or_8(int src) {
    int val = 0;
    bool hl = false;

    if (src == 6) {
        val = get_hl();
        hl = true;
    } else {
        val = .reg(src);
    }

    .reg_A = .reg_A | val;

    update_flag(ZERO, .reg_A == 0);
    update_flag(SUB, false);
    update_flag(HALF_CARRY, false);
    update_flag(CARRY, false);

    return hl ? 8 : 4;
}

// Compare - A - r8, but just set flags and throw away result
int CPU::cp_8(int src) {
    int val = 0;
    bool hl = false;

    if (src == 6) {
        val = get_hl();
        hl = true;
    } else {
        val = .reg(src);
    }

    int res = .reg_A - val;

    update_flag(ZERO, res == 0);
    update_flag(SUB, true);
    update_flag(HALF_CARRY, (((res & 0xF) - (val & 0xF) & 0x10) == 0x10));
    update_flag(CARRY, (res >> 7) & 1);    // check if MSB is 1, indicating a negative result

    return hl ? 8 : 4;
}