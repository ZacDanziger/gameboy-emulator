#include "emulator.h"

Emulator::Emulator() :
    _memory(nullptr),
    _timer(nullptr),
    _cpu(nullptr)
{}


void Emulator::run() {
    // Initialize here

    while (!_cpu->is_halted()) {
        _cpu->step();
    }
}