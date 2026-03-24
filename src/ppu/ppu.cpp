#include "ppu.h"

static int count = 0;

/**
 * Read from VRAM, OAM, or any PPU owned IO registers
 * 
 * @param address the address to be read from
 * @returns the value in memory at address
 */
Byte PPU::read(const Address address) const {
    if (address >= VRAM_START && address < ERAM_START) {
        if (mode == Mode::TRANSFER) {
            return OPEN_BUS_VALUE;
        }

        uint32_t adjusted_address = static_cast<uint32_t>(address);
        if (cgb_mode) {
            adjusted_address += vram_bank * VRAM_SIZE;
        }
        adjusted_address -= VRAM_START;

        return vram[adjusted_address];
    }

    if (address >= OAM_START && address < NOT_USABLE_START) {
        if ((mode == Mode::OAM_SCAN) || (mode == Mode::TRANSFER)) {
            return 0xFF;
        }
        return oam[address - OAM_START];
    }

    switch(address) {
    case LCDC_REGISTER:
        return lcd_control;
    case STAT_REGISTER:
        return lcd_status;
    case SCY_REGISTER:
        return viewport_y;
    case SCX_REGISTER:
        return viewport_x;
    case LY_REGISTER:
        return lcd_y;
    case LYC_REGISTER:
        return ly_compare;
    case DMA_REGISTER:
        return oam_dma;
    case BGP_REGISTER:
        return background_palette;
    case OBP0_REGISTER:
        return object_palette_0;
    case OBP1_REGISTER:
        return object_palette_1;
    case WY_REGISTER:
        return window_y;
    case WX_REGISTER:
        return window_x;
    case VBK_REGISTER:
        return vram_bank;
    case BCPS_BGPI_REGISTER:
        return background_palette_index;
    case BCPD_BGPD_REGISTER:
        return background_color_ram[background_palette_index & 0x3F];
    case OCPS_OGPI_REGISTER:
        return object_palette_index;
    case OCPD_OGPD_REGISTER:
        return object_color_ram[object_palette_index & 0x3F];
    case OPRI_REGISTER:
        return object_priority;
    default:
        throw std::runtime_error("PPU read called on wrong address");
    }
}


/**
 * Write to VRAM, OAM, or any PPU owned IO registers
 * 
 * @param address the address to be written to
 * @param value the data to be written in the address
 */
void PPU::write(const Address address, const Byte value) {
    if (address >= VRAM_START && address < ERAM_START) {
        if (mode == Mode::TRANSFER) {
            return;
        }

        uint32_t adjusted_address = static_cast<uint32_t>(address);
        if (cgb_mode) {
            adjusted_address += vram_bank * VRAM_SIZE;
        }
        adjusted_address -= VRAM_START;

        vram[adjusted_address] = value;

        if (address < TILE_MAP_0_START) {
            int index = address_to_index(address, vram_bank);
            tile_cache[index].dirty = true;
        }
        return;
    }

    if (address >= OAM_START && address < NOT_USABLE_START) {
        if ((mode != Mode::OAM_SCAN) && (mode != Mode::TRANSFER)) {
            oam[address - OAM_START] = value;
        }
        return;
    }

    switch(address) {
    case LCDC_REGISTER:
        lcd_control = value;
        break;

    // May want to implement spurious STAT interrupt
    // https://gbdev.io/pandocs/STAT.html#spurious-stat-interrupts
    case STAT_REGISTER:
        // Bits 0-2 are read-only
        lcd_status = (value & 0xF8);
        break;
    case SCY_REGISTER:
        viewport_y = value;
        break;
    case SCX_REGISTER:
        viewport_x = value;
        break;
    case LY_REGISTER:
        // LY is read only, change it manually
        break;
    case LYC_REGISTER:
        ly_compare = value;
        break;
    case DMA_REGISTER:
        oam_dma = value;
        break;
    case BGP_REGISTER:
        background_palette = value;
        break;
    case OBP0_REGISTER:
        object_palette_0 = value;
        break;
    case OBP1_REGISTER:
        object_palette_1 = value;
        break;
    case WY_REGISTER:
        window_y = value;
        break;
    case WX_REGISTER:
        window_x = value;
        break;
    case VBK_REGISTER:
        if (!cgb_mode) { break; }
        vram_bank = (value & 0x01);
        break;
    case BCPS_BGPI_REGISTER:
        if (!cgb_mode) { break; }
        background_palette_index = value;
        break;
    case BCPD_BGPD_REGISTER:
        if (!cgb_mode) { break; }
        background_color_ram[background_palette_index & 0x3F] = value;
        // if increment is set, increment
        if (is_set(background_palette_index, Bit::Bit7)) {
            background_palette_index = ((background_palette_index & 0x80) | ((background_palette_index + 1) & 0x3F));
        }
        break;
    case OCPS_OGPI_REGISTER:
        if (!cgb_mode) { break; }
        object_palette_index = value;
        break;
    case OCPD_OGPD_REGISTER:
        if (!cgb_mode) { break; }
        object_color_ram[object_palette_index & 0x3F] = value;
        if (is_set(object_palette_index, Bit::Bit7)) {
            object_palette_index = ((object_palette_index & 0x80) | ((object_palette_index + 1) & 0x3F));
        }
        break;
    case OPRI_REGISTER:
        if (!cgb_mode) { break; }
        object_priority = value;
        break;
    default:
        throw std::runtime_error("PPU write called on wrong address");
    }
}


