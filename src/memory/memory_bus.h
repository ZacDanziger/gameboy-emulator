#ifndef MEMORY_BUS_H
#define MEMORY_BUS_H

#include <array>
#include <vector>
#include <memory>

#include "../memory_map.h"
#include "../types.h"
#include "../utils/utils.h"
#include "../interrupt/interrupt_controller.h"
#include "../joypad/joypad.h"
#include "../ppu/ppu.h"
#include "../apu/apu.h"
#include "../timer/timer.h"
#include "../mbc/mbc.h"


/**
 * Memory Management Unit
 * Byte-addressable 16-bit address size
*/
class MemoryBus {
    public:
        MemoryBus(InterruptController& i, Joypad& j, Timer& t, PPU& p, APU& a) :
            interrupt(i),
            joypad(j),
            ppu(p),
            apu(a),
            timer(t),

            wram{},
            // io_registers{},
            hram{},

            prep_speed_switch(0x7E),
            vram_source_high(0xFF),
            vram_source_low(0xFF),
            vram_dest_high(0xFF),
            vram_dest_low(0xFF),
            vram_dma_control(0xFF),
            wram_bank(0x01),

            rom_filename(),

            cgb_mode(false),

            hdma_active(false),
            hdma_source(0x0000),
            hdma_destination(0x0000),
            hdma_remaining(0x0000)
        {}
        
        ~MemoryBus() {
            save();
        }

        void reset();
        void load_rom(const std::string& filename);

        Byte read(const Address address) const;
        void write(const Address address, const Byte data);

        void hdma_tick();

        void save () {if (mbc) { mbc->save(rom_filename); }}
    private:
        InterruptController& interrupt;
        Joypad& joypad;
        PPU& ppu;
        APU& apu;
        Timer& timer;

        std::unique_ptr<MBC> mbc;

        std::array<Byte, 8 * WRAM_BANK_SIZE> wram;         // 0xC000 - 0xDFFF
        std::array<Byte, HRAM_SIZE> hram;                  // 0xFF80 - 0xFFFE

        std::string rom_filename;
        
        bool cgb_mode;
        
        // CGB Registers
        Byte prep_speed_switch;                            // KEY1_SPD_REGISTER
        Byte vram_source_high;                             // HDMA1_REGISTER
        Byte vram_source_low;                              // HDMA2_REGISTER
        Byte vram_dest_high;                               // HDMA3_REGISTER
        Byte vram_dest_low;                                // HDMA4_REGISTER
        Byte vram_dma_control;                             // HDMA5_REGISTER
        Byte wram_bank;                                    // SVBK_WBK_REGISTER

        bool hdma_active;
        Address hdma_source;
        Address hdma_destination;
        Word hdma_remaining;

        void oam_dma_transfer(const Byte value);
        void vram_dma_transfer(const Byte value);
};

#endif // MEMORY_BUS_H