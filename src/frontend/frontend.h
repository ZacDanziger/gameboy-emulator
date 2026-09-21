#ifndef FRONTEND_H
#define FRONTEND_H

#include <string>
#include <array>
#include <sstream>
#include <iomanip>
#include <optional>
#include <utility>

#include "../common/input.h"
#include "../common/output.h"

#include <SDL3/SDL.h>

constexpr int DISPLAY_SCALE = 5;    // initial scaling factor of screen
constexpr int NUM_AUDIO_CHANNELS = 2;
constexpr int TARGET_QUEUE_BYTES = SAMPLES_PER_SECOND * NUM_AUDIO_CHANNELS * sizeof(float) * 0.1f;

/**
 * Handles all SDL3 code, including window & screen, keyboard input, and audio
 */
class Frontend {
    public:
        Frontend(const std::string& title) :
            window(nullptr),
            renderer(nullptr),
            texture(nullptr),

            audio_stream(nullptr),

            base_title(title),

            pending_rom(std::nullopt),

            window_closed(false),
            pause_requested(false),
            rom_dialog_open(false),
            focus_requested(false),

            fps(60.0),
            frame_count(0)
        {
            init(title);
        }

        ~Frontend();

        void present(const Frame& frame, const std::vector<float>& audio_buffer);

        void poll_events(); 
        bool should_quit();
        bool is_rom_dialog_open() { return rom_dialog_open; }
        
        bool take_pause_request();
        std::optional<std::string> take_pending_rom();

        ButtonState get_button_state();

        void clear_audio() { SDL_ClearAudioStream(audio_stream); }
        void set_title(const std::string& title);
        void set_fps(const double new_fps) { fps = new_fps; }

        void delay_ms(uint32_t ms) { SDL_Delay(ms); }
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        
        SDL_AudioStream* audio_stream;
        
        std::string base_title;

        std::optional<std::string> pending_rom;

        bool window_closed;
        bool pause_requested;
        bool rom_dialog_open;
        bool focus_requested;

        double fps;
        int frame_count;
        
        void init(const std::string& title);
        void teardown();

        void open_rom_dialog();
        static void SDLCALL file_dialog_callback(void* userdata, const char* const* filelist, int filter);

        std::string update_title();
};

#endif  // FRONTEND_H