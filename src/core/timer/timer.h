#ifndef TIMER_H
#define TIMER_H

#include <stdexcept>

#include "../types.h"
#include "../memory_map.h"
#include "../utils/bit_utils.h"
#include "../interrupt/interrupt_controller.h"

class Timer {
    public:
        Timer(InterruptController& i):
            interrupt(i),
            
            divider_internal(0x00AB),
            timer(0x00),
            timer_modulo(0x00),

            div_apu_bit(Bit::Bit10),
            previous_high(false),
            apu_previous_high(false),
            overflow_delay(false),
            timer_reload_cycle(false),

            div_apu_event(false)
        {
            // initialize timer_control, div_bit, and enabled
            write(TAC_REGISTER, 0xF8);
        }

        void reset();

        void tick();

        Byte read(const Address address) const;
        void write(const Address address, const Byte value);

        bool take_div_apu_event();

        void set_div_apu_bit(const bool is_double_speed) { div_apu_bit = is_double_speed ? Bit::Bit11 : Bit::Bit10; }
    private:
        InterruptController& interrupt;

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

        bool div_apu_event; 
};

#endif // TIMER_H