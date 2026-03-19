#ifndef PPU_H
#define PPU_H

#include <bitset>
#include <array>
#include "../utils/utils.h"
#include "../memory/memory_map.h"

constexpr int TILES_PER_BANK = 384; // 3 tile blocks of 128 tiles each

constexpr int CYCLES_PER_SCANLINE = 114;
constexpr int SCANLINES_PER_FRAME = 154;

// m-cycle in scanline at which each stage ends
constexpr int OAM_SCAN_END = 20;
constexpr int TRANSFER_END = 78;  // average, based on pan docs numbers [172, 289] dots

constexpr RGBA32 dmg_palette[4] = {
    0xEFEFEFFF, // White
    0x9F9F9FFF, // Light Gray
    0x5F5F5FFF, // Dark Gray
    0x0F0F0FFF  // Black
};

constexpr RGBA32 dmg_green_palette[4] = {
    0xE0F8D0FF, // Lightest green
    0x88C070FF, // Light green
    0x346856FF, // Dark green
    0x081820FF  // Darkest / almost black
};

// NOTE: PPU locks VRAM during mode 3, locks OAM during modes 2 & 3
// NOTE: 1 frame is 16.74 ms, not exactly 1/60th of a second (16.67 ms)
// NOTE: Until MBC is working, just use bank 0 of VRAM
// NOTE: Need a way to signal that frame is ready


/**
 * Pixel Processing Unit
 */
class PPU {
    public:
        PPU(InterruptCallback i, FrameCallback f) :
            request_interrupt(i),
            frame_ready(f),

            vram{},
            oam{},

            tile_cache{},
            frame_buffer{},

            lcd_control(0x00),
            lcd_status(0x00),
            viewport_y(0x00),
            viewport_x(0x00),
            lcd_y(0x00),
            ly_compare(0x00),
            oam_dma(0x00),
            background_palette(0x00),
            object_palette_0(0x00),
            object_palette_1(0x00),
            window_y(0x00),
            window_x(0x00),

            vram_bank(0x00),

            mode(Mode::VBLANK),
            cycles(0),
            window_line_counter(0),

            cgb_mode(false)
        {}

        Byte read(const Address address) const;
        void write(const Address address, const Byte value);

        void load(const Address address, const std::vector<Byte>& data);

        std::array<RGBA32, SCREEN_WIDTH * SCREEN_HEIGHT> get_frame() const { return frame_buffer; }

        void update();
        inline void set_cgb_mode(const bool cgb) { cgb_mode = cgb; }
        void dump_oam();


    private:
        struct Tile {
            bool dirty = true;
            std::array<int, 64> pixels = {0}; // Tiles are 8x8 -> 64 pixels
        };

        InterruptCallback request_interrupt;
        FrameCallback frame_ready;
        
        std::array<Byte, 2 * VRAM_SIZE> vram;      // 0x8000 - 0x9FFF, two banks on CGB
        std::array<Byte, OAM_SIZE> oam;            // 0xFE00 - 0xFE9F
        // TODO: Add Color RAM

        std::array<Tile, 2 * TILES_PER_BANK> tile_cache;
        std::array<RGBA32, SCREEN_WIDTH * SCREEN_HEIGHT> frame_buffer;
        
        // PPU's IO Registers
        Byte lcd_control;                 // LCDC REGISTER
        Byte lcd_status;                  // STAT REGISTER
        Byte viewport_y;                  // SCY REGISTER
        Byte viewport_x;                  // SCX REGISTER
        Byte lcd_y;                       // LY REGISTER
        Byte ly_compare;                  // LYC REGISTER
        Byte oam_dma;                     // DMA REGISTER
        Byte background_palette;          // BGP REGISTER
        Byte object_palette_0;            // OBP0 REGISTER
        Byte object_palette_1;            // OBP1 REGISTER
        Byte window_y;                    // WY REGISTER
        Byte window_x;                    // WX REGISTER

        // CGB Registers
        Byte vram_bank;                   // VBK REGISTER

        Mode mode;                        // (OAM SCAN -> DRAW PIXEL -> HBLANK) * 144 -> VBLANK * 10
        int cycles;
        int window_line_counter;
        bool cgb_mode;

        void draw_scanline();
        void draw_background(std::array<bool, SCREEN_WIDTH>& background_priority);
        void draw_window(std::array<bool, SCREEN_WIDTH>& background_priority);
        void draw_sprites(const std::array<bool, SCREEN_WIDTH>& background_priority);
        
        // Helper functions
        Address get_tile_address(const Byte tile_id)const ;
        int address_to_index(const Address tile_address) const;

        std::array<int, 8> fetch_pixel_slice(const Byte low_byte, const Byte high_byte) const;
        void refresh_tile(const Address tile_address);

        RGBA32 color_id_to_argb(const int color_id, const Byte palette) const;

        std::array<Address, 10> select_sprites(const int sprite_height);
};

#endif // PPU_H