#ifndef PPU_H
#define PPU_H

#include <bitset>
#include <array>
#include <SDL3/SDL.h>

#include "../memory/memory_map.h"

typedef std::array<Byte, 16> Tile;

// Pixel counts of display screen and larger background map 
const std::size_t SCREEN_WIDTH = 160;
const std::size_t SCREEN_HEIGHT = 144;

const std::size_t BACKGROUND_WIDTH = 256;
const std::size_t BACKGROUND_HEIGHT = 256;

const std::size_t COLOR_CHANNELS = 3;

const int CYCLES_PER_SCANLINE = 114;
const int FRAME_PERIOD = 144 * CYCLES_PER_SCANLINE;



// NOTE: PPU locks VRAM during mode 3, locks OAM during modes 2 & 3
// NOTE: 1 frame is 16.74 ms, not exactly 1/60th of a second (16.67 ms)
// NOTE: Until MBC is working, just use bank 0 of VRAM
// NOTE: Background not working
// NOTE: Window not working
// NOTE: Sprites not working
class PPU {
    public:
        PPU(InterruptCallback cb) : 
            request_interrupt(cb),
            vram{0},
            oam{0},
            
            frame_buffer{0},
            background_map{0},

            lcd_status(0x00),
            viewport_y(0x00),
            viewport_x(0x00),
            lcd_y(0x00),
            ly_compare(0x00),
            oam_dma(0x00),
            bgp(0x00),
            object_palette_0(0x00),
            object_palette_1(0x00),
            window_y(0x00),
            window_x(0x00),
            mode(Mode::OAM_SCAN)
            {
                // initialize all flags to 0
                write(LCDC_REGISTER, 0x00);
            }

        Byte read(const Address address) const;
        void write(const Address address, const Byte value);

    private:
        InterruptCallback request_interrupt;
        
        std::array<Byte, VRAM_SIZE> vram;      // 0x8000 - 0x9FFF
        std::array<Byte, OAM_SIZE> oam;        // 0xFE00 - 0xFE9F

        Byte_Array3D<SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_CHANNELS> frame_buffer; 
        // Byte_Array3D<SCREEN_WIDTH, SCREEN_HEIGHT, (COLOR_CHANNELS + 1)> frame_buffer_alpha;   

        Byte_Array3D<BACKGROUND_WIDTH, BACKGROUND_HEIGHT, COLOR_CHANNELS> background_map;
        // Byte_Array3D<BACKGROUND_WIDTH, BACKGROUND_HEIGHT, (COLOR_CHANNELS + 1)> background_map_alpha;

        Byte lcd_control;                 // LCDC REGISTER
        Byte lcd_status;                  // STAT REGISTER
        Byte viewport_y;                  // SCY REGISTER
        Byte viewport_x;                  // SCX REGISTER
        Byte lcd_y;                       // LY REGISTER
        Byte ly_compare;                  // LYC REGISTER
        Byte oam_dma;                     // DMA REGISTER
        Byte bgp;                         // BGP REGISTER
        Byte object_palette_0;            // OBP0 REGISTER
        Byte object_palette_1;            // OBP1 REGISTER
        Byte window_y;                    // WY REGISTER
        Byte window_x;                    // WX REGISTER

        bool enabled;                     // LCDC.7
        Address window_tile_map;          // LCDC.6   
        bool window_enable;               // LCDC.5
        Address tile_data;                // LCDC.4
        Address background_tile_map;      // LCDC.3
        bool obj_size;                    // LCDC.2   (true = 8x16 sprites, false = 8x8 sprites)
        bool obj_enable;                  // LCDC.1
        bool background_window_enable;    // LCDC.0

        Mode mode;                        // OAM SCAN -> DRAW PIXEL -> HBLANK -> VBLANK
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
