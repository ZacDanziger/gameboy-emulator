#ifndef MBC3_H
#define MBC3_H

#include "mbc.h"

class MBC3 : public MBC {
    public:
        MBC3(std::vector<Byte> rom, size_t ram_size, bool has_battery) :
            rom(std::move(rom)),
            eram(ram_size, 0xFF),

            rom_bank(1),
            secondary_bank(0),

            latch_prev(0xFF),

            ram_enable(false),
            has_battery(has_battery)
        {
            num_rom_banks = rom.size() / ROM_BANK_SIZE;
        }

        Byte read(const Address address) const override;
        void write(const Address address, const Byte data) override;

        void save(const std::string& filename) const override;
        void load(const std::string& filename) override;
    private:
        std::vector<Byte> rom;
        std::vector<Byte> eram;

        Byte rom_bank;
        Byte secondary_bank;

        Byte latch_prev;
        size_t num_rom_banks;

        bool ram_enable;
        bool has_battery;

        Byte rtc_read() const;
};

#endif  // MBC3_H