/**
 * Loads the data into a section of memory owned by the PPU
 * 
 * @param address either a value in VRAM [0x8000, 0x9FFF] or the start of OAM (0xFE00)
 * @param data the data to be loaded into VRAM or OAM
 */
void PPU::load(const Address address, const std::vector<Byte>& data) {
    
    size_t size = 0;

    if (address >= VRAM_START && address < ERAM_START) {
        if (!cgb_mode) {
            return;
        }

        size = std::min(data.size(), static_cast<size_t>(ERAM_START - address));
        size_t offset = (address - VRAM_START) + (vram_bank * VRAM_SIZE);
        std::copy_n(data.begin(), size, vram.begin() + offset);

        // Mark any affected tiles as dirty
        for (size_t i = 0; i < size; i += 16) {
            Address tile_address = address + i;
            if (tile_address < TILE_MAP_0_START) {
                int index = address_to_index(tile_address, vram_bank);
                tile_cache[index].dirty = true;
            }
        }

        return;
    }

    if (address == OAM_START) {
        size = std::min(data.size(), static_cast<size_t>(OAM_SIZE));

        std::copy_n(data.begin(), size, oam.begin());
        return;
    }

    throw std::runtime_error("PPU Load called on incorrect starting address");
}


/**
 * Step the PPU through 1 m-cycle
 */
void PPU::update() {
    // if screen is disabled, just return
    if (!is_set(lcd_control, Bit::Bit7)) {
        return;
    }

    cycles += 1;
    cycles %= CYCLES_PER_SCANLINE;

    if (cycles == 0) {
        lcd_y += 1;
        lcd_y %= SCANLINES_PER_FRAME;

        if (lcd_y == ly_compare) {

            set_bit(lcd_status, Bit::Bit2);
            if (is_set(lcd_status, Bit::Bit6)) {
                request_interrupt(Interrupt::LCDStat);
            }
        } else {
            reset_bit(lcd_status, Bit::Bit2);
        }
    }

    // VBlank period
    if (lcd_y > 143) {
        if (mode != Mode::VBLANK) {
            mode = Mode::VBLANK;
            reset_bit(lcd_status, Bit::Bit1);
            set_bit(lcd_status, Bit::Bit0);

            request_interrupt(Interrupt::VBlank);
            if (is_set(lcd_status, Bit::Bit4)) {
                request_interrupt(Interrupt::LCDStat);
            }

            frame_ready();
            window_line_counter = 0;
        }

        return;
    }

    if (cycles < OAM_SCAN_END) {
        if (mode != Mode::OAM_SCAN) {
            mode = Mode::OAM_SCAN;
            set_bit(lcd_status, Bit::Bit1);
            reset_bit(lcd_status, Bit::Bit0);
        
            // if interrupt on OAM is set, request interrupt
            if (is_set(lcd_status, Bit::Bit5)) {
                request_interrupt(Interrupt::LCDStat);
            }
        }
    }

    else if (cycles < TRANSFER_END) {
        if (mode != Mode::TRANSFER) {
            mode = Mode::TRANSFER;
            set_bit(lcd_status, Bit::Bit1);
            set_bit(lcd_status, Bit::Bit0);
            
            // do the actual work here
            draw_scanline();
        }
    }
    
    else {
        if (mode != Mode::HBLANK) {
            mode = Mode::HBLANK;
            reset_bit(lcd_status, Bit::Bit1);
            reset_bit(lcd_status, Bit::Bit0);

            // call MMU::hdma_tick()
            hblank();
        
            // if interrupt on HBlank is set, request interrupt
            if (is_set(lcd_status, Bit::Bit3)) {
                request_interrupt(Interrupt::LCDStat);
            }
        }
    }
}


