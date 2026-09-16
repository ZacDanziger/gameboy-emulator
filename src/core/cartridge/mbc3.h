#ifndef MBC3_H
#define MBC3_H

#include <vector>
#include <utility>
#include <stdexcept>
#include <chrono>

#include "mbc.h"
#include "../types.h"
#include "../memory_map.h"
#include "../utils/bit_utils.h"
#include "../utils/file_io.h"

class MBC3 : public MBC {
    public:
        MBC3(std::vector<Byte> rom, size_t ram_size, bool has_battery) :
            rom(std::move(rom)),
            eram(ram_size, 0xFF),

            rom_bank(1),
            secondary_bank(0),

            latch_prev(0xFF),
            latched_seconds(0x00),
            latched_minutes(0x00),
            latched_hours(0x00),
            latched_days_low(0x00),
            latched_days_high(0x00),

            ram_enable(false),
            has_battery(has_battery)
        {
            num_rom_banks = rom.size() / ROM_BANK_SIZE;
            rtc_start_time = std::chrono::system_clock::now();
        }

        Byte read(const Address address) const override;
        void write(const Address address, const Byte data) override;

        void save() const override;
        void load(const std::string& filename) override;
    private:
        std::vector<Byte> rom;
        std::vector<Byte> eram;

        Byte rom_bank;
        Byte secondary_bank;

        Byte latch_prev;
        Byte latched_seconds;
        Byte latched_minutes;
        Byte latched_hours;
        Byte latched_days_low;
        Byte latched_days_high;

        std::chrono::system_clock::time_point rtc_start_time;

        size_t num_rom_banks;

        bool ram_enable;
        bool has_battery;

        Byte rtc_read() const;
        void latch_time();
};

#endif  // MBC3_H