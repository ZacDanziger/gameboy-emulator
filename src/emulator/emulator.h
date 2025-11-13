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
        Memory _memory;
        Timer _timer;
        CPU _cpu;
        // PPU
        // APU
};


#endif // EMULATOR_H