/**
 * Draw one scanline to the frame
 */
void PPU::draw_scanline() {
    std::array<int, SCREEN_WIDTH> bg_color_ids{};
    std::array<bool, SCREEN_WIDTH> bg_high_priority{};

    // Fill scanline with white if DMG, and background palette 0 color 0 if CGB
    int buffer_index = lcd_y * SCREEN_WIDTH;
    RGBA32 fill_color = cgb_mode ? color_id_to_argb(0, 0x00, false) : dmg_palette[0];
    std::fill(
        frame_buffer.begin() + buffer_index,
        frame_buffer.begin() + buffer_index + SCREEN_WIDTH,
        fill_color
    );

    if ((cgb_mode) || (is_set(lcd_control, Bit::Bit0))) {
        draw_background(bg_color_ids, bg_high_priority);
    }

    if ((is_set(lcd_control, Bit::Bit5))) {
        draw_window(bg_color_ids, bg_high_priority);
    }

    if (is_set(lcd_control, Bit::Bit1)) {
        draw_sprites(bg_color_ids, bg_high_priority);
    }
}


/**
 * Draw one line of the background to the frame buffer
 * 
 * @param bg_color_ids an array to keep track of which bg/window color ids each pixel in the scanline used
 * @param bg_high_priority an array to keep track of which bg/window pixels are marked high priority (CGB only)
 */
void PPU::draw_background(std::array<int, SCREEN_WIDTH>& bg_color_ids, std::array<bool, SCREEN_WIDTH>& bg_high_priority) {
    uint8_t y = viewport_y + lcd_y;
    // divide by 8 to get the correct tile row
    uint8_t tile_row = y >> 3;  // number between 0-31

    Address tile_map = is_set(lcd_control, Bit::Bit3) ? TILE_MAP_1_START : TILE_MAP_0_START;

    for (uint8_t pixel_column = 0; pixel_column < SCREEN_WIDTH; pixel_column++) {
        uint8_t x = viewport_x + pixel_column;
        // divide by 8 again for correct tile column
        uint8_t tile_column = x >> 3;   // number between 0-31

        Word offset = (tile_row << 5) | tile_column;
        Address tile_lookup = tile_map + offset;

        // can't use read during Mode 3, which we will always be in during this function call
        Byte tile_id = vram[tile_lookup - VRAM_START];
        Address tile_address = get_tile_address(tile_id);

        /**
         * CGB tile attribute data
         * All fields will be 0 if not in cgb mode
         */
        Byte tile_attributes = cgb_mode ? vram[tile_lookup - VRAM_START + VRAM_SIZE] : 0x00;
        int tile_bank = is_set(tile_attributes, Bit::Bit3) ? 1 : 0;
        int palette = tile_attributes & 0x07;
        bool flip_x = is_set(tile_attributes, Bit::Bit5);
        bool flip_y = is_set(tile_attributes, Bit::Bit6);

        if (is_set(tile_attributes, Bit::Bit7)) {
            bg_high_priority[pixel_column] = true;
        }

        int index = address_to_index(tile_address, tile_bank);

        if (tile_cache[index].dirty) {
            refresh_tile(tile_address, tile_bank);
        }
        
        int row = flip_y ? (7 - (y % 8)) : (y % 8);
        int col = flip_x ? (7 - (x % 8)) : (x % 8);

        int pixel_color_id = tile_cache[index].pixels[(row << 3) + col];
        bg_color_ids[pixel_column] = pixel_color_id;

        int buffer_index = (lcd_y * SCREEN_WIDTH) + pixel_column;

        frame_buffer[buffer_index] = cgb_mode ? 
            color_id_to_argb(pixel_color_id, palette) : color_id_to_argb(pixel_color_id, background_palette);
    }
}


