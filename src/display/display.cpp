#include "display.h"

Display::Display(const std::string& title, KeyCallback k) {
    on_key_event = k;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(std::string("Failed to initialize SDL ") + SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer(
        title.c_str(),
        DISPLAY_SCALE * SCREEN_WIDTH,
        DISPLAY_SCALE * SCREEN_HEIGHT, 
        SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        teardown();
        throw std::runtime_error(std::string("Failed to create window and renderer ") + SDL_GetError());
    }

    if (!SDL_SetRenderLogicalPresentation(renderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        teardown();
        throw std::runtime_error(std::string("Failed to set renderer logical presentation ") + SDL_GetError());
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!texture) {
        teardown();
        throw std::runtime_error(std::string("Failed to create texture ") + SDL_GetError());
    }
}


Display::~Display() {
    teardown();
}

void Display::present(const std::array<RGBA32, SCREEN_WIDTH * SCREEN_HEIGHT>& frame) {
    SDL_UpdateTexture(texture, nullptr, frame.data(), SCREEN_WIDTH * sizeof(RGBA32));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}


bool Display::poll_events() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            return false;
        }

        if ((e.type == SDL_EVENT_KEY_DOWN) || (e.type == SDL_EVENT_KEY_UP)) {
            bool pressed = (e.type == SDL_EVENT_KEY_DOWN);

            switch(e.key.scancode) {
            case SDL_SCANCODE_Z:
                on_key_event(Key::A, pressed);
                break;
            case SDL_SCANCODE_X:
                on_key_event(Key::B, pressed);
                break;
            case SDL_SCANCODE_RSHIFT:
                on_key_event(Key::Select, pressed);
                break;
            case SDL_SCANCODE_RETURN:
                on_key_event(Key::Start, pressed);
                break;
            case SDL_SCANCODE_RIGHT:
                on_key_event(Key::Right, pressed);
                break;
            case SDL_SCANCODE_LEFT:
                on_key_event(Key::Left, pressed);
                break;
            case SDL_SCANCODE_UP:
                on_key_event(Key::Up, pressed);
                break;
            case SDL_SCANCODE_DOWN:
                on_key_event(Key::Down, pressed);
                break;
            default:
                break;
            }
        }
    }

    return true;
}

void Display::teardown() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
}