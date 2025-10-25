#include "memory.h"

Memory::Memory() {
    _rom_bank_00  = {0};
    _rom_bank_01  = {0};
    _vram         = {0};
    _wram_bank_00 = {0};
    _wram_bank_01 = {0};
    _hram         = {0};
}

Byte Memory::read(Address address) {
    if (address < ROM_BANK_01_START)                    { return _rom_bank_00[address]; }
    if (address < VRAM_START)                           { return _rom_bank_01[address - ROM_BANK_01_START]; }
    if (address < ERAM_START)                           { return _vram[address - VRAM_START]; }
    if (address < WRAM_BANK_01_START)                   { return _wram_bank_00[address - WRAM_BANK_00_START]; }
    if (address < ECHO_START)                           { return _wram_bank_01[address - WRAM_BANK_01_START]; }
    if (address <= HRAM_START && address < IE_REGISTER) { return _hram[address - HRAM_START]; }
}

void Memory::write(Address address, Byte data) {
    if (address < ROM_BANK_01_START)                    { _rom_bank_00[address] = data; }
    if (address < VRAM_START)                           { _rom_bank_01[address - ROM_BANK_01_START] = data; }
    if (address < ERAM_START)                           { _vram[address - VRAM_START] = data; }\
    if (address < WRAM_BANK_01_START)                   { _wram_bank_00[address - WRAM_BANK_00_START] = data; }
    if (address < ECHO_START)                           { _wram_bank_01[address - WRAM_BANK_01_START] = data; }
    if (address <= HRAM_START && address < IE_REGISTER) { _hram[address - HRAM_START] = data; }
}

void Memory::load_rom(std::string file) {
    
}