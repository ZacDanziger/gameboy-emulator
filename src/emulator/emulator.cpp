#include "emulator.h"


void Emulator::run() {
    while (display.poll_events()) {
        int old_cycle_count = timer.get_cycle_count();
        cpu.step();
        int cycles_elapsed = timer.get_cycle_count() - old_cycle_count;

        // for (int i = 0; i < cycles_elapsed; i++) {
        //     ppu.update();
        // }
    }
}

void Emulator::on_frame_ready() {
    display.present(ppu.get_frame());

    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - last_frame_time;
    if (elapsed < FRAME_DURATION) {
        std::this_thread::sleep_for(FRAME_DURATION - elapsed);
    }

    last_frame_time = now;
}