/**
 * Draw one line of the window to the frame buffer
 * 
 * @param bg_color_ids an array to keep track of which bg/window color ids each pixel in the scanline used
 * @param bg_high_priority an array to keep track of which bg/window pixels are marked high priority (CGB only)
 */
void PPU::draw_window(std::array<int, SCREEN_WIDTH>& bg_color_ids, std::array<bool, SCREEN_WIDTH>& bg_high_priority) {
    if ((lcd_y < window_y) || (window_x >= 167)) {
        return;
    }

    // get the correct window line
    uint8_t y = window_line_counter;
    // get the correct window tile row
    uint8_t tile_row = y >> 3;

    Address tile_map = is_set(lcd_control, Bit::Bit6) ? TILE_MAP_1_START : TILE_MAP_0_START;

    // screen x-coordinate starts at window_x - 7 and goes to the right edge of the screen
    for (int pixel_column = window_x - 7; pixel_column < SCREEN_WIDTH; pixel_column++) {
        // window_x values from 0..6 are offscreen
        if (pixel_column < 0) {
            continue;
        }

        // adjust screen x-coordinate relative to window
        uint8_t x = pixel_column - window_x + 7;
        // divide by 8 to get window tile column
        uint8_t tile_column = x >> 3;

        Word offset = (tile_row << 5) | tile_column;
        Address tile_lookup = tile_map + offset;

        // can't use read during Mode 3, which we will always be in during this function call
        Byte tile_id = vram[tile_lookup - VRAM_START];
        Address tile_address = get_tile_address(tile_id);

        /**
         * CGB tile attribute data
         * All fields will be 0 if not in cgb mode
         */
        Byte tile_attributes = cgb_mode ? vram[tile_lookup - VRAM_START + VRAM_SIZE] : 0x00;
        int tile_bank = is_set(tile_attributes, Bit::Bit3) ? 1 : 0;
        int palette = tile_attributes & 0x07;
        bool flip_x = is_set(tile_attributes, Bit::Bit5);
        bool flip_y = is_set(tile_attributes, Bit::Bit6);

        if (is_set(tile_attributes, Bit::Bit7)) {
            bg_high_priority[pixel_column] = true;
        }

        int index = address_to_index(tile_address, tile_bank);

        if (tile_cache[index].dirty) {
            refresh_tile(tile_address, tile_bank);
        }

        int row = flip_y ? (7 - (y % 8)) : (y % 8);
        int col = flip_x ? (7 - (x % 8)) : (x % 8);

        int pixel_color_id = tile_cache[index].pixels[(row << 3) + col];
        bg_color_ids[pixel_column] = pixel_color_id;

        int buffer_index = (lcd_y * SCREEN_WIDTH) + pixel_column;
        frame_buffer[buffer_index] = cgb_mode ? 
            color_id_to_argb(pixel_color_id, palette) : color_id_to_argb(pixel_color_id, background_palette);
    }

    window_line_counter += 1;
}


/**
 * Draw any sprites that belong on the current scanline to the frame buffer
 * 
 * @param bg_color_ids an array to keep track of which bg/window color ids each pixel in the scanline used
 * @param bg_high_priority an array to keep track of which bg/window pixels are marked high priority (CGB only)
 */
