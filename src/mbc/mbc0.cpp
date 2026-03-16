#include "mbc0.h"

Byte MBC0::read(const Address address) const {
    if (address < VRAM_START) {
        return rom[address];
    } 
    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        return eram[address - ERAM_START];
    }

    throw std::runtime_error("MBC read called on wrong address");
}

void MBC0::write(const Address address, const Byte data) {
    // no writing to ROM on an MBC0
    if (address < VRAM_START) {
        return;
    } 
    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        eram[address - ERAM_START] = data;
        return;
    }

    throw std::runtime_error("MBC write called on wrong address");
}