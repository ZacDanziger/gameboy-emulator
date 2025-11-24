#ifndef TIMER_H
#define TIMER_H

#include "../memory/memory.h"

class Timer {
    friend class TimerLogger;

    public:
        Timer();

        void init(Memory* mem);
        // void cycle(int m_cycles);

        void tick();
        void write(const Address address, const Byte value);
        Byte read(const Address address) const;

    private:
        Memory *memory;
        
        uint16_t divider_internal;  // SYSCLK
        Byte divider;               // DIV_REGISTER
        Byte timer;                 // TIMA_REGISTER
        Byte timer_modulo;          // TMA_REGISTER
        Byte timer_control;         // TAC_REGISTER

        uint8_t div_bit;
        bool enabled;
        bool previous_high;
        bool overflow_delay;
        bool timer_reload_cycle;


        void request_timer_interrupt() { memory->write(IF_REGISTER, (memory->read(IF_REGISTER) | 0b100)); }
};

#endif // TIMER_H