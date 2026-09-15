#ifndef VIDEO_H
#define VIDEO_H

#include <cstdint>
#include <array>

using Pixel = uint32_t;

struct Frame {
    static constexpr std::size_t width = 160;
    static constexpr std::size_t height = 144;

    std::array<Pixel, width * height> pixels;
};

// audio sample frequency
constexpr float SAMPLES_PER_SECOND = 44100.0f;

#endif  // VIDEO_H