#include "interconnect.h"

Interconnect::Interconnect(const std::vector<uint8_t>& rom_data) 
    : _rom(rom_data), _vram(VRAM_SIZE), _wram(WRAM_SIZE), _hram(HRAM_SIZE) {}

uint8_t Interconnect::read(uint16_t address) {
    return resolve_device(address).read(address);
}

void Interconnect::write(uint16_t address, uint8_t data) {
    resolve_device(address).write(address, data);
}

Memory& Interconnect::resolve_device(uint16_t address) {
    if (address <= ROM_END) {
        return _rom;
    }

    if (address <= VRAM_END) {
        return _vram;
    }

    if (address <= ERAM_END) {
        return _rom;       // need to understand external RAM
    }

    if (address <= WRAM_END) {
        return _wram;
    }

    if (address <= ECHO_END) {
        throw std::runtime_error("Cannot use Echo RAM");
    }

    if (address <= OAM_END) {
        return _vram;
    }

    if (address < IO_START) {
        throw std::runtime_error("Cannot use this section of memory");
    }

    if (address >= HRAM_START && address <= HRAM_END) {
        return _hram;
    }

    throw std::runtime_error("Address not valid");
}