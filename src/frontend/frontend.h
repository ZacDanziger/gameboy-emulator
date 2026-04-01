#ifndef FRONTEND_H
#define FRONTEND_H

#include <SDL3/SDL.h>
#include <string>
#include "../types.h"

using KeyCallback = std::function<void(Key, bool)>;

constexpr int DISPLAY_SCALE = 4;    // initial scaling factor of screen

/**
 * Handles all SDL3 code, including window & screen, keyboard input, and audio
 */
class Frontend {
    public:
        Frontend(const std::string& title, KeyCallback k);
        ~Frontend();

        void present(const std::array<RGBA32, SCREEN_WIDTH * SCREEN_HEIGHT>& frame);
        bool poll_events(); 
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;

        KeyCallback on_key_event;

        void teardown();
};

#endif  // FRONTEND_H