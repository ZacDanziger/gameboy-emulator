#ifndef MEMORY_H
#define MEMORY_H

#include <array>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include "memory_map.h"
#include "../utils/utils.h"
#include "../timer/timer.h"
#include "../ppu/ppu.h"
#include "../mbc/mbc.h"


/**
 * Memory Management Unit
 * Represents all memory in the gameboy
 * Byte-addressable 16-bit address size
 *      currently handles ROM, VRAM, WRAM, ECHO RAM, I/O REGISTERS, HRAM, and IE REGISTER
 *      currently does not handle ERAM, and OAM
 * TODO: implement VRAM lockout
*/
class MMU {
    public:
        MMU() :
            wram_bank_00{},
            wram_bank_nn{},
            io_registers{},
            hram{},
            ie_register(0x00),
            button_keys(0x0F),
            direction_keys(0x0F)
        {}

        void init(Timer* timer_ptr, PPU* ppu_ptr);

        Byte read(const Address address) const;
        void write(const Address address, const Byte data);
        
        void load_rom(const std::string& filename);

        void request_interrupt(Interrupt interrupt);
        void set_key(Key key, bool pressed);
    private:
        Timer *timer;
        PPU *ppu;
        std::unique_ptr<MBC> mbc;

        std::array<Byte, WRAM_BANK_SIZE> wram_bank_00;     // 0xC000 - 0xCFFF
        std::array<Byte, WRAM_BANK_SIZE> wram_bank_nn;     // 0xD000 - 0xDFFF

        // PROBABLY DIVIDE UP
        std::array<Byte, IO_REG_SIZE> io_registers;        // 0xFF00 - 0xFF7F

        std::array<Byte, HRAM_SIZE> hram;                  // 0xFF80 - 0xFFFE
        Byte ie_register;                                  // 0xFFFF

        // the 2x4 grid of values for JOYP
        Byte button_keys;
        Byte direction_keys;

        void oam_dma_transfer(const Byte value);
};

#endif // MEMORY_H