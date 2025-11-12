#include "timer.h"

Timer::Timer() :
    _memory(nullptr),
    double_speed(false),
    divider_clocksum(0),
    timer_clocksum(0)
{}

void Timer::init(Memory* memory) {
    _memory = memory;
    double_speed = false;
}


void Timer::switch_speed(bool on_off) {
    double_speed = on_off;
}


void Timer::tick(int m_cycles) {
    divider_clocksum += m_cycles;
    // update every 256 m-cycles if normal speed, or 128 m_cycles if double speed
    int div_update_threshold = (CLOCK_SPEED / (DIVIDER_SPEED + (double_speed ? DIVIDER_SPEED : 0)));  

    if (divider_clocksum >= div_update_threshold) {
        divider_clocksum -= div_update_threshold;
        _memory->write(DIV_REGISTER, (_memory->read(DIV_REGISTER) + 1));    // increment DIV_REGISTER
    }

    Byte timer_control = _memory->read(TAC_REGISTER);
    // check timer enable bit
    if (((timer_control >> 2) & 0x1) == 0x0) {
        return;
    }

    timer_clocksum += (4 * m_cycles);   // t-states

    /* Lowest 2 bits of TAC register set speed
     * 00 -> increment every 256 m-cycles
     * 01 -> increment every 4 m-cycles
     * 10 -> increment every 16 m-cycles
     * 11 -> increment every 64 m-cycles
    */

    int timer_frequency = 4096;     // Hz

    switch(timer_control & 0x3) {
    case 0x01:
        timer_frequency = 262144;   // Hz
        break;
    case 0x02:
        timer_frequency = 65536;    // Hz
        break;
    case 0x03:
        timer_frequency = 16384;    // Hz
        break;
    }

    int timer_update_threshold = CLOCK_SPEED / timer_frequency; // do I need to account for double speed mode here?

    if (timer_clocksum >= timer_update_threshold) {
        _memory->write(TIMA_REGISTER, (_memory->read(TIMA_REGISTER) + 1));

        // If an overflow occurs
        if (_memory->read(TIMA_REGISTER) == 0x00) {
            // Set the timer interrupt request flag
            _memory->write(IF_REGISTER, (_memory->read(IF_REGISTER) | 0x4));
            // Reset timer counter to timer modulo
            _memory->write(TIMA_REGISTER, _memory->read(TMA_REGISTER));
        }

        timer_clocksum -= timer_update_threshold;
    }
}

