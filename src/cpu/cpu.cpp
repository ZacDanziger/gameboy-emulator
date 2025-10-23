#include "cpu.h"

CPU::CPU(const std::vector<uint8_t>& rom_data) : _memory(rom_data), _registers{0} {} 

// May need wrapping add
uint8_t CPU::fetch() {
    return _memory.read(_registers.pc++);
}

int CPU::step() {
    handle_interrupts();    // not created yet
    uint8_t opcode = fetch();
    
    if (opcode == 0x76) {
        halt();
    }

    // LD 8-bit
    if (opcode >= 0x40 && opcode < 0x80) {
        int source = opcode & 0b111;
        int destination = (opcode >> 3) & 0b111;

        return ld_8(destination, source);
    }

    // ADD and ADC 8-bit
    if ((opcode >> 4) == 0x8) {
        int reg = opcode & 0b111;

        // ADC
        if ((opcode >> 3) & 1) {
            return adc_8(reg);
        } else {
            return add_8(reg);
        }

    }

    // SUB and SBC 8-bit
    if ((opcode >> 4) == 0x9) {
        int reg = opcode & 0b111;

        if ((opcode >> 3) & 1) {
            return sbc_8(reg);
        }

        return sub_8(reg);
    }
}

uint8_t CPU::get_hl() {
    int address =  (_registers.h << 8) | _registers.l;
    return _memory.read(address);
}

void CPU::set_hl(int reg) {
    uint16_t address = (_registers.h << 8) | _registers.l;
    uint8_t data = _registers.reg(reg);
    _memory.write(address, data);
}

int CPU::ld_8(int dest, int src) {
    if (dest == 6) {
        set_hl(src);
        return 8;
    }

    if (src == 6) {
        _registers.reg(dest) = get_hl();
        return 8;
    }
    _registers.reg(dest) = _registers.reg(src);
    return 4;
}