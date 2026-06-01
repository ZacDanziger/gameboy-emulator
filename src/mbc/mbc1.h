#ifndef MBC1_H
#define MBC1_H

#include <vector>
#include <utility>
#include <stdexcept>

#include "mbc.h"
#include "../types.h"
#include "../memory_map.h"
#include "../utils/utils.h"

class MBC1 : public MBC {
    public:
        MBC1(std::vector<Byte> rom, size_t ram_size, bool has_battery) :
            rom(std::move(rom)),
            eram(ram_size, 0xFF),

            rom_bank(1),
            secondary_bank(0),
            ram_enable(false),
            banking_mode(false),
            has_battery(has_battery)
        {
            num_rom_banks = rom.size() / ROM_BANK_SIZE;
        }

        Byte read(const Address address) const override;
        void write(const Address address, const Byte data) override;

        void save() const override;
        void load(const std::string& filename) override;
    private:
        std::vector<Byte> rom;
        std::vector<Byte> eram;

        Byte rom_bank;
        Byte secondary_bank;    // either RAM bank number, or upper 2 bits of ROM bank number

        size_t num_rom_banks;

        bool ram_enable;
        bool banking_mode;

        bool has_battery;
};

#endif  // MBC1_H