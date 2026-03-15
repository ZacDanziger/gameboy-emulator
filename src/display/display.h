#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL3/SDL.h>
#include <string>
#include "../types.h"

constexpr int DISPLAY_SCALE = 4;    // initial scaling factor of screen

class Display {
    public:
        Display(const std::string& title);
        ~Display();

        void present(const std::array<RGBA32, SCREEN_WIDTH * SCREEN_HEIGHT>& frame);
        bool poll_events(); 
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;

        void teardown();
};

#endif  // DISPLAY_H