void PPU::draw_sprites(const std::array<int, SCREEN_WIDTH>& bg_color_ids, const std::array<bool, SCREEN_WIDTH>& bg_high_priority) {
    // stored as height - 1 so boundary checks are lcd_y <= sprite_y + sprite_height
    int sprite_height = is_set(lcd_control, Bit::Bit2) ? 15: 7;

    // only 10 sprites allowed per scanline
    std::array<Address, 10> selected_sprites = select_sprites(sprite_height);

    struct SpritePixel {
        int color_id = 0;
        Byte x_coord = 0x00;
        bool priority = false;
        Byte palette = 0x00;
    };

    std::array<SpritePixel, SCREEN_WIDTH> sprite_buffer{};

    // loop over all the sprites, marking which pixel belongs to which sprite according to the
    //  relevant priority rules
    for (int i = 0; i < 10; i++) {
        // when you get to your first 0 address, there are no more sprites
        if (selected_sprites[i] == 0x0000) {
            break;
        }

        Address adjusted_sprite_address = selected_sprites[i] - OAM_START;

        // Cant use read during mode 3
        Byte sprite_y_pos = oam[adjusted_sprite_address];  // top edge of sprite + 16
        Byte sprite_x_pos = oam[adjusted_sprite_address + 1];  // left edge of sprite + 8
        Byte sprite_tile_index = oam[adjusted_sprite_address+ 2];
        Byte sprite_attributes = oam[adjusted_sprite_address + 3];

        bool priority = is_set(sprite_attributes, Bit::Bit7);
        bool flip_y = is_set(sprite_attributes, Bit::Bit6);
        bool flip_x = is_set(sprite_attributes, Bit::Bit5);

        int bank = is_set(sprite_attributes, Bit::Bit3) ? 1: 0;
        if (!cgb_mode) {
            bank = 0;
        }

        Byte palette = 0x00;
        if (cgb_mode) {
            palette = sprite_attributes & 0x07;
        } else {
            palette = is_set(sprite_attributes, Bit::Bit4) ? object_palette_1 : object_palette_0;
        }

        // if sprite is offscreen, skip it
        if ((sprite_x_pos == 0) || (sprite_x_pos >= 168)) {
            continue;
        }

        // get the relative row of the sprite  - [0,7] on an 8x8, and [0,15] on an 8x16
        int y = 16 - sprite_y_pos + lcd_y;  // translates to lcd_y - top edge of sprite

        if (flip_y) {
            y = sprite_height - y;
        }

        if (sprite_height == 15) {
            // mask the LSB to get an even index
            sprite_tile_index &= 0xFE;
            if (y >= 8) {
                sprite_tile_index += 1;
                y -= 8;
            }
        }

        // sprites only use 8000 addressing mode
        Address tile_address = TILE_DATA_0 + (16 * sprite_tile_index);
        int index = address_to_index(tile_address, bank);

        if (tile_cache[index].dirty) {
            refresh_tile(tile_address, bank);
        }

        // 1st pass - iterate over the sprite's 8 pixels in this line, as long as they're in [0,159]
        int pixel_column = -1;
        for (int j = 8; j > 0; j--) {
            pixel_column = sprite_x_pos - j;

            if (pixel_column >= SCREEN_WIDTH) {
                break;
            }

            if (pixel_column < 0) {
                continue;
            }

            // get the relative column of the sprite in [0,7]
            int x = (flip_x) ? (j - 1) : (8 - j);

            int color_id = tile_cache[index].pixels[(y * 8) + x];

            // if current pixel is already claimed, check claiming sprites priority vs current sprites
            if (color_id != 0) {
                if ((sprite_buffer[pixel_column].color_id != 0) &&
                    ((cgb_mode) || (sprite_buffer[pixel_column].x_coord <= sprite_x_pos)))
                {
                    continue;
                }
                sprite_buffer[pixel_column] = {color_id, sprite_x_pos, priority, palette};
            }
        }
    }

    // 2nd pass - set the pixels to the sprite that owns them's pixel value 
    for (int pixel_column = 0; pixel_column < SCREEN_WIDTH; pixel_column++) {
        int buffer_index = (lcd_y * SCREEN_WIDTH) + pixel_column;

        /**
         * CGB Priority rules
         * 
         * LCDC Bit 0 | OAM Attr Bit 7 | BG Attr Bit 7 | Priority
         * -----------|----------------|---------------|---------
         *       0    |        0       |        0      |   OBJ
         *       0    |        0       |        1      |   OBJ
         *       0    |        1       |        0      |   OBJ
         *       0    |        1       |        1      |   OBJ
         *       1    |        0       |        0      |   OBJ
         *       1    |        0       |        1      |   BG color 1-3, otherwise OBJ
         *       1    |        1       |        0      |   BG color 1-3, otherwise OBJ
         *       1    |        1       |        1      |   BG color 1-3, otherwise OBJ
         * 
         * Table source: https://gbdev.io/pandocs/Tile_Maps.html#bg-to-obj-priority-in-cgb-mode
         */
        bool bg_is_occupied = (bg_color_ids[pixel_column] != 0);
        bool bg_has_priority = (bg_high_priority[pixel_column]);
        bool oam_defers = sprite_buffer[pixel_column].priority;

        bool background_wins = false;
        if (cgb_mode) {
            background_wins = is_set(lcd_control, Bit::Bit0) && 
                                (bg_has_priority || oam_defers) && (bg_is_occupied);
        } else {
            background_wins = oam_defers && bg_is_occupied;
        }


        if ((sprite_buffer[pixel_column].color_id == 0) || 
            (background_wins))
        {
            continue;
        }
        

        frame_buffer[buffer_index] = color_id_to_argb(sprite_buffer[pixel_column].color_id, sprite_buffer[pixel_column].palette, true);
    }
}


