#include "emulator.h"

Emulator::Emulator() {
    timer.init(&memory);
    cpu.init(&timer, &memory);
}


void Emulator::run() {
    // Initialize here

    while (!cpu.is_halted()) {
        cpu.step();
    }
}