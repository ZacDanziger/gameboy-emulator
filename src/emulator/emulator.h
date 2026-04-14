#ifndef EMULATOR_H
#define EMULATOR_H

#include <thread>
#include <chrono>

#include <iostream>

#include "../memory/memory_bus.h"
#include "../timer/timer.h"
#include "../cpu/cpu.h"
#include "../ppu/ppu.h"
#include "../apu/apu.h"
#include "../frontend/frontend.h"

constexpr auto FRAME_DURATION = std::chrono::nanoseconds(16742706); // ~59.7 fps
constexpr auto AUTOSAVE_INTERVAL = std::chrono::minutes(5);
const std::string EMULATOR_NAME = "GameBoy Color Emulator";

class Emulator {
    public:
        Emulator():
            interrupt(),
            joypad(interrupt, [this]{ memory_bus.save(); }),
            ppu(interrupt, [this]{ this->on_frame_ready(); }, [this]{ memory_bus.hdma_tick(); }),
            apu(),
            timer(interrupt, [this]{ ppu.update(); apu.update(); }, [this]{ apu.frame_sequencer(); }),
            memory_bus(interrupt, joypad, timer, ppu, apu, [this](bool active){ dma_active = active; }),
            cpu(interrupt, joypad, timer, memory_bus),
            frontend(EMULATOR_NAME, joypad),
            
            dma_active(false),
            frame_complete(false),

            last_frame_time(std::chrono::steady_clock::now()),
            last_save_time(std::chrono::steady_clock::now())
        {}
        
        void load(const std::string& rom_file);
        void run();
    private:
        InterruptController interrupt;
        Joypad joypad;
        PPU ppu;
        APU apu;
        Timer timer;
        MemoryBus memory_bus;
        CPU cpu;
        Frontend frontend;

        bool dma_active;
        bool frame_complete;

        std::chrono::steady_clock::time_point last_frame_time;
        std::chrono::steady_clock::time_point last_save_time;

        void reset();
        void on_frame_ready();

        void update_fps();
        void throttle();
        void autosave();
};


#endif // EMULATOR_H