#include "mbc.h"

MBC::MBC() {
    rom_bank_selector = 0x00;
    ram_bank_selector = 0x00;
}

void MBC::init(Byte mbc_type, Byte rom_size, Byte ram_size) {
    // not handling 0x52, 0x53, and 0x54 as they're only listed in unofficial docs and are not well documented
    if (rom_size > 0x08) {  
        throw std::runtime_error("Invalid Cartridge ROM size selected at memory[0x0148]");
    }
    if (ram_size > 0x05) {
        throw std::runtime_error("Invalid Cartridge RAM size selected at memory[0x0149]");
    }

    int max_rom_banks = 0;
    int max_ram_banks = 0;

    int num_rom_banks = (1 << (rom_size + 1));  // 2 if 00, 4 if 01, etc
    int num_ram_banks = 0;

    switch(ram_size) {
    case 0x02:
        num_ram_banks = 1;
        break;
    case 0x03:
        num_ram_banks = 4;
        break;
    case 0x04:
        num_ram_banks = 16;
        break;
    case 0x05:
        num_ram_banks = 8;
        break;
    }

    switch(mbc_type) {
    case 0x00:
        // No MBC
        max_rom_banks = 2;
        break;
    case 0x01:
        // MBC1
        // Max 2MBytes of ROM and/or 32 KB of RAM
        max_rom_banks = 32;
        break;
    case 0x02:
        // MBC1 + RAM
        max_rom_banks = 128;
        max_ram_banks = 4;
        break;
    case 0x03:
        // MBC1 + RAM + BATTERY
        break;
    case 0x05:
        // MBC2
        break;
    case 0x06:
        // MBC2 + BATTERY
        break;
    case 0x08:
        // ROM + RAM    (unused, do I need to implement)
        break;
    case 0x09:
        // ROM + RAM + BATTERY (unused, do I need to implement)
        break;
    case 0x0B:
        // MMM01
        break;
    case 0x0C:
        // MMM01 + RAM
        break;
    case 0x0D:
        // MMM01 + RAM + BATTERY
        break;
    case 0x0F:
        // MBC3 + TIMER + BATTERY
        break;
    case 0x10:
        // MBC3 + TIMER + RAM + BATTERY
        break;
    case 0x11:
        // MBC3
        break;
    case 0x12:
        // MBC3 + RAM
        break;
    case 0x13:
        // MBC3 + RAM + BATTERY
        break;
    case 0x19:
        // MBC5
        break;
    case 0x1A:
        // MBC5 + RAM
        break;
    case 0x1B:
        // MBC5 + RAM + BATTERY
        break;
    case 0x1C:
        // MBC5 + RUMBLE
        break;
    case 0x1D:
        // MBC5 + RUMBLE + RAM
        break;
    case 0x1E:
        // MBC5 + RUMBLE + RAM + BATTERY
        break;
    case 0x20:
        // MBC6
        break;
    case 0x22:
        // MBC7 + SENSOR + RUMBLE + RAM + BATTERY
        break;
    case 0xFC:
        // POCKET CAMERA
        break;
    case 0xFD:
        // BANDAI TAMA5
        break;
    case 0xFE:
        // HuC3
        break;
    case 0xFF:
        // HuC1 + RAM + BATTERY
        break;
    default:
        throw std::runtime_error("Invalid MBC Type selected at memory[0x0147]");
    }

    if (num_rom_banks > max_rom_banks) {
        throw std::runtime_error("Number ROM banks selected greater than maximum allowed by MBC type");
    }

    if (num_ram_banks > max_ram_banks) {
        throw std::runtime_error("Number RAM banks selected greater than maximum allowed by MBC type");
    }

    rom_banks = std::vector<std::array<Byte, ROM_BANK_SIZE>>(num_rom_banks);
    ram_banks = std::vector<std::array<Byte, ERAM_SIZE>>(num_ram_banks);
}