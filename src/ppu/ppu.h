#ifndef PPU_H
#define PPU_H

#include "../timer/timer.h"
#include "../memory/memory.h"
// #include <SDL3/SDL.h>

class PPU {
    public:
        PPU();

        void init(Timer* tim, Memory* mem);
    private:
        Memory *memory;
        Timer *timer;
};

#endif // PPU_H