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
 * Byte-addressable 16-bit address size
*/
class MMU {
    public:
        MMU() :
            wram{},
            io_registers{},

            prep_speed_switch(0x7E),
            vram_source_high(0xFF),
            vram_source_low(0xFF),
            vram_dest_high(0xFF),
            vram_dest_low(0xFF),
            vram_dma_control(0xFF),
            wram_bank(0x01),

            hram{},
            ie_register(0x00),

            button_keys(0x0F),
            direction_keys(0x0F),

            rom_filename(),

            cgb_mode(false),
            hdma_state{}
        {
            write(IF_REGISTER, 0xE1);
        }
        
        ~MMU() {
            if (mbc) {
                mbc->save(rom_filename);
            }
        }

        inline void init(Timer* timer_ptr, PPU* ppu_ptr) { timer = timer_ptr; ppu = ppu_ptr; }

        Byte read(const Address address) const;
        void write(const Address address, const Byte data);
        
        void load_rom(const std::string& filename);

        void request_interrupt(Interrupt interrupt);
        void set_key(Key key, bool pressed);

        void hdma_tick();

        inline bool any_button_pressed() const { return ((button_keys != 0x0F) || (direction_keys != 0x0F)); }

        void print_tiles(const std::string& filename) { ppu->dump_tiles_ppm(filename); }
    private:
        Timer *timer;
        PPU *ppu;
        std::unique_ptr<MBC> mbc;

        std::array<Byte, 8 * WRAM_BANK_SIZE> wram;         // 0xC000 - 0xDFFF

        // PROBABLY DIVIDE UP
        std::array<Byte, IO_REG_SIZE> io_registers;        // 0xFF00 - 0xFF7F

        // CGB Registers
        Byte prep_speed_switch;                            // KEY1_SPD_REGISTER
        Byte vram_source_high;                             // HDMA1_REGISTER
        Byte vram_source_low;                              // HDMA2_REGISTER
        Byte vram_dest_high;                               // HDMA3_REGISTER
        Byte vram_dest_low;                                // HDMA4_REGISTER
        Byte vram_dma_control;                             // HDMA5_REGISTER
        Byte wram_bank;                                    // SVBK_WBK_REGISTER

        std::array<Byte, HRAM_SIZE> hram;                  // 0xFF80 - 0xFFFE
        Byte ie_register;                                  // 0xFFFF

        // the 2x4 grid of values for JOYP
        Byte button_keys;
        Byte direction_keys;

        std::string rom_filename;
        bool cgb_mode;

        struct HDMAState {
            bool active = false;
            Address source = 0x0000;
            Address destination = 0x0000;
            Word remaining = 0x0000;
        };

        HDMAState hdma_state;

        void oam_dma_transfer(const Byte value);
        void vram_dma_transfer(const Byte value);
        
};

#endif // MEMORY_H