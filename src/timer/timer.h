#ifndef TIMER_H
#define TIMER_H

#include "../memory/memory.h"

class Timer {
    public:
        Timer();
        void init(Memory* mem);
        void tick(int m_cycles);
        void switch_speed(bool on_off);
    private:
        Memory *memory;

        bool double_speed;

        Word divider_clocksum;
        Word timer_clocksum;
        
        Word divider_update_threshold;
        Word timer_update_threshold;
};

#endif // TIMER_H