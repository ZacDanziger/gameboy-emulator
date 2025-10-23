#include "cpu.h"

CPU::CPU(const std::vector<uint8_t>& rom_data) : _memory(rom_data), _registers{0} {} 

// May need wrapping add
uint8_t CPU::fetch() {
    return _memory.read(_registers.pc++);
}

size_t CPU::step() {
    handle_interrupts();    // not created yet
    uint8_t opcode = fetch();
    
    if (opcode == 0x76) {
        halt();
    }

    if (opcode >= 0x40 && opcode < 0x80) {
        int source = opcode & 0b111;
        int destination = (opcode >> 3) & 0b111;

        return LD_8(destination, source);
    }
}

uint8_t CPU::get_hl() {
    int address =  (_registers.h << 8) | _registers.l;
    return _memory.read(address);
}

void CPU::set_hl(int reg) {
    uint16_t address = (_registers.h << 8) | _registers.l;
    uint8_t data = *(_registers.reg_array[reg]);
    _memory.write(address, data);
}

size_t CPU::LD_8(int& dest, int src) {
    if (dest == 6) {
        set_hl(src);
        return 8;
    }

    if (src == 6) {
        *(_registers.reg_array[dest]) = get_hl();
    }
    *(_registers.reg_array[dest]) = *(_registers.reg_array[src]);
    return 4;
}