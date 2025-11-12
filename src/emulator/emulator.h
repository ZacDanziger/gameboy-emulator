#ifndef EMULATOR_H
#define EMULATOR_H

#include "../cpu/cpu.h"

class Emulator {
    public:
        Emulator();

        void run();
    private:
        Memory *_memory;
        Timer *_timer;
        CPU *_cpu;
        // PPU
        // APU
};


#endif // EMULATOR_H