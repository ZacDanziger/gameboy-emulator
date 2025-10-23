#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include "memory.h"

/*
ROM bank 00-----------------------0x0000 - 0x3FFF
ROM banks 01-NN-------------------0x4000 - 0x7FFF
Video RAM-------------------------0x8000 - 0x9FFF
External RAM----------------------0xA000 - 0xBFFF
Work RAM--------------------------0xC000 - 0xCFFF
Switchable Banks 1-7--------------0xD000 - 0xDFFF
Echo RAM--------------------------0xE000 - 0xFDFF
Object Attribute Memory (OAM)-----0xFE00 - 0xFE9F
NOT USABLE------------------------0xFEA0 - 0xFEFF
I/O Registers---------------------0xFF00 - 0xFF7F
High RAM--------------------------0xFF80 - 0xFFFE
Interrupt Enable Register---------0xFFFF
*/
const uint16_t ROM_END = 0x7FFF;
const uint16_t VRAM_END = 0x9FFF;
const uint16_t ERAM_END = 0xBFFF;
const uint16_t WRAM_END = 0xDFFF;
const uint16_t ECHO_END = 0xFDFF;
const uint16_t OAM_END = 0xFE9F;
const uint16_t HRAM_START = 0xFF80;
const uint16_t HRAM_END = 0xFFFE;
const uint16_t IE_REGISTER = 0xFFFF;

// Needs Error Handling
class Interconnect {
    public:
        Interconnect(const std::vector<uint8_t>& rom_data);

        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
    
    private:
        ROM _rom;
        Memory _vram;
        Memory _wram;
        Memory _hram;

        Memory& resolve_device(uint16_t address);
};

#endif