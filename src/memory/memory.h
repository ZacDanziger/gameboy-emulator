#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include <stdint.h>
#include <array>
#include <stdexcept>


typedef uint8_t Byte;
typedef uint16_t Address;

/*
ROM bank 00-----------------------0x0000 - 0x3FFF
ROM banks 01-NN-------------------0x4000 - 0x7FFF
Video RAM-------------------------0x8000 - 0x9FFF
External RAM----------------------0xA000 - 0xBFFF
Work RAM--------------------------0xC000 - 0xCFFF
Switchable Banks 1-7--------------0xD000 - 0xDFFF
Echo RAM--------------------------0xE000 - 0xFDFF
Object Attribute Memory (OAM)-----0xFE00 - 0xFE9F
NOT USABLE!!!---------------------0xFEA0 - 0xFEFF
I/O Registers---------------------0xFF00 - 0xFF7F
High RAM--------------------------0xFF80 - 0xFFFE
Interrupt Enable Register---------0xFFFF
*/
const Address ROM_BANK_01_START  = 0x4000;
const Address VRAM_START         = 0x8000;
const Address ERAM_START         = 0xA000;
const Address WRAM_BANK_00_START = 0xC000;
const Address WRAM_BANK_01_START = 0xD000;
const Address ECHO_START         = 0xE000;
const Address OAM_START          = 0xFE00;
const Address OAM_END            = 0xFE9F;
const Address IO_START           = 0xFF00;
const Address HRAM_START         = 0xFF80;
const Address IE_REGISTER        = 0xFFFF;

/*
VRAM size = 0x9FFF - 0x7FFF = 0x2000
WRAM size = 0xDFFF - 0xBFFF = 0x2000
HRAM size = 0xFFFE - 0xFF7F = 0x007F
*/
const uint16_t ROM_BANK_SIZE  = 0x4000;
const uint16_t VRAM_SIZE      = 0x2000;
const uint16_t WRAM_BANK_SIZE = 0x1000;
const uint16_t HRAM_SIZE      = 0x007F;

/*
    Represents all memory in the gameboy
    ** currently handles ROM, VRAM, WRAM, and HRAM
    ** currently does not handle ERAM, ECHO RAM, OAM, I/O REGISTERS, and IE REGISTER
*/
class Memory {
    public:
        Memory();

        Byte read(Address address);
        void write(Address address, Byte data);

        void load_rom(std::string file);
    
    private:
        std::array<Byte, ROM_BANK_SIZE> _rom_bank_00;       // 0x0000 - 0x3FFF
        std::array<Byte, ROM_BANK_SIZE> _rom_bank_01;       // 0x4000 - 0x7FFF
        std::array<Byte, VRAM_SIZE> _vram;                  // 0x8000 - 0x9FFF
        std::array<Byte, WRAM_BANK_SIZE> _wram_bank_00;     // 0xC000 - 0xCFFF
        std::array<Byte, WRAM_BANK_SIZE> _wram_bank_01;     // 0xD000 - 0xDFFF
        std::array<Byte, HRAM_SIZE> _hram;                  // 0xFF80 - 0xFFFE
};

#endif