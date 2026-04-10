#ifndef FRONTEND_H
#define FRONTEND_H

#include <SDL3/SDL.h>
#include <string>
#include <array>

#include <chrono>
#include <iostream>

#include "../types.h"
#include "../joypad/joypad.h"

constexpr int DISPLAY_SCALE = 4;    // initial scaling factor of screen

constexpr int TARGET_QUEUE_BYTES = 44100 * 2 * sizeof(float) * 0.1f;

/**
 * Handles all SDL3 code, including window & screen, keyboard input, and audio
 */
class Frontend {
    public:
        Frontend(const std::string& title, Joypad& j) :
            window(nullptr),
            renderer(nullptr),
            texture(nullptr),

            audio_stream(nullptr),

            joypad(j)
        {
            init(title);
        }

        ~Frontend();

        void present(const std::array<RGBA32, SCREEN_WIDTH * SCREEN_HEIGHT>& frame, const std::vector<float>& audio_buffer);
        bool poll_events(); 
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;

        SDL_AudioStream* audio_stream;

        Joypad& joypad;
        void init(const std::string& title);
        void teardown();
};

#endif  // FRONTEND_H