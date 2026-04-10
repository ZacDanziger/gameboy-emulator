#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <functional>

// Types
using Byte = uint8_t;
using Word = uint16_t;
using Address = uint16_t;
using RGBA32 = uint32_t;


enum class Bit : Word {
    Bit0 = 0x0001,
    Bit1 = 0x0002,
    Bit2 = 0x0004,
    Bit3 = 0x0008,
    Bit4 = 0x0010,
    Bit5 = 0x0020,
    Bit6 = 0x0040,
    Bit7 = 0x0080,
    Bit8 = 0x0100,
    Bit9 = 0x0200,
    Bit10 = 0x0400,
    Bit11 = 0x0800,
    Bit12 = 0x1000,
    Bit13 = 0x2000,
    Bit14 = 0x4000,
    Bit15 = 0x8000
};


enum class Interrupt : Byte {
    VBlank  = 0b00000001, // PPU
    LCDStat = 0b00000010, // PPU
    Timer   = 0b00000100, // Timer
    Serial  = 0b00001000, // I/O Registers
    Joypad  = 0b00010000  // I/O Registers
};


enum class Key {
    A, B, Select, Start,    // button keys
    Right, Left, Up, Down,  // direction keys
    SAVE                    // save
};


enum class Mode {
    HBLANK,
    VBLANK,
    OAM_SCAN,
    TRANSFER
};


enum class MBC_Type : Byte{
    MBC0                   = 0x00,
    MBC1                   = 0x01,
    MBC1_RAM               = 0x02,
    MBC1_RAM_BATTERY       = 0x03,
    MBC3_TIMER_BATTERY     = 0x0F,
    MBC3_TIMER_RAM_BATTERY = 0x10,
    MBC3                   = 0x11,
    MBC3_RAM               = 0x12,
    MBC3_RAM_BATTERY       = 0x13
};

// Pixel counts of display screen and larger background map 
constexpr std::size_t SCREEN_WIDTH = 160;
constexpr std::size_t SCREEN_HEIGHT = 144;

constexpr Byte OPEN_BUS_VALUE = 0xFF;

using InterruptCallback = std::function<void(Interrupt)>;


#endif // TYPES_H