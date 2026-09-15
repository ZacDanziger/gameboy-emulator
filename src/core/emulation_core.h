#ifndef EMULATOR_H
#define EMULATOR_H

#include <chrono>

#include "apu/apu.h"
#include "cpu/cpu.h"
#include "interrupt/interrupt_controller.h"
#include "joypad/joypad.h"
#include "memory/memory_bus.h"
#include "ppu/ppu.h"
#include "timer/timer.h"

#include "../frontend/frontend.h"

constexpr auto AUTOSAVE_INTERVAL = std::chrono::minutes(5);
const std::string EMULATOR_NAME = "GameBoy Color Emulator";

enum class EmulatorState {
    Idle,
    Running,
    Paused
};

class Emulator {
    public:
        Emulator():
            state(EmulatorState::Idle),

            interrupt(),
            joypad(interrupt),
            ppu(interrupt, [this]{ this->on_frame_ready(); }, [this]{ memory_bus.hdma_tick(); }),
            apu(),
            timer(interrupt, [this]{ ppu.update(); apu.update(); }, [this]{ apu.frame_sequencer(); }),
            memory_bus(interrupt, joypad, timer, ppu, apu),
            cpu(interrupt, joypad, timer, memory_bus),
            frontend(EMULATOR_NAME, joypad),
            
            frame_complete(false),

            rom_dialog_opened(false),
            rom_dialog_was_opened(false),
            state_before_dialog(EmulatorState::Idle),

            last_frame_time(std::chrono::steady_clock::now()),
            last_save_time(std::chrono::steady_clock::now()),

            frame_count(0),
            fps_timer(std::chrono::steady_clock::now())
        {}
        
        void load(const std::string& rom_file);
        void run();
    private:
        EmulatorState state;

        InterruptController interrupt;
        Joypad joypad;
        PPU ppu;
        APU apu;
        Timer timer;
        MemoryBus memory_bus;
        CPU cpu;
        Frontend frontend;

        bool frame_complete;

        bool rom_dialog_opened;
        bool rom_dialog_was_opened;
        EmulatorState state_before_dialog;

        std::chrono::steady_clock::time_point last_frame_time;
        std::chrono::steady_clock::time_point last_save_time;

        int frame_count;
        std::chrono::steady_clock::time_point fps_timer;

        void reset();
        void on_frame_ready();

        void update_fps();
        void autosave();
};


#endif // EMULATOR_H