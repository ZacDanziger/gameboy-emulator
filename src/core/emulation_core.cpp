#include "emulation_core.h"

void EmulationCore::load(const std::string& rom_file) {
    reset();
    memory_bus.load_rom(rom_file);
}


void EmulationCore::run_until_frame() {
    while (!ppu.is_frame_ready()) {
        cpu.step();

        if (joypad.take_save_request()) {
            memory_bus.save();
        }
    }
}


/**
 * Reset the EmulationCore and all of its members to their post Boot ROM states
 */
void EmulationCore::reset() {
    frame_complete = false;

    interrupt.reset();
    joypad.reset();
    ppu.reset();
    apu.reset();
    timer.reset();
    memory_bus.reset();
    cpu.reset();
}