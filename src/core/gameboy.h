#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <memory>

#include "types.h"
#include "memory_map.h"

#include "utils/file_io.h"

#include "cartridge/cartridge.h"
#include "interrupt/interrupt_controller.h"
#include "joypad/joypad.h"
#include "timer/timer.h"
#include "ppu/ppu.h"
#include "apu/apu.h"
#include "bus/memory_bus.h"
#include "cpu/cpu.h"

constexpr uint16_t SPEED_SWITCH_DELAY = 0x0802; // 0d2050 m-cycles 

class GameBoy {
    public:
        GameBoy():
            interrupt(),
            joypad(interrupt),
            timer(interrupt),
            ppu(interrupt),
            apu(),
            bus(interrupt, joypad, timer, ppu, apu),
            cpu(interrupt, bus),

            cgb_mode(false),

            speed_switch_delay_counter(0x0000)
        {}

        ~GameBoy() {
            save();
        }
        
        void reset();

        void save() { bus.save(); }
        void load_rom(const std::string& rom_file);

        void run_until_frame();

        Frame flush_frame() { return ppu.get_frame(); }
        std::vector<float> flush_audio() { return apu.flush_audio_buffer(); }
        
        void set_button_state(const ButtonState& state) { joypad.set_button_state(state); }
        
        bool is_cgb() const { return cgb_mode; }
    private:
        InterruptController interrupt;
        Joypad joypad;
        Timer timer;
        PPU ppu;
        APU apu;
        MemoryBus bus;
        CPU cpu;



        bool cgb_mode;

        uint16_t speed_switch_delay_counter;
};


#endif // GAMEBOY_H