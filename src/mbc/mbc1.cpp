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
        adjusted_address = address & 0x3FFF;
        adjusted_address |= (rom_bank << 14);
        adjusted_address |= (secondary_bank << 19);

        return rom[adjusted_address];
    }

    // read from eram
    if ((address >= ERAM_START) && (address < WRAM_BANK_00_START)) {
        if (!ram_enable) {
            return 0xFF;
        }
        if (banking_mode) {
            // mask top two bits and replace them with the value of the secondary_bank register
            adjusted_address = address & 0x3FFF;
            adjusted_address |= (secondary_bank << 13);

            return eram[adjusted_address - ERAM_START];
        }

        return eram[address - ERAM_START];
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

        // clamp rom bank selected to number of rom banks available
        rom_bank %= num_rom_banks;
        return;
    }

    // RAM bank number OR upper bits of ROM bank number
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
        if (!ram_enable) {
            return;
        }
        eram[address - ERAM_START] = data;
        return;
    }

    throw std::runtime_error("MBC write called on wrong address");
}