#include "timer.h"

Timer::Timer() {
    memory = nullptr;
    double_speed = false;
    divider_clocksum = 0;
    timer_clocksum = 0;
    divider_update_threshold = 64;    // 16384 Hz update frequency (m-cycles)
}

/**
 * Initialize timer's memory pointer
 * 
 * @param mem a valid pointer to a memory object
 */
void Timer::init(Memory* mem) {
    if (mem == nullptr) {
        throw std::runtime_error("Cannot initialize with nullptr");
    }
    memory = mem;
}


/**
 * Enables and Disables CGB's double speed mode
 * 
 * @param on_off true if switching to double speed mode, false if switching from double speed mode
 */
void Timer::switch_speed(bool on_off) {
    double_speed = on_off;
    if (double_speed) {
        divider_update_threshold = 32;    // 32768 Hz update frequency (m-cycles)
    } else {
        divider_update_threshold = 64;
    }
}


/**
 * Keeps count of m-cycles, updates DIV and TIMA registers accordingly
 *     Will set the timer interrupt request flag when TIMA overflows 
 * 
 * @param m_cycles the number of m-cycles that have passed since the last tick
 */
void Timer::tick(int m_cycles) {
    divider_clocksum += m_cycles;

    if (divider_clocksum >= divider_update_threshold) {
        divider_clocksum -= divider_update_threshold;
        memory->write(DIV_REGISTER, (memory->read(DIV_REGISTER) + 1));
    }

    Byte timer_control = memory->read(TAC_REGISTER);
    // check timer enable bit
    if (((timer_control >> 2) & 0x1) == 0x0) {
        return;
    }

    timer_clocksum += m_cycles;

    /* Lowest 2 bits of TAC register set speed
     * 00 -> divider bit 8  (256 m-cycles)
     * 01 -> divider bit 2  (4 m-cycles)
     * 10 -> divider bit 4  (16 m-cycles)
     * 11 -> divider bit 6  (64 m-cycles)
    */

    switch(timer_control & 0x3) {        // NORMAL    | DOUBLE SPEED
    case 0x00:                           //-----------|-------------
        timer_update_threshold = 256;    // 4096 Hz   | 8192 Hz 
        break;                           //           | 
    case 0x01:                           //           |
        timer_update_threshold = 4;      // 262144 Hz | 524288 Hz
        break;                           //           |
    case 0x02:                           //           |
        timer_update_threshold = 16;     // 65536 Hz  | 131072 Hz
        break;                           //           |
    case 0x03:                           //           |
        timer_update_threshold = 64;     // 16384 Hz  | 32768 Hz
        break;
    }

    if (double_speed) {
        timer_update_threshold /= 2;
    }

    // while instead of if because it's possible that timer_clocksum / timer_update_threshold >= 2
    while (timer_clocksum >= timer_update_threshold) {
        Byte tima = memory->read(TIMA_REGISTER);
        memory->write(TIMA_REGISTER, tima + 1);

        // If an overflow occurs
        if (tima == 0xFF) {
            // Set the timer interrupt request flag
            memory->write(IF_REGISTER, (memory->read(IF_REGISTER) | 0x4));
            // Reset timer counter to timer modulo
            memory->write(TIMA_REGISTER, memory->read(TMA_REGISTER));
        }
        timer_clocksum -= timer_update_threshold;
    }
}

