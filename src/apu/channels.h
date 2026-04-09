#ifndef CHANNELS_H
#define CHANNELS_H

#include "../types.h"

static constexpr bool duty_table[4][8] = {
    {0, 0, 0, 0, 0, 0, 0, 1},   // 12.5%
    {0, 0, 0, 0, 0, 0, 1, 1},   // 25%
    {0, 0, 0, 0, 1, 1, 1, 1},   // 50%
    {1, 1, 1, 1, 1, 1, 0, 0}    // 75%
};

// channels 1 & 2
// NOTE: period dividers are clocked once per m-cycle - waveform is 8 samples long
struct PulseChannel {
    Byte sweep                   = 0x00;    // NR_10_REGISTER
    Byte timer_and_duty_cycle    = 0x00;    // NR_11_REGISTER and NR_21_REGISTER
    Byte volume_and_envelope     = 0x00;    // NR_12_REGISTER and NR_22_REGISTER
    Byte period_low              = 0x00;    // NR_13_REGISTER and NR_23_REGISTER
    Byte period_high_and_control = 0x00;    // NR_14_REGISTER and NR_24_REGISTER

    bool active                  = false;
    int length_timer             = 0;
    int duty_cycle               = 0;
    int duty_pos                 = 0;
    Word period_divider          = 0;   // ticks every 4 dots (1 m-cycle) NOT affected by double speed mode
    int current_volume           = 0;
    int envelope_timer           = 0;
    float output                 = 0.0f;

    // channel 1 only
    bool sweep_enabled           = false;
    int sweep_timer              = 0;
    Word shadow_period           = 0x0000;
};


struct WaveChannel {
    Byte DAC_enable              = 0x00;    // NR_31_REGISTER
    Byte initial_length_timer    = 0x00;    // NR_32_REGISTER
    Byte output_level            = 0x00;    // NR_33_REGISTER
    Byte period_low              = 0x00;    // NR_34_REGISTER
    Byte period_high_and_control = 0x00;    // NR_35_REGISTER

    bool active                  = false;

    // TODO: add more fields
};

struct NoiseChannel {
    Byte initial_length_timer    = 0x00;    // NR_41_REGISTER
    Byte volume_and_envelope     = 0x00;    // NR_42_REGISTER
    Byte freq_and_randomness     = 0x00;    // NR_43_REGISTER
    Byte control                 = 0x00;    // NR_44_REGISTER

    bool active                  = false;

    // TODO: add more fields
};


#endif // CHANNELS_H