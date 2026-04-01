#include "emulator.h"


void Emulator::run() {
    while (frontend.poll_events()) {
        cpu.step();
    }
}

/**
 * Reset the Emulator and all of its members to their post Boot ROM states
 */
void Emulator::reset() {
    last_frame_time = std::chrono::steady_clock::now();
    last_save_time = std::chrono::steady_clock::now();

    interrupt.reset();
    joypad.reset();
    ppu.reset();
    timer.reset();
    memory_bus.reset();
    cpu.reset();
}


void Emulator::on_frame_ready() {
    frontend.present(ppu.get_frame());

    auto now = std::chrono::steady_clock::now();

    if (now - last_save_time >= AUTOSAVE_INTERVAL) {
        memory_bus.save();
        last_save_time = now;
    }

    auto elapsed = now - last_frame_time;
    if (elapsed < FRAME_DURATION) {
        std::this_thread::sleep_for(FRAME_DURATION - elapsed);
    }

    last_frame_time += FRAME_DURATION;
}