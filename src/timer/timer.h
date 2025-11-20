#ifndef TIMER_H
#define TIMER_H

#include "../memory/memory.h"

class Timer {
    public:
        Timer();

        void init(Memory* mem);
        void tick(int m_cycles);

        // writing to DIV resets the entire 16-bit register to 0
        void reset_divider() { divider_clock_cycles = 0x0000; } 
    private:
        Memory *memory;
        
        uint16_t divider_clock_cycles;
        uint16_t divider_update_threshold;

        uint16_t timer_clock_cycles;
        uint16_t timer_update_threshold;

        void request_timer_interrupt() { memory->write(IF_REGISTER, (memory->read(IF_REGISTER) | 0b100)); }
};

#endif // TIMER_H