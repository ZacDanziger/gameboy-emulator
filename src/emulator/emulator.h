#ifndef EMULATOR_H
#define EMULATOR_H

#include <thread>
#include <chrono>

#include "../memory/memory_bus.h"
#include "../timer/timer.h"
#include "../cpu/cpu.h"
#include "../ppu/ppu.h"
#include "../frontend/frontend.h"

constexpr auto FRAME_DURATION = std::chrono::nanoseconds(16742706); // ~59.7 fps
constexpr auto AUTOSAVE_INTERVAL = std::chrono::minutes(5);

const std::string EMULATOR_NAME = "GameBoy Color Emulator";

class Emulator {
    public:
        Emulator():
            interrupt(),
            joypad(interrupt),
            ppu(interrupt, [this]{ this->on_frame_ready(); }, [this]{ memory_bus.hdma_tick(); }),
            timer(interrupt, [this]{ ppu.update(); }),
            memory_bus(interrupt, joypad, ppu, timer),
            cpu(interrupt, joypad, timer, memory_bus),
            frontend(EMULATOR_NAME, joypad),
            
            last_frame_time(std::chrono::steady_clock::now()),
            last_save_time(std::chrono::steady_clock::now())
        {}
        
        void load(const std::string& rom_file) { reset(); memory_bus.load_rom(rom_file); }
        void run();
    private:
        InterruptController interrupt;
        Joypad joypad;
        PPU ppu;
        Timer timer;
        MemoryBus memory_bus;
        CPU cpu;
        Frontend frontend;
        // APU

        
        std::chrono::steady_clock::time_point last_frame_time;
        std::chrono::steady_clock::time_point last_save_time;

        void reset();
        void on_frame_ready();
};


#endif // EMULATOR_H