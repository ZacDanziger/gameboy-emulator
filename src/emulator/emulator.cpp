#include "emulator.h"

void Emulator::run() {
    while (display.poll_events()) {
        cpu.step();
    }
}

void Emulator::on_frame_ready() {
    display.present(ppu.get_frame());

    auto elapsed = std::chrono::steady_clock::now() - last_frame_time;
    if (elapsed < FRAME_DURATION) {
        std::this_thread::sleep_for(FRAME_DURATION - elapsed);
    }

    last_frame_time = std::chrono::steady_clock::now();
}