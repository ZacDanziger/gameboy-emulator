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
    while(SDL_GetAudioStreamQueued(audio_stream) > TARGET_QUEUE_BYTES) {}

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
            event_buffer.push_back(QuitRequested{});
            return;
        }

        if ((e.type == SDL_EVENT_KEY_DOWN) || (e.type == SDL_EVENT_KEY_UP)) {
            bool pressed = (e.type == SDL_EVENT_KEY_DOWN);

            switch(e.key.scancode) {
            case SDL_SCANCODE_Z:
                event_buffer.push_back(InputEvent{Button::A, pressed});
                break;
            case SDL_SCANCODE_X:
                event_buffer.push_back(InputEvent{Button::B, pressed});
                break;
            case SDL_SCANCODE_RSHIFT:
                event_buffer.push_back(InputEvent{Button::Select, pressed});
                break;
            case SDL_SCANCODE_RETURN:
                event_buffer.push_back(InputEvent{Button::Start, pressed});
                break;
            case SDL_SCANCODE_RIGHT:
            case SDL_SCANCODE_D:
                event_buffer.push_back(InputEvent{Button::Right, pressed});
                break;
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_A:
                event_buffer.push_back(InputEvent{Button::Left, pressed});
                break;
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_W:
                event_buffer.push_back(InputEvent{Button::Up, pressed});
                break;
            case SDL_SCANCODE_DOWN:
            case SDL_SCANCODE_S:
                event_buffer.push_back(InputEvent{Button::Down, pressed});
                break;
            case SDL_SCANCODE_P:
                if (pressed) event_buffer.push_back(TogglePause{});
                break;
            case SDL_SCANCODE_O:
                if (pressed) open_rom_dialog();
                break;
            default:
                break;
            }
        }
    }

    if (focus_requested) {
        SDL_RaiseWindow(window);
        focus_requested = false;
    }
}


std::vector<AppEvent> Frontend::take_events() {
    return std::exchange(event_buffer, std::vector<AppEvent>{});
}


void Frontend::open_rom_dialog() {
    event_buffer.push_back(RomDialogOpened{});

    SDL_DialogFileFilter filters[] = {
        {"Game Boy / Game Boy Color ROMs", "gb;gbc"}
        //{"Game Boy Advance ROMs", "gba"}
    };

    SDL_ShowOpenFileDialog(file_dialog_callback, this, window, filters, 1, nullptr, false);
}


// std::optional<std::string> Frontend::take_pending_rom() {
//     return std::exchange(pending_rom, std::nullopt);
// }


// bool Frontend::take_pause_request() {
//     bool requested = pause_requested;
//     pause_requested = false;
//     return requested;
// }


void SDLCALL Frontend::file_dialog_callback(void* userdata, const char* const* filelist, int filter) {
    Frontend* frontend = static_cast<Frontend*>(userdata);

    if (filelist == nullptr) {
        frontend->event_buffer.push_back(RomDialogCanceled{});
        return;
    }

    if (filelist[0] != nullptr) {
        frontend->event_buffer.push_back(RomSelected{filelist[0]});
        frontend->focus_requested = true;
    }
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