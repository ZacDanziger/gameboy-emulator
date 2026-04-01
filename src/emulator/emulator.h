#ifndef EMULATOR_H
#define EMULATOR_H

#include "../memory/memory_bus.h"
#include "../timer/timer.h"
#include "../cpu/cpu.h"
#include "../ppu/ppu.h"
#include "../frontend/frontend.h"

#include <thread>
#include <chrono>

constexpr auto FRAME_DURATION = std::chrono::nanoseconds(16742706); // ~59.7 fps
constexpr auto AUTOSAVE_INTERVAL = std::chrono::minutes(5);

class Emulator {
    public:
        Emulator(const std::string& rom_file):
            interrupt(),
            memory_bus(interrupt),
            frontend("GameBoy Color Emulator", [this](Key key, bool pressed){ memory_bus.set_key(key, pressed); }),
            timer(interrupt,
                   [this]{ ppu.update(); }),
            cpu(memory_bus, timer, interrupt),
            ppu(interrupt,
                [this]{ this->on_frame_ready(); },
                [this]{ memory_bus.hdma_tick(); }),
            
            last_frame_time(std::chrono::steady_clock::now()),
            last_save_time(std::chrono::steady_clock::now())
            {
                memory_bus.init(&timer, &ppu);

                memory_bus.load_rom(rom_file);
            }

        void run();
    private:
        InterruptController interrupt;
        MemoryBus memory_bus;
        Frontend frontend;
        Timer timer;
        CPU cpu;
        PPU ppu;
        // APU

        
        std::chrono::steady_clock::time_point last_frame_time;
        std::chrono::steady_clock::time_point last_save_time;
        void on_frame_ready();
};


#endif // EMULATOR_H