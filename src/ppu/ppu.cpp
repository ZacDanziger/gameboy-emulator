#include "ppu.h"

PPU::PPU(){
    memory = nullptr;
    timer = nullptr;
    frame_buffer = {0};
    background_map = {0};
    scanline_clock_cycles = 0;
    frame_clock_cycles = 0;
}

void PPU::init(Timer* timer_ptr, Memory* memory_ptr) {
    timer = timer_ptr;
    memory = memory_ptr;

    // SDL start up here?
}


void PPU::render_frame() {
    //TODO: pass frame to SDL to render
}

// TODO: FINISH ME
void PPU::update_cycles(int m_cycles) {
    update_flags();
    if (!enabled) {
        return;
    }

    std::bitset<8> lcd_status(memory->read(STAT_REGISTER));

    frame_clock_cycles += m_cycles;
    scanline_clock_cycles += m_cycles;

    // In VBLANK
    if (frame_clock_cycles >= 16416) {
        if (mode != VBLANK) {
            mode = VBLANK;
            request_vblank_interrupt();
            // push completed frame to SDL
            render_frame();
        }
        // finished VBLANK period
        if (frame_clock_cycles >= 17556) {
            frame_clock_cycles = 0;
            scanline_clock_cycles = 0;
            memory->write(LY_REGISTER, 0x00);
            ly_compare();
        }
    } else {
        if (scanline_clock_cycles < 20) {
            if (mode != OAM_SCAN) {
                mode = OAM_SCAN;
                if (lcd_status[5]) {
                    request_stat_interrupt();
                }
            }
        }
        // if (scanline_clock_cycles < ) {
        //     if (mode != DRAW_PIXELS) {
        //         mode = DRAW_PIXELS;
        //     }
        // }
        // if (scanline_clock_cycles < ) {
        //     if (mode != HBLANK) {
        //         mode = HBLANK;
        //         if (lcd_status[3])
        //     }
        // }
    }
}


/**
 * Add scanline to frame_buffer
 * One scanline takes 114 m-cycles
 */
void PPU::update_scanline() {
    if (background_window_enable) {
        draw_background();
    }
    if (window_enable) {
        draw_window();
    }
    if (obj_enable) {
        draw_objects();
    }
}

/**
 * Read LCDC register from memory, update control flags to match
 */
void PPU::update_flags() {
    std::bitset<8> lcd_control(memory->read(LCDC_REGISTER));

    enabled = lcd_control[7];
    window_tile_map = lcd_control[6] ? TILE_MAP_1_START : TILE_MAP_0_START;
    window_enable = lcd_control[5];
    tile_data = lcd_control[4] ? TILE_DATA_0 : TILE_DATA_1;
    background_tile_map = lcd_control[3] ? TILE_MAP_1_START : TILE_MAP_0_START;
    obj_size = lcd_control[2];
    obj_enable = lcd_control[1];
    background_window_enable = lcd_control[0];
}


/**
 * 
 */
Address PPU::get_tile_address(const Byte offset) const {
    // Case 1: Objects
    // TODO: handle coming from OAM

    if (tile_data == TILE_DATA_0) {
        // Case 2: LCDC.4 == 1 -> 0x8000 addressing mode
        return (tile_data + (16 * offset));
    } else {
        // Case 3: LCDC.4 == 0 -> 0x9000 addressing mode
        int8_t signed_offset = (int8_t) offset;
        return ((Address) ((int16_t)(tile_data) + (16 * signed_offset)));
    }
}

/**
 * Given a tile's address in memory, get the tile's 16 bytes
 * 
 * @param address the tile's address in memory
 * @return the tile containing the 16 bytes
 */
Tile PPU::get_tile(const Address address) {
    // TODO: handle VRAM bank switching in memory
    // Can't use copy_n here, because I don't have access to underlying VRAM array from PPU
    Tile tile = {0};
    for (int i = 0; i < 16; i++) {
        tile[i] = memory->read(address + i);
    }

    return tile;
}

/**
 * Translate a tile's 16 bytes into the 8x8 grid with 2 bits each, describing the color in that pixel
 * --- NOT TESTED ---
 * 
 * @param tile the tile to convert
 * @param color_map an out parameter that will contain the tile's color map at the end of the function
 */
void get_color_map(const Tile& tile, std::array<std::array<std::bitset<2>, 8>, 8> &color_map) {
    color_map = {0};

    for (int row = 0; row < 8; row ++) {
        // get byte pairs as bitsets
        std::bitset<8> lsb_byte(tile[2 * row]); 
        std::bitset<8> msb_byte(tile[(2 * row) + 1]);

        for (int col = 0; col < 8; col++) {
            color_map[row][col] = ((msb_byte[7 - col] << 1) | lsb_byte[7 - col]);
        }
    }
}