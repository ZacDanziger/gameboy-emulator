#ifndef MBC_H
#define MBC_H

#include <array>
#include <vector>
#include "../types.h"
#include "memory_map.h"

/**
 * Not finished implementing
 */
class MBC {
    public:
        MBC();

        void init(Byte mbc_type, Byte rom_size, Byte ram_size);

        int num_rom_banks() { return rom_banks.size(); }
        int num_ram_banks() { return ram_banks.size(); }

        Byte read(const Address address) const;
        void write(const Address address, const Byte value);
    private:
        std::vector<std::array<Byte, ROM_BANK_SIZE>> rom_banks;
        std::vector<std::array<Byte, ERAM_SIZE>> ram_banks;

        Byte rom_bank_selector;
        Byte ram_bank_selector;

};

#endif // MBC_H