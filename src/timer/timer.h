#ifndef TIMER_H
#define TIMER_H

#include "../memory/memory.h"

class Timer {
    public:
        Timer();
        void init(Memory* memory);
        void tick(int m_cycles);
        void switch_speed(bool on_off);
    private:
        Memory *_memory;
        bool double_speed;
        Word divider_clocksum;
        Word timer_clocksum;

        const int CLOCK_SPEED = 4194304; // Hz
        const int DIVIDER_SPEED = 16384; // Hz
};

#endif // TIMER_H