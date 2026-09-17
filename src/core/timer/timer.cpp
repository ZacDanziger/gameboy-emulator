#include "timer.h"

/**
 * Reset the Timer to its post Boot ROM state
 */
void Timer::reset() {
    divider_internal = 0x00AB;
    timer = 0x00;
    timer_modulo = 0x00;

    div_apu_bit = Bit::Bit10;
    previous_high = false;
    apu_previous_high = false;
    overflow_delay = false;
    timer_reload_cycle = false;
    double_speed_mode = false;

    write(TAC_REGISTER, 0xF8);
}


/**
 * Move the timer forward one m-cycle
 */
void Timer::tick() {
    timer_reload_cycle  = false;

    if (overflow_delay) {
        overflow_delay = false;
        timer = timer_modulo;
        timer_reload_cycle = true;

        interrupt.request_interrupt(Interrupt::Timer);
    }

    divider_internal += 1;

    // check timer enable bit
    bool set = is_set(divider_internal, div_bit);
    bool high = (enabled && set);

    // drive the DIV-APU tick
    bool apu_high = is_set(divider_internal, div_apu_bit);
    if (!apu_high && apu_previous_high) {
        div_apu_event = true;
    }
    apu_previous_high = apu_high;

    // check falling edge
    if (!high && previous_high) {
        timer += 1;

        // If an overflow occurs
        if (timer == 0x00) {
            overflow_delay = true;
        }
    }

    previous_high = high;
}


/**
 * Read the value from DIV, TIMA, TMA, or TAC registers
 * 
 * @param address the address of one of the registers that Timer controls
 * @return the value in the requested register
 */
Byte Timer::read(const Address address) const {
    switch (address) {
    case DIV_REGISTER:
        return ((divider_internal >> 6) & 0xFF);
    case TIMA_REGISTER:
        return timer;
    case TMA_REGISTER:
        return timer_modulo;
    case TAC_REGISTER:
        return timer_control;
    default:
        throw std::runtime_error("Timer read called on wrong address");
    }
}


/**
 * Write to DIV, TIMA, TMA, or TAC registers
 * 
 * @param address the address of one of the registers that Timer is responsible for
 * @param value the new value to be written to the register
 */
void Timer::write(const Address address, const Byte value) {
    switch(address) {
    case DIV_REGISTER:
        // any write to DIV resets it to 0
        divider_internal = 0x0000;
        break;
    case TIMA_REGISTER:
        if (!timer_reload_cycle) {
            timer = value;
        }
        // writing to timer the cycle it overflows prevents timer interrupt flag going off
        // https://gbdev.io/pandocs/Timer_Obscure_Behaviour.html#timer-overflow-behavior
        if (overflow_delay) {
            overflow_delay = false;
        }
        break;
    case TMA_REGISTER:
        if (timer_reload_cycle) {
            timer = value;
        }
        timer_modulo = value;
        break;
    case TAC_REGISTER:
        timer_control = value;
        enabled = is_set(timer_control, Bit::Bit2);

        /* Lowest 2 bits of TAC register set speed
        * 00 -> divider bit 9  (256 m-cycles)
        * 01 -> divider bit 3  (4 m-cycles)
        * 10 -> divider bit 5  (16 m-cycles)
        * 11 -> divider bit 7  (64 m-cycles)
        * 
        * since we're working in m-cycles and not t-states, each of the above bits -= 2
        */
        switch(timer_control & 0x3) {
        case 0x00:                           //-----------|-------------
            div_bit = Bit::Bit7;             // 4096 Hz   | 8192 Hz 
            break;                           //           | 
        case 0x01:                           //           |
            div_bit = Bit::Bit1;             // 262144 Hz | 524288 Hz
            break;                           //           |
        case 0x02:                           //           |
            div_bit = Bit::Bit3;             // 65536 Hz  | 131072 Hz
            break;                           //           |
        case 0x03:                           //           |
            div_bit = Bit::Bit5;             // 16384 Hz  | 32768 Hz
            break;
        }
        break;
    default:
        throw std::runtime_error("Timer write called on wrong address");
    }
}


bool Timer::take_div_apu_event() {
    bool event = div_apu_event;
    div_apu_event = false;
    return event;
}

