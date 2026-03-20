#include "mbc3.h"

Byte MBC3::read(const Address address) const {
    uint32_t adjusted_address = 0x00000000;

    // read from bank 00
    if (address < ROM_BANK_NN_START) {
        return rom[address];
    }

    // read from bank nn
    if (address < VRAM_START) {
        adjusted_address = address - ROM_BANK_NN_START;
        adjusted_address += (rom_bank * ROM_BANK_SIZE);

        return rom[adjusted_address];
    }

    // read from eram
    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        if (!ram_enable) {
            return 0xFF;
        }

        if (secondary_bank >= 0x08) {
            return rtc_read();
        }

        if (eram.size() == 0) {
            return 0xFF;
        }

        adjusted_address = address - ERAM_START;
        adjusted_address += (secondary_bank * ERAM_BANK_SIZE);

        // if address is out of bounds, wrap around
        adjusted_address %= eram.size();

        return eram[adjusted_address];
    }

    throw std::runtime_error("MBC read called on incorrect address");
}


void MBC3::write(const Address address, const Byte data) {
    // enable RAM
    if (address < 0x2000) {
        ram_enable = ((data & 0x0F) == 0x0A);
        return;
    }

    // ROM bank number
    if (address < 0x4000) {
        rom_bank = (data & 0x7F);
        if (rom_bank == 0) {
            rom_bank = 1;
        }

        // clamp rom bank selected to number of rom banks available
        rom_bank %= num_rom_banks;
        return;
    }

    // Secondary bank number
    if (address < 0x6000) {
        if (data <= 0x0C){
            secondary_bank = data;
        }
        return;
    }

    // Latch clock data
    if (address < 0x8000) {
        if ((data == 0x01) && (latch_prev == 0x00)) {
            // Do nothing
            // If / when I implement more accurate RTC handling this will be important
        }
        latch_prev = data;
        return;
    }

    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        if ((!ram_enable) || (eram.size() == 0)) {
            return;
        }
        uint32_t adjusted_address = 0x00000000;

        adjusted_address = address - ERAM_START;
        if (secondary_bank < 0x08) {
            adjusted_address += (secondary_bank * ERAM_BANK_SIZE);
        }

        adjusted_address %= eram.size();

        eram[adjusted_address] = data;
        return;
    }

    throw std::runtime_error("MBC write called on incorrect address");
}

Byte MBC3::rtc_read() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&t);

    switch(secondary_bank) {
    case 0x08:
        return tm->tm_sec;
    case 0x09:
        return tm->tm_min;
    case 0x0A:
        return tm->tm_hour;
    case 0x0B:
        return tm->tm_yday & 0xFF;
    case 0x0C:
        return (tm->tm_yday >> 8) & 0x01;
    default:
        throw std::runtime_error("RTC read called on incorrect secondary bank value");
    }
}


void MBC3::save(const std::string& filename) const {
    if (!has_battery) {
        return;
    }

    std::string save_file = filename.substr(0, filename.find_last_of('.')) + ".sav";

    write_file(save_file, eram);

    return;
}


void MBC3::load(const std::string& filename) {
    if (!has_battery) {
        return;
    }

    std::string save_file = filename.substr(0, filename.find_last_of('.')) + ".sav";

    // confirm save file exists
    std::ifstream file(save_file);
    if (!file.is_open()) return;
    file.close();

    eram = read_file(save_file);

    return;
}