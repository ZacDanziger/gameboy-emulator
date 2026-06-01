#include "mbc0.h"

/**
 * Read from ROM or ERAM
 * 
 * @param address the address to read from, in range [0x0000, 0x7FFF] U [0xA000, 0xBFFF]
 * @returns the data stored at that address, or an error if out of bounds
 */
Byte MBC0::read(const Address address) const {
    if (address < VRAM_START) {
        return rom[address];
    } 
    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        return eram[address - ERAM_START];
    }

    throw std::runtime_error("MBC read called on wrong address");
}

/**
 * Write to ERAM
 * NOTE: writes to ROM will do nothing
 * 
 * @param address the address to write to
 * @param data the data to write to the address
 */
void MBC0::write(const Address address, const Byte data) {
    // no writing to ROM on an MBC0
    if (address < VRAM_START) {
        return;
    } 
    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        if (eram.size() == 0) {
            return;
        }
        eram[address - ERAM_START] = data;
        return;
    }

    throw std::runtime_error("MBC write called on wrong address");
}