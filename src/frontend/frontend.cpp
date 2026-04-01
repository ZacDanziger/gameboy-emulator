#include "frontend.h"


Frontend::~Frontend() {
    teardown();
}

void Frontend::present(const std::array<RGBA32, SCREEN_WIDTH * SCREEN_HEIGHT>& frame) {
    SDL_UpdateTexture(texture, nullptr, frame.data(), SCREEN_WIDTH * sizeof(RGBA32));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}


bool Frontend::poll_events() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            return false;
        }

        if ((e.type == SDL_EVENT_KEY_DOWN) || (e.type == SDL_EVENT_KEY_UP)) {
            bool pressed = (e.type == SDL_EVENT_KEY_DOWN);

            switch(e.key.scancode) {
            case SDL_SCANCODE_Z:
                joypad.set_key(Key::A, pressed);
                break;
            case SDL_SCANCODE_X:
                joypad.set_key(Key::B, pressed);
                break;
            case SDL_SCANCODE_RSHIFT:
                joypad.set_key(Key::Select, pressed);
                break;
            case SDL_SCANCODE_RETURN:
                joypad.set_key(Key::Start, pressed);
                break;
            case SDL_SCANCODE_RIGHT:
            case SDL_SCANCODE_D:
                joypad.set_key(Key::Right, pressed);
                break;
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_A:
                joypad.set_key(Key::Left, pressed);
                break;
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_W:
                joypad.set_key(Key::Up, pressed);
                break;
            case SDL_SCANCODE_DOWN:
            case SDL_SCANCODE_S:
                joypad.set_key(Key::Down, pressed);
                break;
            case SDL_SCANCODE_P:
                joypad.set_key(Key::SAVE, pressed);
                break;
            default:
                break;
            }
        }
    }

    return true;
}


void Frontend::init(const std::string& title) {
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


void Frontend::teardown() {
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