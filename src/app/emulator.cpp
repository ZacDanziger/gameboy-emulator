#include "emulator.h"

void Emulator::load_rom(const std::string& rom_file) {
    gameboy.load(rom_file);
    frontend.set_title(rom_file);

    state = AppState::Running;

    last_frame_time = std::chrono::steady_clock::now();
    last_save_time = std::chrono::steady_clock::now();

    frame_count = 0;
    fps_timer = std::chrono::steady_clock::now();

    rom_dialog_was_open = false;
    state_before_dialog = AppState::Idle;
}


void Emulator::run() {
    while(true) {
        if (state == AppState::Running) {
            // Input
            gameboy.set_button_state(frontend.get_button_state());
            
            // Update
            gameboy.run_until_frame();
            autosave();
            update_fps();


            // Render
            frontend.present(gameboy.flush_frame(), gameboy.flush_audio());


        } else {
            frontend.delay_ms(16);  // ~60 fps
        }

        frontend.poll_events();

        // If the window was closed, stop the loop
        if (frontend.should_quit()) {
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

        // Check ROM dialog state
        bool rom_dialog_open = frontend.is_rom_dialog_open();


        // Rising Edge - store old state, pause emulator
        if (!rom_dialog_was_open && rom_dialog_open) {
            state_before_dialog = state;
            state = AppState::Paused;
        }


        // Falling edge - load ROM if one was selected, otherwise return to old state
        if (rom_dialog_was_open && !rom_dialog_open) {
            if (auto rom = frontend.take_pending_rom()) {
                load_rom(*rom);
            } else {
                state = state_before_dialog;
            }
        }

        rom_dialog_was_open = rom_dialog_open;
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