/**
 * Given a tile ID, index into tile data to get the starting address of the 16 byte tile
 * 
 * @param tile_id a byte specifying a tile's location in memory, combined with one of two starting locations
 * @returns the starting address of a tile of 16 bytes
 */
Address PPU::get_tile_address(const Byte tile_id) const {
    if (is_set(lcd_control, Bit::Bit4)) {
        // 8000 method
        return (TILE_DATA_0 + (tile_id * 16));
    } else {
        // 8800 method
        int8_t signed_id = static_cast<int8_t>(tile_id);
        return static_cast<Address>(static_cast<int32_t>(TILE_DATA_1) + (signed_id * 16));
    }
}


int PPU::address_to_index(const Address tile_address, int bank) const {
    // bank 1 only enabled in CGB mode
    if (!cgb_mode) {
        bank = 0;
    }

    int index = static_cast<int>((tile_address - VRAM_START) >> 4);
    index += bank * TILES_PER_BANK;
    return index;
}


/**
 * Read two tile bytes and convert them to color ids, numbers in range [0,3]
 * 
 * @param low_byte the byte containing the 1s place of the color ids of the 8 pixels
 * @param high_byte the byte containing the 2s place of the color ids of the 8 pixels
 * @returns an array of 8 color ids
 */
std::array<int, 8> PPU::fetch_pixel_slice(const Byte low_byte, const Byte high_byte) const {
    std::bitset<8> high_bits(high_byte);
    std::bitset<8> low_bits(low_byte);

    std::array<int, 8> pixel_slice{};

    for (int i = 0; i < 8; i++) {
        int color_id = static_cast<int>((high_bits[7 - i] << 1) | (low_bits[7 - i]));
        pixel_slice[i] = color_id;
    }

    return pixel_slice;
}


/**
 * Converts a 16-byte tile into an array of 64 color ids, a flattened version of the 8x8 tile
 * 
 * @param tile_address the starting address of a tile of 16 bytes
 */
void PPU::refresh_tile(const Address tile_address, int bank) {
    if (!cgb_mode) {
        bank = 0;
    }

    std::array<int, 64> tile_pixels{};
    std::array<int, 8> pixel_row{};

    Byte high_byte = 0x00;
    Byte low_byte = 0x00;
    Address byte_address = 0x0000;


    uint32_t adjusted_address = tile_address - VRAM_START;
    adjusted_address += bank * VRAM_SIZE;
    for (int i = 0; i < 8; i++) {
        byte_address = adjusted_address + (2 * i);

        // can't use read during mode 3, which we are always in when this function is called
        low_byte = vram[byte_address];
        high_byte = vram[byte_address + 1];

        pixel_row = fetch_pixel_slice(low_byte, high_byte);

        std::copy_n(pixel_row.begin(), 8, tile_pixels.begin() + (8 * i));
    }

    int index = address_to_index(tile_address, bank);
    std::copy(tile_pixels.begin(), tile_pixels.end(), tile_cache[index].pixels.begin());
    tile_cache[index].dirty = false;
}


/**
 * Converts color ids to RGBA32, according to the current palette 
 * 
 * @param color_id a number in range [0,3]
 * @param palette byte that matches color ids in the range [0,3] to palette indices in the range[0,3]
 * @returns the RGBA32 color value associated with that color id
 */
