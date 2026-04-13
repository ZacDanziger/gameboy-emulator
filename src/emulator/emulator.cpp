#include "emulator.h"


void Emulator::run() {
    while (true) {
        cpu.step();

        if (frame_complete) {
            frame_complete = false;
            if (!frontend.poll_events()) {
                break;
            }
        }
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
    apu.reset();
    timer.reset();
    memory_bus.reset();
    cpu.reset();

}


void Emulator::on_frame_ready() {
    frame_complete = true;

    update_fps();

    if (dma_active) {
        return;
    }

    frontend.present(ppu.get_frame(), apu.flush_audio_buffer());

    autosave();
}


void Emulator::update_fps() {
    static int frame_count = 0;
    static auto fps_timer = std::chrono::steady_clock::now();
    frame_count++;
    if (frame_count % 60 == 0) {
        auto now = std::chrono::steady_clock::now();
        double fps = 60.0 / std::chrono::duration<double>(now - fps_timer).count();
        frontend.set_fps(fps);
        fps_timer = now;
    }
}


void Emulator::autosave() {
    auto now = std::chrono::steady_clock::now();
    if (now - last_save_time >= AUTOSAVE_INTERVAL) {
        memory_bus.save();
        last_save_time = now;
    }
}