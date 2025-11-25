#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <cstdint>
#include <functional>

// Types
using Byte = uint8_t;
using Word = uint16_t;
using Address = uint16_t;

enum class Interrupt {
    VBlank = 0,
    LCDStat = 1,
    Timer = 2,
    Serial = 3,
    Joypad = 4
};

enum class Mode {
    HBLANK,
    VBLANK,
    OAM_SCAN,
    DRAW_PIXELS
};

using InterruptCallback = std::function<void(Interrupt)>;

template <std::size_t width, std::size_t height, std::size_t depth>
using Byte_Array3D = std::array<std::array<std::array<Byte, width>, height>, depth>;

#endif // INTERRUPT_H