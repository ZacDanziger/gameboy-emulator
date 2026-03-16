#include "emulator.h"

void Emulator::run() {
    while (display.poll_events()) {
        cpu.step();
    }
}

void Emulator::on_frame_ready() {
    display.present(ppu.get_frame());

    auto now = std::chrono::steady_clock::now();
    if (auto elapsed = now - last_frame_time; elapsed < FRAME_DURATION) {
        std::this_thread::sleep_for(FRAME_DURATION - elapsed);
    }

    last_frame_time = now;
}