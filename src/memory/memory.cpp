#include "memory.h"

Memory::Memory() {
    _rom_bank_00 = {0};
    _rom_bank_01 = {0};
    _vram = {0};
    _wram_bank_00 = {0};
    _wram_bank_01 = {0};
    _hram = {0};
}

Byte Memory::read(Address address) {
    if (address < ROM_BANK_01_START) {
        return _rom_bank_00[address];
    }

    if (address < VRAM_START) {
        return _rom_bank_01[address - ROM_BANK_01_START];
    }

    if (address < ERAM_START) {
        return _vram[address - VRAM_START];
    }

    if (address < WRAM_BANK_01_START && address >= WRAM_BANK_00_START) {
        return _wram_bank_00[address - WRAM_BANK_00_START];
    }

    if (address < ECHO_START) {
        return _wram_bank_01[address - WRAM_BANK_01_START];
    }

    if (address <= HRAM_START && address < IE_REGISTER) {
        return _hram[address - HRAM_START];
    }

    throw std::runtime_error("Cannot read from that area of memory");
}

void Memory::write(Address address, Byte data) {
    if (address < ROM_BANK_01_START) {
        _rom_bank_00[address] = data;
        return;
    }
    if (address < VRAM_START) {
        _rom_bank_01[address - ROM_BANK_01_START] = data;
        return;
    }
    if (address < ERAM_START) {
        _vram[address - VRAM_START] = data;
        return;
    }
    if (address < WRAM_BANK_01_START && address >= WRAM_BANK_00_START) {
        _wram_bank_00[address - WRAM_BANK_00_START] = data;
        return;
    }
    if (address < ECHO_START) {
        _wram_bank_01[address - WRAM_BANK_01_START] = data;
        return;
    }
    if (address <= HRAM_START && address < IE_REGISTER) {
        _hram[address - HRAM_START] = data;
        return;
    }

    throw std::runtime_error("Cannot write to that area of memory");
}

// Need to be able to select rom bank 00 or 01
void Memory::load_rom(std::string file) {
    std::ifstream infile(file, std::ios::binary);
    if (!infile.is_open()) {
        throw std::runtime_error("Cannot open file");
    }

    std::array<Byte, ROM_BANK_SIZE> buffer = {0};

    infile.read(reinterpret_cast<char*>(buffer.data()), (buffer.size()));
    // for (int i = 0; i < ROM_BANK_SIZE; i++) {
    //     infile >> buffer[i];
    // }

    infile.close();

    _rom_bank_00 = buffer;
}