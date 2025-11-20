#ifndef PPU_H
#define PPU_H

#include "../timer/timer.h"
#include "../memory/memory.h"
#include <bitset>
#include <SDL3/SDL.h>

typedef std::array<Byte, 16> Tile;



// Pixel counts of display screen and larger background map 
const std::size_t SCREEN_WIDTH = 160;
const std::size_t SCREEN_HEIGHT = 144;

const std::size_t BACKGROUND_WIDTH = 256;
const std::size_t BACKGROUND_HEIGHT = 256;

const std::size_t COLOR_CHANNELS = 3;

const int CYCLES_PER_SCANLINE = 114;
const int FRAME_PERIOD = 144 * CYCLES_PER_SCANLINE;


template <std::size_t width, std::size_t height, std::size_t depth>
using Byte_Array3D = std::array<std::array<std::array<Byte, width>, height>, depth>;

/**
 * Tile data--------0x8000 - 0x97FF
 * |- Block 0-------0x8000 - 0x87FF (8000 method)(For Objects)
 * |- Block 1-------0x8800 - 0x8FFF (Shared / both methods)
 * |- Block 2-------0x9000 - 0x97FF (8800 method)
 * 
 * Tile map 1-------0x9800 - 0x9BFF (32 x 32 tiles)
 * Tile map 2-------0x9C00 - 0x9FFF (32 x 32 tiles)
 */

 const Address TILE_DATA_0 = 0x8000;
 const Address TILE_DATA_1 = 0x9000;
 const Address TILE_MAP_0_START = 0x9800;
 const Address TILE_MAP_1_START = 0x9C00;

enum Mode {
    HBLANK,
    VBLANK,
    OAM_SCAN,
    DRAW_PIXELS
};


// NOTE: PPU locks VRAM during mode 3, locks OAM during modes 2 & 3
// NOTE: 1 frame is 16.74 ms, not exactly 1/60th of a second (16.67 ms)
// NOTE: Until MBC is working, just use bank 0 of VRAM
class PPU {
    public:
        PPU();

        void init(Timer* timer_ptr, Memory* memory_ptr);
        void render_frame();
        void update_cycles(int m_cycles);

    private:
        Memory *memory;
        Timer *timer;

        Byte_Array3D<SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_CHANNELS> frame_buffer; 
        // Byte_Array3D<SCREEN_WIDTH, SCREEN_HEIGHT, (COLOR_CHANNELS + 1)> frame_buffer_alpha;   

        Byte_Array3D<BACKGROUND_WIDTH, BACKGROUND_HEIGHT, COLOR_CHANNELS> background_map;
        // Byte_Array3D<BACKGROUND_WIDTH, BACKGROUND_HEIGHT, (COLOR_CHANNELS + 1)> background_map_alpha;

        bool enabled;                     // LCDC.7
        Address window_tile_map;          // LCDC.6   
        bool window_enable;               // LCDC.5
        Address tile_data;                // LCDC.4
        Address background_tile_map;      // LCDC.3
        bool obj_size;                    // LCDC.2   (true = 8x16 sprites, false = 8x8 sprites)
        bool obj_enable;                  // LCDC.1
        bool background_window_enable;    // LCDC.0

        Mode mode;                        // OAM SCAN -> DRAW PIXEL -> HBLANK -> VBLANK
        Byte scan_line;                   // local copy of LY register

        Word scanline_clock_cycles;
        Word frame_clock_cycles;

        void update_flags();
        void request_stat_interrupt() { memory->write(IF_REGISTER, (memory->read(IF_REGISTER) | 0b10)); }
        void request_vblank_interrupt() { memory->write(IF_REGISTER, (memory->read(IF_REGISTER) | 0x01)); }
        void ly_compare();
        
        
        void update_scanline();
        void draw_background();
        void draw_window();
        void draw_objects();
        
        Byte get_tile_ID(const Address address) { return ((address >> 4) % 256); }  // does this matter?
        Address get_tile_address(const Byte offset) const;
        Tile get_tile(const Address address);
        void get_color_map(const Tile& tile, std::array<std::array<std::bitset<2>, 8>, 8> &color_map);
};

#endif // PPU_H

// SDL Example Program:
// int main(int, char**){
//     SDL_Init(SDL_INIT_VIDEO);

//     SDL_Window* win = SDL_CreateWindow("SDL3 Image", 640, 480, 0);
//     if (win == nullptr) {
//         std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
//     if (ren == nullptr) {
//         std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
//         SDL_DestroyWindow(win);
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Surface* bmp = SDL_LoadBMP("lettuce.bmp");
//     if (bmp == nullptr) {
//         std::cerr << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
//         SDL_DestroyRenderer(ren);
//         SDL_DestroyWindow(win);
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, bmp);
//     SDL_DestroySurface(bmp);

//     if (tex == nullptr) {
//         std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
//         SDL_DestroyRenderer(ren);
//         SDL_DestroyWindow(win);
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Event e;
//     bool quit = false;

//     while (!quit) {
//         while (SDL_PollEvent(&e)) {
//             if (e.type == SDL_EVENT_QUIT) {
//                 quit = true;
//             }
//         }

//         SDL_RenderClear(ren);
//         SDL_RenderTexture(ren, tex, NULL, NULL);
//         SDL_RenderPresent(ren);
//     }

//     SDL_DestroyTexture(tex);
//     SDL_DestroyRenderer(ren);
//     SDL_DestroyWindow(win);
//     SDL_Quit();

//     return 0;
// }
