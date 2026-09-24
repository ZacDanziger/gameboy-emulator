#ifndef MEMORY_BUS_H
#define MEMORY_BUS_H

#include <array>
#include <vector>
#include <string>

#include "../types.h"
#include "../memory_map.h"

#include "../interrupt/interrupt_controller.h"
#include "../joypad/joypad.h"
#include "../timer/timer.h"
#include "../ppu/ppu.h"
#include "../apu/apu.h"
#include "../cartridge/cartridge.h"

class MemoryBus {
    public:
        MemoryBus(InterruptController& interrupt, Joypad& joypad, Timer& timer, PPU& ppu, APU& apu):
            interrupt(interrupt),
            joypad(joypad),
            timer(timer),
            ppu(ppu),
            apu(apu),

            wram{},
            hram{},

            cgb_mode(false),

            key1_register(0x7E),
            vram_source_high(0xFF),
            vram_source_low(0xFF),
            vram_dest_high(0xFF),
            vram_dest_low(0xFF),
            vram_dma_control(0xFF),
            wram_bank(0x01),

            hdma_chunk_buffer(16),

            hdma_active(false),
            hdma_source(0x0000),
            hdma_destination(0x0000),
            hdma_remaining(0x0000),

            dma_delay_counter(0x0000)
        {}

        void reset();

        void save() { if (cartridge) { cartridge->save(); } }
        void load(const std::string& rom_file);

        Byte read(const Address address) const;
        void write(const Address address, const Byte data);

        bool tick_dma_counter();
        void hdma_tick();

        bool check_joypad_pressed() const { return joypad.any_button_pressed(); }

        void set_cgb_mode(const bool is_cgb) { cgb_mode = is_cgb; }
        bool get_cgb_mode() const { return cgb_mode; }
        bool get_hdma_active() const { return hdma_active; }
    private:
        InterruptController& interrupt;
        Joypad& joypad;
        Timer& timer;
        PPU& ppu;
        APU& apu;

        std::unique_ptr<Cartridge> cartridge;

        std::array<Byte, 8 * WRAM_BANK_SIZE> wram;         // 0xC000 - 0xDFFF
        std::array<Byte, HRAM_SIZE> hram;                  // 0xFF80 - 0xFFFE

        std::vector<Byte> hdma_chunk_buffer;

        bool cgb_mode;

        // CGB Registers
        Byte key1_register;                                // KEY1_SPD_REGISTER
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

        uint16_t dma_delay_counter;

        void oam_dma_transfer(const Byte value);
        void vram_dma_transfer(const Byte value);
};

#endif // MEMORY_BUS_H