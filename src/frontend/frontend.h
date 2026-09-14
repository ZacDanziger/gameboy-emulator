#ifndef FRONTEND_H
#define FRONTEND_H

#include <string>
#include <array>
#include <sstream>
#include <iomanip>
#include <optional>
#include <utility>

#include "../types.h"
#include "../joypad/joypad.h"

#include <SDL3/SDL.h>

constexpr int DISPLAY_SCALE = 5;    // initial scaling factor of screen
constexpr int NUM_AUDIO_CHANNELS = 2;
constexpr int TARGET_QUEUE_BYTES = SAMPLES_PER_SECOND * NUM_AUDIO_CHANNELS * sizeof(float) * 0.1f;

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

            joypad(j),

            base_title(title),
            pending_rom(),

            focus_requested(false),
            pause_requested(false),

            fps(60.0),
            frame_count(0)
        {
            init(title);
        }

        ~Frontend();

        void present(const std::array<RGBA32, SCREEN_WIDTH * SCREEN_HEIGHT>& frame, const std::vector<float>& audio_buffer);
        bool poll_events(); 

        void open_rom_dialog();
        std::optional<std::string> take_pending_rom();
        bool take_pause_request();

        void clear_audio() { SDL_ClearAudioStream(audio_stream); }

        void set_title(const std::string& title);
        void set_fps(const double new_fps) { fps = new_fps; }
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        
        SDL_AudioStream* audio_stream;
        
        Joypad& joypad;
        
        std::string base_title;
        std::optional<std::string> pending_rom;

        bool focus_requested;
        bool pause_requested;

        double fps;
        int frame_count;
        
        void init(const std::string& title);
        void teardown();

        static void SDLCALL file_dialog_callback(void* userdata, const char* const* filelist, int filter);

        std::string update_title();
};

#endif  // FRONTEND_H