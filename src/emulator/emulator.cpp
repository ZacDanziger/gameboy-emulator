#include "emulator.h"

Emulator::Emulator() {
    _timer.init(&_memory);
    _cpu.init(&_timer, &_memory);
}


void Emulator::run() {
    // Initialize here

    while (!_cpu.is_halted()) {
        _cpu.step();
    }
}