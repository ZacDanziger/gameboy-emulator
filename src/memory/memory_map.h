#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include "../types.h"

constexpr uint16_t KiB = 0x0400;

/*
Memory Map and Sizes
--------------------------------------------------
ROM bank 00------------------------0x0000 - 0x3FFF (16 KiB)
ROM banks 01-NN--------------------0x4000 - 0x7FFF (16 KiB)
Video RAM--------------------------0x8000 - 0x9FFF (8 KiB)
External RAM-----------------------0xA000 - 0xBFFF (8 KiB)
Work RAM---------------------------0xC000 - 0xCFFF (4 KiB)
Switchable Banks 1-7---------------0xD000 - 0xDFFF (4 KiB)
Echo RAM---------------------------0xE000 - 0xFDFF (7680 Bytes)
Object Attribute Memory (OAM)------0xFE00 - 0xFE9F (160 Bytes)
NOT USABLE!!!----------------------0xFEA0 - 0xFEFF (96 Bytes)
I/O Registers----------------------0xFF00 - 0xFF7F (128 Bytes)
High RAM---------------------------0xFF80 - 0xFFFE (127 Bytes)
Interrupt Enable Register----------0xFFFF          (1 Byte)
*/

constexpr Address ROM_BANK_00_START  = 0x0000;
constexpr Address ROM_BANK_NN_START  = 0x4000;
constexpr Address VRAM_START         = 0x8000;
constexpr Address ERAM_START         = 0xA000;
constexpr Address WRAM_BANK_00_START = 0xC000;
constexpr Address WRAM_BANK_NN_START = 0xD000;
constexpr Address ECHO_START         = 0xE000;
constexpr Address OAM_START          = 0xFE00;
constexpr Address NOT_USABLE_START   = 0xFEA0;
constexpr Address IO_START           = 0xFF00;
constexpr Address HRAM_START         = 0xFF80;


constexpr uint16_t ROM_BANK_SIZE     = 16 * KiB; 
constexpr uint16_t VRAM_SIZE         = 8 * KiB;
constexpr uint16_t ERAM_BANK_SIZE    = 8 * KiB;
constexpr uint16_t WRAM_BANK_SIZE    = 4 * KiB;
constexpr uint16_t ECHO_RAM_SIZE     = 0x1E00;
constexpr uint16_t OAM_SIZE          = 0x00A0;
constexpr uint16_t IO_REG_SIZE       = 0x0080;
constexpr uint16_t HRAM_SIZE         = 0x007F;

/**
 * Tile data--------0x8000 - 0x97FF
 * |- Block 0-------0x8000 - 0x87FF (8000 method)(For Objects)
 * |- Block 1-------0x8800 - 0x8FFF (Shared / both methods)
 * |- Block 2-------0x9000 - 0x97FF (8800 method)
 * 
 * Tile map 1-------0x9800 - 0x9BFF (32 x 32 tiles)
 * Tile map 2-------0x9C00 - 0x9FFF (32 x 32 tiles)
 */

constexpr Address TILE_DATA_0      = 0x8000;
constexpr Address TILE_DATA_1      = 0x9000;
constexpr Address TILE_MAP_0_START = 0x9800;
constexpr Address TILE_MAP_1_START = 0x9C00;

// Cartrige Header
constexpr Address TITLE_START        = 0x0134;
constexpr Address TITLE_END          = 0x0143;

constexpr Address CGB_FLAG           = 0x0143;  // in CGB cartridges, specifies if game is DMG compatible

constexpr Address MBC_TYPE           = 0x0147;
constexpr Address CART_ROM_SIZE      = 0x0148;
constexpr Address CART_RAM_SIZE      = 0x0149;

constexpr Address HEADER_CHECKSUM    = 0x014D;


// Registers
constexpr Address JOYP_REGISTER      = 0xFF00;  // Joypad
constexpr Address SB_REGISTER        = 0xFF01;  // Serial transfer data
constexpr Address SC_REGISTER        = 0xFF02;  // Serial transfer control
constexpr Address DIV_REGISTER       = 0xFF04;  // Divider register
constexpr Address TIMA_REGISTER      = 0xFF05;  // Timer counter
constexpr Address TMA_REGISTER       = 0xFF06;  // Timer modulo
constexpr Address TAC_REGISTER       = 0xFF07;  // Timer control
constexpr Address IF_REGISTER        = 0xFF0F;  // Interrupt flag

constexpr Address NR10_REGISTER      = 0xFF10;  // Sound channel 1 sweep
constexpr Address NR11_REGISTER      = 0xFF11;  // Sound channel 1 length timer & duty cycle
constexpr Address NR12_REGISTER      = 0xFF12;  // Sound channel 1 volume & envelope
constexpr Address NR13_REGISTER      = 0xFF13;  // Sound channel 1 period low
constexpr Address NR14_REGISTER      = 0xFF14;  // Sound channel 1 period high & control

