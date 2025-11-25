#ifndef EMULATOR_H
#define EMULATOR_H

#include "../memory/mmu.h"
#include "../timer/timer.h"
#include "../cpu/cpu.h"

class Emulator {
    public:
        Emulator(): mmu(), timer([this](Interrupt i){ mmu.request_interrupt(i); }), cpu() {}

        void run();
    private:
        MMU mmu;
        Timer timer;
        CPU cpu;
        // PPU
        // APU
};


#endif // EMULATOR_H