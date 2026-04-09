#ifndef TIMER_H
#define TIMER_H

#include <stdexcept>

#include "../memory_map.h"
#include "../types.h"
#include "../utils/utils.h"
#include "../interrupt/interrupt_controller.h"

using TickCallback = std::function<void()>;

/**
 * TODO: adjust div-apu bit to bit 5 in double speed mode
 * TODO: tick APU
 */
class Timer {
    public:
        Timer(InterruptController& i, TickCallback t1, TickCallback t2):
            interrupt(i),
            on_tick(t1),
            div_apu_tick(t2),
            
            divider_internal(0x00AB),
            timer(0x00),
            timer_modulo(0x00),

            div_apu_bit(Bit::Bit4),
            previous_high(false),
            apu_previous_high(false),
            overflow_delay(false),
            timer_reload_cycle(false),
            double_speed_mode(false)
        {
            // initialize timer_control, div_bit, and enabled
            write(TAC_REGISTER, 0xF8);
        }

        void reset();

        void tick();

        Byte read(const Address address) const;
        void write(const Address address, const Byte value);

        bool get_double_speed() const { return double_speed_mode; }
        void set_double_speed(const bool ds) { double_speed_mode = ds; }
    private:
        InterruptController& interrupt;
        TickCallback on_tick;
        TickCallback div_apu_tick;

        Word divider_internal;      // SYSCLK (DIV_REGISTER)
        Byte timer;                 // TIMA_REGISTER
        Byte timer_modulo;          // TMA_REGISTER
        Byte timer_control;         // TAC_REGISTER

        Bit div_bit;
        Bit div_apu_bit;
        bool enabled;
        bool previous_high;
        bool apu_previous_high;
        bool overflow_delay;
        bool timer_reload_cycle;

        bool double_speed_mode;
};

#endif // TIMER_H