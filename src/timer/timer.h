#ifndef TIMER_H
#define TIMER_H

#include "../memory/memory.h"

class Timer {
    public:
        Timer();

        void init(Memory* mem);
        void tick(int m_cycles);

        void switch_speed(bool on_off);

        // writing to DIV resets the entire 16-bit register to 0
        void reset_divider() { divider_clocksum = 0x0000; } 
    private:
        Memory *memory;

        bool double_speed;

        Word divider_clocksum;
        Word divider_update_threshold;
        
        Word timer_clocksum;
        Word timer_update_threshold;
};

#endif // TIMER_H