#include "emulator.h"

void Emulator::load_rom(const std::string& rom_file) {
    gameboy.load(rom_file);
    frontend.set_title(rom_file);
    state = AppState::Running;
}


void Emulator::run() {
    while(true) {
        if (state == AppState::Running) {
            gameboy.run_until_frame();
            frontend.present(gameboy.flush_frame(), gameboy.flush_audio());
        }

        // if window was closed, exit emulator
        if (!frontend.poll_events()) {
            break;
        }

        // If pause was pressed, toggle pause
        if (frontend.take_pause_request()) {
            switch(state) {
                case AppState::Idle:
                    break;
                case AppState::Running:
                    state = AppState::Paused;
                    break;
                case AppState::Paused:
                    state = AppState::Running;
                    break;
            }
        }
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