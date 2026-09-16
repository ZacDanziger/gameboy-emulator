#include "emulator.h"

void Emulator::run() {
    while(state == AppState::Running) {
        gameboy.run_until_frame();
        frontend.present(gameboy.flush_frame(), gameboy.flush_audio());
    }
}


void Emulator::update_fps() {
    frame_count++;
    if (frame_count % 60 == 0) {
        auto now = std::chrono::steady_clock::now();
        double fps = 60.0 / std::chrono::duration<double>(now - fps_timer).count();
        frontend.set_fps(fps);
        fps_timer = now;
        frame_count = 0;
    }
}


void Emulator::autosave() {
    auto now = std::chrono::steady_clock::now();
    if (now - last_save_time >= AUTOSAVE_INTERVAL) {
        gameboy.save();
        last_save_time = now;
    }
}