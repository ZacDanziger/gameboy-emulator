#ifndef TIMER_H
#define TIMER_H

#include <stdexcept>
#include "../memory/memory_map.h"
#include "../utils/utils.h"

using TickCallback = std::function<void()>;

class Timer {
    friend class TimerLogger;

    public:
        Timer(InterruptCallback i, TickCallback t):
            request_interrupt(i),
            on_tick(t),
            
            divider_internal(0x00AB),
            timer(0x00),
            timer_modulo(0x00),

            previous_high(false),
            overflow_delay(false),
            timer_reload_cycle(false),
            double_speed_mode(false)
        {
            // initialize timer_control, div_bit, and enabled
            write(TAC_REGISTER, 0xF8);
        }

        void tick();

        void write(const Address address, const Byte value);
        Byte read(const Address address) const;

        bool get_double_speed() const { return double_speed_mode; }
        void set_double_speed(const bool ds) { double_speed_mode = ds; }
    private:
        InterruptCallback request_interrupt;
        TickCallback on_tick;

        Word divider_internal;      // SYSCLK (DIV_REGISTER)
        Byte timer;                 // TIMA_REGISTER
        Byte timer_modulo;          // TMA_REGISTER
        Byte timer_control;         // TAC_REGISTER

        Bit div_bit;
        bool enabled;
        bool previous_high;
        bool overflow_delay;
        bool timer_reload_cycle;

        bool double_speed_mode;
};

#endif // TIMER_H