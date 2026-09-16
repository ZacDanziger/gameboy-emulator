#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>
#include <chrono>

#include "../core/gameboy.h"
#include "../frontend/frontend.h"

constexpr auto AUTOSAVE_INTERVAL = std::chrono::minutes(5);
const std::string EMULATOR_NAME = "GameBoy Color Emulator";

enum class AppState {
    Idle,
    Running,
    Paused
};

class Emulator {
    public:
        Emulator() : 
            gameboy(),
            frontend(EMULATOR_NAME),

            state(AppState::Idle),

            last_frame_time(std::chrono::steady_clock::now()),
            last_save_time(std::chrono::steady_clock::now()),

            frame_count(0),
            fps_timer(std::chrono::steady_clock::now())
        {}

        void run();
    private:
        GameBoy gameboy;
        Frontend frontend;

        AppState state;

        std::chrono::steady_clock::time_point last_frame_time;
        std::chrono::steady_clock::time_point last_save_time;

        int frame_count;
        std::chrono::steady_clock::time_point fps_timer;

        void update_fps();
        void autosave();
};

#endif  // EMULATOR_H