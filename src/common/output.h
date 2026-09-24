#ifndef OUTPUT_H
#define OUTPUT_H

#include <cstdint>
#include <array>

// audio sample frequency
constexpr float SAMPLES_PER_SECOND = 44100.0f;

using Pixel = uint32_t;

struct Frame {
    static constexpr std::size_t WIDTH = 160;
    static constexpr std::size_t HEIGHT = 144;

    std::array<Pixel, WIDTH * HEIGHT> pixels;
};

#endif  // OUTPUT_H