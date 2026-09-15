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
        
        if (!frame_complete) {
            continue;
        }

        frame_complete = false;

        // if window was closed, exit emulator
        if (!frontend.poll_events()) {
            break;
        }

        // If pause was pressed, toggle pause
        if (frontend.take_pause_request()) {
            switch(state) {
                case EmulatorState::Idle:
                    break;
                case EmulatorState::Running:
                    state = EmulatorState::Paused;
                    break;
                case EmulatorState::Paused:
                    state = EmulatorState::Running;
                    break;
            }
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