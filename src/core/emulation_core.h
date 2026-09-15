#ifndef EMULATION_CORE_H
#define EMULATION_CORE_H


#include "apu/apu.h"
#include "cpu/cpu.h"
#include "interrupt/interrupt_controller.h"
#include "joypad/joypad.h"
#include "memory/memory_bus.h"
#include "ppu/ppu.h"
#include "timer/timer.h"

#include "../frontend/frontend.h"




class EmulationCore {
    public:
        EmulationCore():
            interrupt(),
            joypad(interrupt),
            ppu(interrupt, [this]{ this->on_frame_ready(); }, [this]{ memory_bus.hdma_tick(); }),
            apu(),
            timer(interrupt, [this]{ ppu.update(); apu.update(); }, [this]{ apu.frame_sequencer(); }),
            memory_bus(interrupt, joypad, timer, ppu, apu),
            cpu(interrupt, joypad, timer, memory_bus),
            
            frame_complete(false)
        {}
        
        void load(const std::string& rom_file);
        void run_until_frame();
    private:
        InterruptController interrupt;
        Joypad joypad;
        PPU ppu;
        APU apu;
        Timer timer;
        MemoryBus memory_bus;
        CPU cpu;

        bool frame_complete;

        void reset();
};


#endif // EMULATION_CORE_H