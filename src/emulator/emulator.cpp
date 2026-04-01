#include "emulator.h"


void Emulator::run() {
    while (frontend.poll_events()) {
        cpu.step();
    }
}

void Emulator::on_frame_ready() {
    frontend.present(ppu.get_frame());

    auto now = std::chrono::steady_clock::now();

    if (now - last_save_time >= AUTOSAVE_INTERVAL) {
        mmu.save();
        last_save_time = now;
    }

    auto elapsed = now - last_frame_time;
    if (elapsed < FRAME_DURATION) {
        std::this_thread::sleep_for(FRAME_DURATION - elapsed);
    }

    last_frame_time += FRAME_DURATION;
}