RGBA32 PPU::color_id_to_argb(const int color_id, const Byte palette, const bool is_sprite) const {
    if ((color_id < 0) || (color_id >= 4)) {
        throw std::runtime_error("color id must be a value between 0 and 3 (inclusive)");
    }

    // DMG mode
    // TODO: come back and implement DMG compatibility palettes instead of using hardcoded one
    if (!cgb_mode) {
        int shade = (palette >> (color_id * 2)) & 0x03;
        return dmg_palette[shade];
    }

    int index = (palette * 8) + (color_id * 2);
    const std::array<Byte, 64>& ram = is_sprite ? object_color_ram : background_color_ram;

    Word color = ram[index + 1] << 8 | ram[index];

    Byte red = (color & 0x1F);
    Byte green = ((color >> 5) & 0x1F);
    Byte blue = ((color >> 10) & 0x1F);

    red = (red << 3) | (red >> 2);
    green = (green << 3) | (green >> 2);
    blue = (blue << 3) | (blue >> 2);

    return (red << 24) | (green << 16) | (blue << 8) | 0xFF;
}


std::array<Address, 10> PPU::select_sprites(const int sprite_height) {
    std::array<Address, 10> selected_sprites{};
    int index = 0;

    // reindex lcd_y to avoid dealing with possible underflow issues
    Byte adjusted_lcd_y = lcd_y + 16;

    for (int i = 0; i < 40; i++) {
        Address sprite_address = 4 * i;

        // can't use read during mode 3
        Byte sprite_y_pos = oam[sprite_address];   // top edge of sprite + 16

        // check if current scanline is between top and bottom of sprite
        if ((adjusted_lcd_y >= sprite_y_pos) && (adjusted_lcd_y <= (sprite_y_pos + sprite_height))) {
            selected_sprites[index] = sprite_address + OAM_START;
            index++;
        }

        if (index == 10) {
            break;
        }
    }

    return selected_sprites;
}


std::vector<Byte> PPU::dump_tiles() {
    std::vector<Byte> tile_data(0x3000);

    std::copy_n(vram.begin(), 0x1800, tile_data.begin());
    std::copy_n(vram.begin() + VRAM_SIZE, 0x1800, tile_data.begin() + 0x1800);

    // dump(tile_data, "../build/tile_data.txt");
    return tile_data;
}

void PPU::print_tiles(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::vector<Byte> tiles = dump_tiles();
    int num_tiles = tiles.size() / 16;

    for (int t = 0; t < num_tiles; t++) {
        file << "Tile " << t << " (bank " << (t >= TILES_PER_BANK ? 1 : 0) << "):\n";
        
        for (int row = 0; row < 8; row++) {
            Byte low  = tiles[t * 16 + row * 2];
            Byte high = tiles[t * 16 + row * 2 + 1];
            
            for (int col = 7; col >= 0; col--) {
                int color_id = (((high >> col) & 1) << 1) | ((low >> col) & 1);
                file << ".░▒█"[color_id];
            }
            file << "\n";
        }
        file << "\n";
    }
}

void PPU::dump_tiles_ppm(const std::string& filename) {
    std::vector<Byte> tiles = dump_tiles();
    int num_tiles = tiles.size() / 16;

    // lay tiles out in a grid, 16 tiles wide
    int tiles_wide = 16;
    int tiles_tall = (num_tiles + tiles_wide - 1) / tiles_wide;
    int img_width  = tiles_wide * 8;
    int img_height = tiles_tall * 8;

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // PPM header
    file << "P6\n" << img_width << " " << img_height << "\n255\n";

    for (int ty = 0; ty < tiles_tall; ty++) {
        for (int row = 0; row < 8; row++) {
            for (int tx = 0; tx < tiles_wide; tx++) {
                int t = ty * tiles_wide + tx;

                for (int col = 7; col >= 0; col--) {
                    if (t >= num_tiles) {
                        // pad with black if we're past the last tile
                        file.put(0); file.put(0); file.put(0);
                        continue;
                    }

                    Byte low  = tiles[t * 16 + row * 2];
                    Byte high = tiles[t * 16 + row * 2 + 1];
                    int color_id = (((high >> col) & 1) << 1) | ((low >> col) & 1);

                    // use palette 0 for background tiles, is_sprite=false
                    RGBA32 color = color_id_to_argb(color_id, 0, false);

                    // RGBA32 is (R << 24 | G << 16 | B << 8 | A)
                    file.put((color >> 24) & 0xFF); // R
                    file.put((color >> 16) & 0xFF); // G
                    file.put((color >>  8) & 0xFF); // B
                    // PPM has no alpha channel
                }
            }
        }
    }
}