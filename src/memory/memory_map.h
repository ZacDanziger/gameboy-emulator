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
const Address IO_START           = 0xFF00;
const Address HRAM_START         = 0xFF80;


const uint16_t ROM_BANK_SIZE     = 16 * KiB; 
const uint16_t VRAM_SIZE         = 8 * KiB;
const uint16_t ERAM_SIZE         = 8 * KiB;
const uint16_t WRAM_BANK_SIZE    = 4 * KiB;
const uint16_t IO_REG_SIZE       = 0x0080;
const uint16_t HRAM_SIZE         = 0x007F;


// Registers
const Address MBC_TYPE           = 0x0147;
const Address CART_ROM_SIZE      = 0x0148;
const Address CART_RAM_SIZE      = 0x0149;


const Address SB_REGISTER        = 0xFF01;  // serial transfer data
const Address SC_REGISTER        = 0xFF02;  // serial transfer control
const Address DIV_REGISTER       = 0xFF04;  // divider register
const Address TIMA_REGISTER      = 0xFF05;  // timer counter
const Address TMA_REGISTER       = 0xFF06;  // timer modulo
const Address TAC_REGISTER       = 0xFF07;  // timer control
const Address IF_REGISTER        = 0xFF0F;  // interrupt flag
const Address IE_REGISTER        = 0xFFFF;  // interrupt enable

#endif  // MEMORY_MAP_H