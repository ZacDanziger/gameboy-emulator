#ifndef EMULATOR_H
#define EMULATOR_H

#include "../memory/mmu.h"
#include "../timer/timer.h"
#include "../cpu/cpu.h"
#include "../ppu/ppu.h"
#include "../display/display.h"

#include <thread>
#include <chrono>

#include "../utils/logger.h"

constexpr auto FRAME_DURATION = std::chrono::nanoseconds(16742706); // ~59.7 fps

class Emulator {
    public:
        Emulator(const std::string& rom_file):
            mmu(),
            display("GameBoy v1", [this](Key key, bool pressed){ mmu.set_key(key, pressed); }),
            timer([this](Interrupt i){ mmu.request_interrupt(i); },
                   [this]{ ppu.update(); }),
            cpu(),
            ppu([this](Interrupt i){ mmu.request_interrupt(i); },
                [this]{ this->on_frame_ready(); },
                [this]{ mmu.hdma_tick(); }),

            last_frame_time(std::chrono::steady_clock::now())
            {
                mmu.init(&timer, &ppu);
                cpu.init(&timer, &mmu);

                mmu.load_rom(rom_file);
            }

        void run();
    private:
        MMU mmu;
        Display display;
        Timer timer;
        CPU cpu;
        PPU ppu;
        // APU
        
        std::chrono::steady_clock::time_point last_frame_time;
        void on_frame_ready();
};


#endif // EMULATOR_H