constexpr Address NR21_REGISTER      = 0xFF16;  // Sound channel 2 length timer & duty cycle
constexpr Address NR22_REGISTER      = 0xFF17;  // Sound channel 2 volume & envelope
constexpr Address NR23_REGISTER      = 0xFF18;  // Sound channel 2 period low
constexpr Address NR24_REGISTER      = 0xFF19;  // Sound channel 2 period high & control

constexpr Address NR30_REGISTER      = 0xFF1A;  // Sound channel 3 DAC enable
constexpr Address NR31_REGISTER      = 0xFF1B;  // Sound channel 3 length timer
constexpr Address NR32_REGISTER      = 0xFF1C;  // Sound channel 3 output level
constexpr Address NR33_REGISTER      = 0xFF1D;  // Sound channel 3 period low
constexpr Address NR34_REGISTER      = 0xFF1E;  // Sound channel 3 period high & control

constexpr Address NR41_REGISTER      = 0xFF20;  // Sound channel 4 length timer
constexpr Address NR42_REGISTER      = 0xFF21;  // Sound channel 4 volume & envelope
constexpr Address NR43_REGISTER      = 0xFF22;  // Sound channel 4 frequency & randomness
constexpr Address NR44_REGISTER      = 0xFF23;  // Sound channel 4 control

constexpr Address NR50_REGISTER      = 0xFF24;  // Master volume & VIN panning
constexpr Address NR51_REGISTER      = 0xFF25;  // Sound panning
constexpr Address NR52_REGISTER      = 0xFF26;  // Sound on/off

// WAVE RAM: 0xFF30 - 0xFF3F

constexpr Address LCDC_REGISTER      = 0xFF40;  // LCD Control
constexpr Address STAT_REGISTER      = 0xFF41;  // LCD Status
constexpr Address SCY_REGISTER       = 0xFF42;  // Viewport Y Position
constexpr Address SCX_REGISTER       = 0xFF43;  // Viewport X Position
constexpr Address LY_REGISTER        = 0xFF44;  // LCD Y Coordinate
constexpr Address LYC_REGISTER       = 0xFF45;  // LY Compare
constexpr Address DMA_REGISTER       = 0xFF46;  // OAM DMA source address & start
constexpr Address BGP_REGISTER       = 0xFF47;  // BGP palette data
constexpr Address OBP0_REGISTER      = 0xFF48;  // OBJ palette 0 data
constexpr Address OBP1_REGISTER      = 0xFF49;  // OBJ palette 1 data
constexpr Address WY_REGISTER        = 0xFF4A;  // Window Y position
constexpr Address WX_REGISTER        = 0xFF4B;  // Window X position

constexpr Address KEY0_SYS_REGISTER  = 0xFF4C;  // CPU mode select
constexpr Address KEY1_SPD_REGISTER  = 0xFF4D;  // Prepare speed switch

constexpr Address VBK_REGISTER       = 0xFF4F;  // VRAM Bank (only bit 0 matters)
constexpr Address BANK_REGISTER      = 0xFF50;  // Boot ROM mapping control

constexpr Address HDMA1_REGISTER     = 0xFF51;  // VRAM DMA source high
constexpr Address HDMA2_REGISTER     = 0xFF52;  // VRAM DMA source low
constexpr Address HDMA3_REGISTER     = 0xFF53;  // VRAM DMA destination high
constexpr Address HDMA4_REGISTER     = 0xFF54;  // VRAM DMA destination low
constexpr Address HDMA5_REGISTER     = 0xFF55;  // VRAM DMA length/mode/start

constexpr Address RP_REGISTER        = 0xFF56;  // Infrared communications port

constexpr Address BCPS_BGPI_REGISTER = 0xFF68;  // Background color palette specification / Background palette index
constexpr Address BCPD_BGPD_REGISTER = 0xFF69;  // Background color palette data / Background palette data
constexpr Address OCPS_OGPI_REGISTER = 0xFF6A;  // OBJ color palette specification / OBJ palette index
constexpr Address OCPD_OGPD_REGISTER = 0xFF6B;  // OBJ color palette data / OBJ palette data
constexpr Address OPRI_REGISTER      = 0xFF6C;  // Object priority mode

constexpr Address SVBK_WBK_REGISTER  = 0xFF70;  // WRAM bank

constexpr Address PCM12_REGISTER     = 0xFF76;  // Audio digital outputs 1 & 2
constexpr Address PCM34_REGISTER     = 0xFF77;  // Audio digital outputs 3 & 4

constexpr Address IE_REGISTER        = 0xFFFF;  // Interrupt enable

#endif  // MEMORY_MAP_H