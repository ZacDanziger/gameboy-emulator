#include "cpu.h"

CPU::CPU(const std::vector<uint8_t>& rom_data) : _memory(rom_data), _registers{0} {} 

uint8_t CPU::next_byte() {
    uint8_t byte = _memory.read(_registers.pc);
    _registers.pc++;

    return byte;
}

size_t CPU::step() {
    handle_interrupts();
    uint8_t opcode = next_byte();

    switch(opcode) {

    }
}