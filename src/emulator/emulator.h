#ifndef EMULATOR_H
#define EMULATOR_H

#include "../memory/memory.h"
#include "../timer/timer.h"
#include "../cpu/cpu.h"
#include "../ppu/ppu.h"

class Emulator {
    public:
        Emulator();

        void run();
    private:
        Memory memory;
        Timer timer;
        CPU cpu;
        // PPU
        // APU
};


#endif // EMULATOR_H