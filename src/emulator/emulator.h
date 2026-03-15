#ifndef EMULATOR_H
#define EMULATOR_H

#include "../memory/mmu.h"
#include "../timer/timer.h"
#include "../cpu/cpu.h"
#include "../ppu/ppu.h"
#include "../display/display.h"

#include "../utils/logger.h"

#include <thread>
#include <chrono>

constexpr auto FRAME_DURATION = std::chrono::nanoseconds(16742706); // ~59.7 fps

const bool LOGGING = false;

class Emulator {
    public:
        Emulator(const std::string& rom_file):
            display("GameBoy v1"),
            mmu(),
            timer([this](Interrupt i){ mmu.request_interrupt(i); },
                   [this]{ ppu.update(); }),
            ppu([this](Interrupt i){ mmu.request_interrupt(i); },
                [this]{ this->on_frame_ready(); }),
            cpu(),

            last_frame_time(std::chrono::steady_clock::now()),

            cpu_logger(&cpu)
            {
                mmu.init(&timer, &ppu);
                cpu.init(&timer, &mmu);

                mmu.load_rom(rom_file);

                if (LOGGING) {
                    cpu_logger.init_ofstream("/Users/zacdanziger/Documents/Personal/Coding/gameboy-emulator/build/cpu_log.txt");
                }
            }

        void run();
    private:
        Display display;
        MMU mmu;
        Timer timer;
        CPU cpu;
        PPU ppu;
        // APU

        CPULogger cpu_logger;
        
        std::chrono::steady_clock::time_point last_frame_time;
        void on_frame_ready();
};


#endif // EMULATOR_H