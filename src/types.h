#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <functional>

// Types
using Byte = uint8_t;
using Word = uint16_t;
using Address = uint16_t;
using RGBA32 = uint32_t;

enum class Interrupt : Byte {
    VBlank  = 0b00000001, // PPU
    LCDStat = 0b00000010, // PPU
    Timer   = 0b00000100, // Timer
    Serial  = 0b00001000, // I/O Registers
    Joypad  = 0b00010000  // I/O Registers
};

enum class Bit : Byte {
    Bit0 = 0b00000001,
    Bit1 = 0b00000010,
    Bit2 = 0b00000100,
    Bit3 = 0b00001000,
    Bit4 = 0b00010000,
    Bit5 = 0b00100000,
    Bit6 = 0b01000000,
    Bit7 = 0b10000000
};

enum class Mode {
    HBLANK,
    VBLANK,
    OAM_SCAN,
    TRANSFER
};

// Pixel counts of display screen and larger background map 
constexpr std::size_t SCREEN_WIDTH = 160;
constexpr std::size_t SCREEN_HEIGHT = 144;

using InterruptCallback = std::function<void(Interrupt)>;

#endif // TYPES_H