#ifndef MEMORY_H
#define MEMORY_H

#include <array>
#include <stdexcept>
#include <algorithm>
#include "memory_map.h"
#include "../utils/utils.h"

/**
 * Represents all memory in the gameboy
 * Byte-addressable 16-bit address size
 *      currently handles ROM, VRAM, WRAM, ECHO RAM, I/O REGISTERS, HRAM, and IE REGISTER
 *      currently does not handle ERAM, and OAM
 * TODO: implement VRAM lockout
*/
class Memory {
    public:
        Memory();

        Byte read(const Address address) const;
        void write(const Address address, const Byte data);
        void load(const Address address, const std::string& filename);
        void load_rom(const std::string& filename);

    private:
        std::array<Byte, ROM_BANK_SIZE> rom_bank_00;       // 0x0000 - 0x3FFF
        std::array<Byte, ROM_BANK_SIZE> rom_bank_nn;       // 0x4000 - 0x7FFF
        std::array<Byte, VRAM_SIZE> vram;                  // 0x8000 - 0x9FFF
        std::array<Byte, WRAM_BANK_SIZE> wram_bank_00;     // 0xC000 - 0xCFFF
        std::array<Byte, WRAM_BANK_SIZE> wram_bank_nn;     // 0xD000 - 0xDFFF
        std::array<Byte, OAM_SIZE> oam;                    // 0xFE00 - 0xFE9F
        std::array<Byte, IO_REG_SIZE> io_registers;        // 0xFF00 - 0xFF7F
        std::array<Byte, HRAM_SIZE> hram;                  // 0xFF80 - 0xFFFE
        Byte ie_register;                                  // 0xFFFF

        std::pair<Byte*, size_t> resolve_region(const Address address);
        void initialize_mbc();
        Address convert_echo_RAM_address(const Address address) const;
};

#endif // MEMORY_H