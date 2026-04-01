#ifndef APU_H
#define APU_H

#include "../utils/utils.h"
#include "../memory/memory_map.h"

#include <cmath>    // for eventual use of pow()

constexpr int SAMPLE_FREQUENCY = 44100;

/**
 * Audio Processing Unit
 * --- IN PROGRESS ---
 * 
 * Considerations - How does SDL3 expect audio input? start there, and work backwards 
 * should audio_buffer be a vector? if no, how long an array should it be?
 */
class APU {
    public:
        APU() :
            audio_buffer{},
            wave_ram{},
            
            div_apu_counter(0)
        {
            clear_registers();
        }

        Byte read(const Address address) const;
        void write(const Address address, const Byte data);

        void tick();

        const std::vector<int16_t>& flush_audio_buffer() const { return audio_buffer; }
    private:
        std::vector<int16_t> audio_buffer;

        std::array<Byte, 16> wave_ram;       // 0xFF30 - 0xFF3F

        Byte audio_master_control;           // NR_52_REGISTER
        Byte sound_panning;                  // NR_51_REGISTER
        Byte master_volume_and_vin_panning;  // NR_50_REGISTER

        Byte channel_1_sweep;                // NR_10_REGISTER
        Byte channel_1_timer_and_duty_cycle; // NR_11_REGISTER
        Byte channel_1_volume_and_envelope;  // NR_12_REGISTER
        Byte channel_1_period_low;           // NR_13_REGISTER
        Byte channel_1_period_high;          // NR_14_REGISTER

        Byte channel_2_timer_and_duty_cycle; // NR_21_REGISTER
        Byte channel_2_volume_and_envelope;  // NR_22_REGISTER
        Byte channel_2_period_low;           // NR_23_REGISTER
        Byte channel_2_period_high;          // NR_24_REGISTER

        Byte channel_3_DAC_enable;           // NR_30_REGISTER
        Byte channel_3_length_timer;         // NR_31_REGISTER
        Byte channel_3_output_level;         // NR_32_REGISTER
        Byte channel_3_period_low;           // NR_33_REGISTER
        Byte channel_3_period_high;          // NR_34_REGISTER

        Byte channel_4_length_timer;         // NR_41_REGISTER
        Byte channel_4_volume_and_envelope;  // NR_42_REGISTER
        Byte channel_4_freq_and_randomness;  // NR_43_REGISTER
        Byte channel_4_control;              // NR_44_REGISTER

        int div_apu_counter;

        void clear_registers();
        inline bool audio_enabled() const { return is_set(audio_master_control, Bit::Bit7); }
};

#endif // APU_H