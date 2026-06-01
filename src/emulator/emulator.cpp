#include "emulator.h"

void Emulator::load(const std::string& rom_file) {
    reset();
    memory_bus.load_rom(rom_file);
    frontend.set_title(rom_file);
}


void Emulator::run() {
    while (true) {
        cpu.step();

        if (joypad.is_save_requested()) {
            memory_bus.save();
            joypad.save_acknowledged();
        }

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
    auto now = std::chrono::steady_clock::now();
    last_frame_time = now;
    last_save_time = now;

    frame_count = 0;
    fps_timer = now;

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

    if (memory_bus.is_dma_active()) {
        return;
    }

    frontend.present(ppu.get_frame(), apu.flush_audio_buffer());
    autosave();
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
        memory_bus.save();
        last_save_time = now;
    }
}