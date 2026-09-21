#include "frontend.h"


Frontend::~Frontend() {
    teardown();
}


void Frontend::present(const Frame& frame, const std::vector<float>& audio_buffer) {
    frame_count += 1;
    if (frame_count == 60) {
        SDL_SetWindowTitle(window, update_title().c_str());
        frame_count = 0;
    }

    // sync-by-audio
    while(SDL_GetAudioStreamQueued(audio_stream) > TARGET_QUEUE_BYTES) {
        SDL_Delay(1);   // Unsure about this - remove if it impacts performance
    }

    SDL_PutAudioStreamData(audio_stream, audio_buffer.data(), audio_buffer.size() * sizeof(float));

    SDL_UpdateTexture(texture, nullptr, frame.pixels.data(), Frame::WIDTH * sizeof(Pixel));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}


void Frontend::poll_events() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            window_closed = true;
            return;
        }

        if (e.type == SDL_EVENT_KEY_DOWN && e.key.repeat == 0) {
            if (e.key.scancode == SDL_SCANCODE_P) {
                pause_requested = true;
            } else if (e.key.scancode == SDL_SCANCODE_O) {
                open_rom_dialog();
            }
        }
    }

    if (focus_requested) {
        SDL_RaiseWindow(window);
        focus_requested = false;
    }
}

bool Frontend::should_quit() {
    bool quit = window_closed;
    window_closed = false;
    return quit;
}

ButtonState Frontend::get_button_state() {
    const bool* keys = SDL_GetKeyboardState(nullptr);


    ButtonState state;
    state.a      = keys[SDL_SCANCODE_Z];
    state.b      = keys[SDL_SCANCODE_X];
    state.select = keys[SDL_SCANCODE_RSHIFT];
    state.start  = keys[SDL_SCANCODE_RETURN];
    state.right  = keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D];
    state.left   = keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A];
    state.up     = keys[SDL_SCANCODE_UP]    || keys[SDL_SCANCODE_W];
    state.down   = keys[SDL_SCANCODE_DOWN]  || keys[SDL_SCANCODE_S];
    
    return state;
}


void Frontend::open_rom_dialog() {
    rom_dialog_open = true;

    SDL_DialogFileFilter filters[] = {
        {"Game Boy / Game Boy Color ROMs", "gb;gbc"}
        //{"Game Boy Advance ROMs", "gba"}
    };

    SDL_ShowOpenFileDialog(file_dialog_callback, this, window, filters, 1, nullptr, false);
}


std::optional<std::string> Frontend::take_pending_rom() {
    return std::exchange(pending_rom, std::nullopt);
}


bool Frontend::take_pause_request() {
    bool requested = pause_requested;
    pause_requested = false;
    return requested;
}


void SDLCALL Frontend::file_dialog_callback(void* userdata, const char* const* filelist, int filter) {
    Frontend* frontend = static_cast<Frontend*>(userdata);

    if (filelist == nullptr) {
        frontend->rom_dialog_open = false;
        return;
    }

    if (filelist[0] != nullptr) {
        frontend->pending_rom = filelist[0];
        frontend->focus_requested = true;
    }

    frontend->rom_dialog_open = false;
}


void Frontend::init(const std::string& title) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        throw std::runtime_error(std::string("Failed to initialize SDL ") + SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer(
        update_title().c_str(),
        DISPLAY_SCALE * Frame::WIDTH,
        DISPLAY_SCALE * Frame::HEIGHT,
        SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        teardown();
        throw std::runtime_error(std::string("Failed to create window and renderer ") + SDL_GetError());
    }

    if (!SDL_SetRenderLogicalPresentation(renderer, Frame::WIDTH, Frame::HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
        teardown();
        throw std::runtime_error(std::string("Failed to set renderer logical presentation ") + SDL_GetError());
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Frame::WIDTH, Frame::HEIGHT);
    if (!texture) {
        teardown();
        throw std::runtime_error(std::string("Failed to create texture ") + SDL_GetError());
    }

    SDL_AudioSpec spec = {SDL_AUDIO_F32, NUM_AUDIO_CHANNELS, (int)SAMPLES_PER_SECOND};
    audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!audio_stream) {
        teardown();
        throw std::runtime_error(std::string("Failed to create audio stream ") + SDL_GetError());
    }

    SDL_ResumeAudioStreamDevice(audio_stream);
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

    if (audio_stream) {
        SDL_DestroyAudioStream(audio_stream);
    }

    SDL_Quit();
}


void Frontend::set_title(const std::string& title) {
    size_t start = title.find_last_of('/');
    start = (start == std::string::npos) ? 0 : start + 1;
    size_t end = title.find_last_of('.');
    base_title = title.substr(start, end - start);
}


std::string Frontend::update_title() {
    std::ostringstream oss;
    oss << base_title << " (" << std::fixed << std::setprecision(1) << fps << " fps)";

    return oss.str();
}