#include "ppu.h"

void PPU::render_frame() {
    //TODO: pass frame to SDL to render
}


Byte PPU::read(const Address address) const {
    if (address >= VRAM_START && address < ERAM_START) {
        if (mode == Mode::DRAW_PIXELS) {
            return 0xFF;
        }
        return vram[address - VRAM_START];
    }

    if (address >= OAM_START && address <= OAM_STOP) {
        if ((mode == Mode::OAM_SCAN) || (mode == Mode::DRAW_PIXELS)) {
            return 0xFF;
        }
        return oam[address - OAM_SIZE];
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
        return bgp;
    case OBP0_REGISTER:
        return object_palette_0;
    case OBP1_REGISTER:
        return object_palette_1;
    case WY_REGISTER:
        return window_y;
    case WX_REGISTER:
        return window_x;
    default:
        throw std::runtime_error("PPU read called on wrong address");
    }
}


void PPU::write(const Address address, const Byte value) {
    if (address >= VRAM_START && address < ERAM_START) {
        if (mode != Mode::DRAW_PIXELS) {
            vram[address - VRAM_START] = value;
        }
        return;
    }

    if (address >= OAM_START && address <= OAM_STOP) {
        if ((mode != Mode::OAM_SCAN) && (mode != Mode::DRAW_PIXELS)) {
            oam[address - OAM_SIZE] = value;
        }
        return;
    }

    std::bitset<8> lcdc(value);

    switch(address) {
    case LCDC_REGISTER:
        lcd_control = value;

        enabled = lcdc[7];
        window_tile_map = lcdc[6] ? TILE_MAP_1_START : TILE_MAP_0_START;
        window_enable = lcdc[5];
        tile_data = lcdc[4] ? TILE_DATA_0 : TILE_DATA_1;
        background_tile_map = lcdc[3] ? TILE_MAP_1_START : TILE_MAP_0_START;
        obj_size = lcdc[2];
        obj_enable = lcdc[1];
        background_window_enable = lcdc[0];

        break;
    case STAT_REGISTER:
        lcd_status = value;
        break;
    case SCY_REGISTER:
        viewport_y = value;
        break;
    case SCX_REGISTER:
        viewport_x = value;
        break;
    case LY_REGISTER:
        lcd_y = value;
        break;
    case LYC_REGISTER:
        ly_compare = value;
        break;
    case DMA_REGISTER:
        oam_dma = value;
        break;
    case BGP_REGISTER:
        bgp = value;
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
    default:
        throw std::runtime_error("PPU write called on wrong address");
    }
}