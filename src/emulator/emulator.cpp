#include "emulator.h"

void Emulator::run() {
    // Initialize here

    while (!_cpu.is_halted()) {
        _cpu.step();
    }
}