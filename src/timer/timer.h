#ifndef TIMER_H
#define TIMER_H

#include <stdexcept>
#include "../memory/memory_map.h"

class Timer {
    friend class TimerLogger;

    public:
        Timer(InterruptCallback cb):
            request_interrupt(cb),
            divider_internal(0x0000),
            divider(0x00),
            timer(0x00),
            timer_modulo(0x00),
            timer_control(0x00),

            div_bit(7),
            enabled(false),
            previous_high(false),
            overflow_delay(false),
            timer_reload_cycle(false)
        {}

        void tick();

        void write(const Address address, const Byte value);
        Byte read(const Address address) const;

    private:
        InterruptCallback request_interrupt;

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
};

#endif // TIMER_H