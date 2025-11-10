#ifndef EMULATOR_H
#define EMULATOR_H

#include "../cpu/cpu.h"

class Emulator {
    public:
        Emulator(): _cpu() {}

        void run();
    private:
        CPU _cpu;
        // PPU
        // APU
};


#endif // EMULATOR_H