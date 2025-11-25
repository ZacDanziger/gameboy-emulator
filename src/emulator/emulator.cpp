#include "emulator.h"


void Emulator::run() {
    // Initialize here

    while (!cpu.is_halted()) {
        cpu.step();
    }
}