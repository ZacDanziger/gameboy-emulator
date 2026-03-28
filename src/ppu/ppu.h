#ifndef PPU_H
#define PPU_H

#include <bitset>
#include <array>
#include "../utils/utils.h"
#include "../memory/memory_map.h"

#include <iostream>

using FrameCallback = std::function<void()>;
using HBlankCallback = std::function<void()>;

constexpr int MAX_SPRITES = 40;
constexpr int MAX_SPRITES_PER_SCANLINE = 10;

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


/**
 * Pixel Processing Unit
 */
class PPU {
    public:
        PPU(InterruptCallback i, FrameCallback f, HBlankCallback h) :
            request_interrupt(i),
            frame_ready(f),
            hblank(h),

            vram{},
            oam{},

            background_color_ram{},
            object_color_ram{},

            tile_cache{},
            frame_buffer{},

            lcd_control(0x91),
            lcd_status(0x00),
            viewport_y(0x00),
            viewport_x(0x00),
            lcd_y(0xFF),
            ly_compare(0x00),
            oam_dma(0x00),
            background_palette(0xFC),
            object_palette_0(0x00),
            object_palette_1(0x00),
            window_y(0x00),
            window_x(0x00),

            vram_bank(0x00),
            background_palette_index(0x00),
            object_palette_index(0x00),
            object_priority(0x00),

            mode(Mode::VBLANK),
            cycles(-1),
            window_line_counter(0),

            cgb_mode(false)
        {}

        Byte read(const Address address) const;
        void write(const Address address, const Byte value);

        void load(const Address address, const std::vector<Byte>& data);

        inline std::array<RGBA32, SCREEN_WIDTH * SCREEN_HEIGHT> get_frame() const { return frame_buffer; }

        void update();
        inline void set_cgb_mode(const bool cgb) { cgb_mode = cgb; }

        // Debug Function
        void print_tiles_ppm(const std::string& filename);
    private:
        struct Tile {
            bool dirty = true;
            std::array<int, 64> pixels{}; // Tiles are 8x8 -> 64 pixels
        };

        InterruptCallback request_interrupt;
        FrameCallback frame_ready;
        HBlankCallback hblank;
        
        std::array<Byte, 2 * VRAM_SIZE> vram;      // 0x8000 - 0x9FFF, two banks on CGB
        std::array<Byte, OAM_SIZE> oam;            // 0xFE00 - 0xFE9F
        
        // CGB only
        std::array<Byte, 64> background_color_ram;
        std::array<Byte, 64> object_color_ram;

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
        Byte background_palette_index;    // BCPS/BCPI REGISTER
        Byte object_palette_index;        // OCPS/OCPI REGISTER
        Byte object_priority;             // OPRI REGISTER - not currently used, may change in future


        Mode mode;                        // 
        int cycles;
        int window_line_counter;
        bool cgb_mode;

        void draw_scanline();
        void draw_background(std::array<int, SCREEN_WIDTH>& bg_color_ids, std::array<bool, SCREEN_WIDTH>& bg_high_priority);
        void draw_window(std::array<int, SCREEN_WIDTH>& bg_color_ids, std::array<bool, SCREEN_WIDTH>& bg_high_priority);
        void draw_sprites(const std::array<int, SCREEN_WIDTH>& bg_color_ids, const std::array<bool, SCREEN_WIDTH>& bg_high_priority);
        
        // Helper functions
        Address get_tile_address(const Byte tile_id)const ;
        int address_to_index(const Address tile_address, int bank = 0) const;

        std::array<int, 8> fetch_pixel_slice(const Byte low_byte, const Byte high_byte) const;
        void refresh_tile(const Address tile_address, int bank = 0);

        RGBA32 color_id_to_rgba(const int color_id, const Byte palette, const bool is_sprite = false) const;

        std::array<Address, 10> select_sprites(const int sprite_height);

        // Debug function
        std::vector<Byte> dump_tiles();
};

#endif // PPU_H