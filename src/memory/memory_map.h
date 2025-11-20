#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <cstdint>

// Types
typedef uint8_t Byte;
typedef uint16_t Word, Address;

const uint16_t KiB = 0x0400;

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

const Address ROM_BANK_00_START  = 0x0000;
const Address ROM_BANK_NN_START  = 0x4000;
const Address VRAM_START         = 0x8000;
const Address ERAM_START         = 0xA000;
const Address WRAM_BANK_00_START = 0xC000;
const Address WRAM_BANK_NN_START = 0xD000;
const Address ECHO_START         = 0xE000;
const Address OAM_START          = 0xFE00;
const Address OAM_STOP           = 0xFE9F;
const Address IO_START           = 0xFF00;
const Address HRAM_START         = 0xFF80;


const uint16_t ROM_BANK_SIZE     = 16 * KiB; 
const uint16_t VRAM_SIZE         = 8 * KiB;
const uint16_t ERAM_SIZE         = 8 * KiB;
const uint16_t WRAM_BANK_SIZE    = 4 * KiB;
const uint16_t ECHO_RAM_SIZE     = 0x1E00;
const uint16_t OAM_SIZE          = 0x00A0;
const uint16_t IO_REG_SIZE       = 0x0080;
const uint16_t HRAM_SIZE         = 0x007F;


// Registers
const Address MBC_TYPE           = 0x0147;
const Address CART_ROM_SIZE      = 0x0148;
const Address CART_RAM_SIZE      = 0x0149;

const Address JOYP_REGISTER      = 0xFF00;  // Joypad
const Address SB_REGISTER        = 0xFF01;  // Serial transfer data
const Address SC_REGISTER        = 0xFF02;  // Serial transfer control
const Address DIV_REGISTER       = 0xFF04;  // Divider register
const Address TIMA_REGISTER      = 0xFF05;  // Timer counter
const Address TMA_REGISTER       = 0xFF06;  // Timer modulo
const Address TAC_REGISTER       = 0xFF07;  // Timer control
const Address IF_REGISTER        = 0xFF0F;  // Interrupt flag

const Address NR10_REGISTER      = 0xFF10;  // Sound channel 1 sweep
const Address NR11_REGISTER      = 0xFF11;  // Sound channel 1 length timer & duty cycle
const Address NR12_REGISTER      = 0xFF12;  // Sound channel 1 volume & envelope
const Address NR13_REGISTER      = 0xFF13;  // Sound channel 1 period low
const Address NR14_REGISTER      = 0xFF14;  // Sound channel 1 period high & control

const Address NR21_REGISTER      = 0xFF16;  // Sound channel 2 length timer & duty cycle
const Address NR22_REGISTER      = 0xFF17;  // Sound channel 2 volume & envelope
const Address NR23_REGISTER      = 0xFF18;  // Sound channel 2 period low
const Address NR24_REGISTER      = 0xFF19;  // Sound channel 2 period high & control

const Address NR30_REGISTER      = 0xFF1A;  // Sound channel 3 DAC enable
const Address NR31_REGISTER      = 0xFF1B;  // Sound channel 3 length timer
const Address NR32_REGISTER      = 0xFF1C;  // Sound channel 3 output level
const Address NR33_REGISTER      = 0xFF1D;  // Sound channel 3 period low
const Address NR34_REGISTER      = 0xFF1E;  // Sound channel 3 period high & control

const Address NR41_REGISTER      = 0xFF20;  // Sound channel 4 length timer
const Address NR42_REGISTER      = 0xFF21;  // Sound channel 4 volume & envelope
const Address NR43_REGISTER      = 0xFF22;  // Sound channel 4 frequency & randomness
const Address NR44_REGISTER      = 0xFF23;  // Sound channel 4 control

const Address NR50_REGISTER      = 0xFF24;  // Master volume & VIN panning
const Address NR51_REGISTER      = 0xFF25;  // Sound panning
const Address NR52_REGISTER      = 0xFF26;  // Sound on/off

// WAVE RAM: 0xFF30 - 0xFF3F

const Address LCDC_REGISTER      = 0xFF40;  // LCD Control
const Address STAT_REGISTER      = 0xFF41;  // LCD Status
const Address SCY_REGISTER       = 0xFF42;  // Viewport Y Position
const Address SCX_REGISTER       = 0xFF43;  // Viewport X Position
const Address LY_REGISTER        = 0xFF44;  // LCD Y Coordinate
const Address LYC_REGISTER       = 0xFF45;  // LY Compare
const Address DMA_REGISTER       = 0xFF46;  // OAM DMA source address & start
const Address BGP_REGISTER       = 0xFF47;  // BGP palette data
const Address OBP0_REGISTER      = 0xFF48;  // OBJ palette 0 data
const Address OBP1_REGISTER      = 0xFF49;  // OBJ palette 1 data
const Address WY_REGISTER        = 0xFF4A;  // Window Y position
const Address WX_REGISTER        = 0xFF4B;  // Window X position

const Address KEY0_SYS_REGISTER  = 0xFF4C;  // CPU mode select
const Address KEY1_SPD_REGISTER  = 0xFF4D;  // Prepare speed switch

const Address VBK_REGISTER       = 0xFF4F;  // VRAM Bank (only bit 0 matters)
const Address BANK_REGISTER      = 0xFF50;  // Boot ROM mapping control

const Address HDMA1_REGISTER     = 0xFF51;  // VRAM DMA source high
const Address HDMA2_REGISTER     = 0xFF52;  // VRAM DMA source low
const Address HDMA3_REGISTER     = 0xFF53;  // VRAM DMA destination high
const Address HDMA4_REGISTER     = 0xFF54;  // VRAM DMA destination low
const Address HDMA5_REGISTER     = 0xFF55;  // VRAM DMA length/mode/start

const Address RP_REGISTER        = 0xFF56;  // Infrared communications port

const Address BCPS_BGPI_REGISTER = 0xFF68;  // Background color palette specification / Background palette index
const Address BCPD_BGPD_REGISTER = 0xFF69;  // Background color palette data / Background palette data
const Address OCPS_OGPI_REGISTER = 0xFF6A;  // OBJ color palette specification / OBJ palette index
const Address OCPD_OGPD_REGISTER = 0xFF6B;  // OBJ color palette data / OBJ palette data
const Address OPRI_REGISTER      = 0xFF6C;  // Object priority mode

const Address SVBK_WBK_REGISTER  = 0xFF70;  // WRAM bank

const Address PCM12_REGISTER     = 0xFF76;  // Audio digital outputs 1 & 2
const Address PCM34_REGISTER     = 0xFF77;  // Audio digital outputs 3 & 4

const Address IE_REGISTER        = 0xFFFF;  // Interrupt enable

#endif  // MEMORY_MAP_H