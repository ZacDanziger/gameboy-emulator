#include "timer.h"

Timer::Timer() {
    memory = nullptr;
    divider_clock_cycles = 0x0000;
    divider_update_threshold = 64;  // 16384 Hz update frequency (m-cycles)
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
 * Keeps count of m-cycles, updates DIV and TIMA registers accordingly
 *     Will set the timer interrupt request flag when TIMA overflows 
 * 
 * @param m_cycles the number of m-cycles that have passed since the last tick
 */
void Timer::tick(int m_cycles) {
    divider_clock_cycles += m_cycles;

    if (divider_clock_cycles >= divider_update_threshold) {
        memory->write(DIV_REGISTER, (memory->read(DIV_REGISTER) + 1));
        divider_clock_cycles -= divider_update_threshold;
    }


    Byte timer_control = memory->read(TAC_REGISTER);

    // check timer enable bit
    if (((timer_control >> 2) & 0x1) == 0x0) {
        return;
    }

    timer_clock_cycles += m_cycles;

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

    while (timer_clock_cycles >= timer_update_threshold) {
        Byte tima = memory->read(TIMA_REGISTER);
        memory->write(TIMA_REGISTER, tima + 1);

        // If an overflow occurs
        if (tima == 0xFF) {
            // Set the timer interrupt request flag
            request_timer_interrupt();
            // Reset timer counter to timer modulo
            memory->write(TIMA_REGISTER, memory->read(TMA_REGISTER));
        }
        
        timer_clock_cycles -= timer_update_threshold;
    }
}

