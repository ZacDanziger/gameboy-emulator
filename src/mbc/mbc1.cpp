#include "mbc1.h"

Byte MBC1::read(const Address address) const {
    uint32_t adjusted_address = 0x00000000;

    // read from bank 00
    if (address < ROM_BANK_NN_START) {
        if (banking_mode) {
            adjusted_address = (secondary_bank << 19) | address;
            return rom[adjusted_address];
        }

        return rom[address];
    }

    // read from bank nn
    if (address < VRAM_START) {
        Address adjusted_rom_bank = (secondary_bank << 5) | rom_bank;

        adjusted_rom_bank %= num_rom_banks;

        adjusted_address = address - ROM_BANK_NN_START;
        adjusted_address += (adjusted_rom_bank * ROM_BANK_SIZE);

        return rom[adjusted_address];
    }

    // read from eram
    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        if ((!ram_enable) || (eram.size() == 0)) {
            return 0xFF;
        }

        adjusted_address = address - ERAM_START;

        if (banking_mode) {
            adjusted_address += (secondary_bank * ERAM_BANK_SIZE);
        }

        // if address is out of bounds, wrap around
        adjusted_address %= eram.size();

        return eram[adjusted_address];
    }

    throw std::runtime_error("MBC read called on incorrect address");
}


void MBC1::write(const Address address, const Byte data) {
    // enable RAM
    if (address < 0x2000) {
        ram_enable = ((data & 0x0F) == 0x0A);
        return;
    }

    // ROM bank number
    if (address < 0x4000) {
        rom_bank = (data & 0x1F);
        if (rom_bank == 0) {
            rom_bank = 1;
        }

        // // clamp rom bank selected to number of rom banks available
        // rom_bank %= num_rom_banks;
        return;
    }

    // Secondary bank number
    if (address < 0x6000) {
        secondary_bank = data & 0x03;
        return;
    }

    // banking mode select
    if (address < 0x8000) {
        banking_mode = is_set(data, Bit::Bit0);
        return;
    }

    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        if ((!ram_enable) || (eram.size() == 0)) {
            return;
        }
        uint32_t adjusted_address = 0x00000000;

        adjusted_address = address - ERAM_START;

        if (banking_mode) {
            adjusted_address += (secondary_bank * ERAM_BANK_SIZE);
        }

        adjusted_address %= eram.size();

        eram[adjusted_address] = data;
        return;
    }

    throw std::runtime_error("MBC write called on wrong address");
}


void MBC1::save(const std::string& filename) const {
    if (!has_battery) {
        return;
    }

    std::string save_file = filename.substr(0, filename.find_last_of('.')) + ".sav";

    write_file(save_file, eram);
    return;
}


void MBC1::load(const std::string& filename) {
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