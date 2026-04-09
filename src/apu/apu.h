#ifndef APU_H
#define APU_H

#include "../memory_map.h"
#include "../types.h"
#include "../utils/utils.h"
#include "channels.h"

#include <cmath>

// output sample rate
constexpr float CYCLES_PER_SECOND = 1048576.0f;
constexpr float SAMPLES_PER_SECOND = 44100.0f;
constexpr float CYCLES_PER_SAMPLE = CYCLES_PER_SECOND / SAMPLES_PER_SECOND;

/**
 * Audio Processing Unit
 * --- IN PROGRESS ---
 * 
 * Considerations - How does SDL3 expect audio input? start there, and work backwards 
 * should audio_buffer be a vector? if no, how long an array should it be?
 * 
 * TODO: get channel 2 working
 */
class APU {
    public:
        APU() :
            audio_buffer{},
            wave_ram{},

            audio_master_control(0x00),
            sound_panning(0x00),
            master_volume_and_vin_panning(0x00),
            
            channel_1(),
            channel_2(),
            channel_3(),
            channel_4(),

            div_apu_counter(0),
            sample_accumulator(0)
        {}

        void reset();

        Byte read(const Address address) const;
        void write(const Address address, const Byte data);

        void update();
        void div_apu_tick();

        std::vector<float> flush_audio_buffer();
        bool audio_enabled() const { return is_set(audio_master_control, Bit::Bit7); }
    private:
        std::vector<float> audio_buffer;
        std::array<Byte, 16> wave_ram;           // 0xFF30 - 0xFF3F
        
        Byte audio_master_control;               // NR_52_REGISTER
        Byte sound_panning;                      // NR_51_REGISTER
        Byte master_volume_and_vin_panning;      // NR_50_REGISTER
        
        PulseChannel channel_1;
        PulseChannel channel_2;
        WaveChannel channel_3;
        NoiseChannel channel_4;
        
        int div_apu_counter;

        float sample_accumulator;

        void clear_registers_and_channels();

        void trigger_channel_2();

        void channel_2_output();

        int mix();

        void push_sample();

        void tick_length_timers();
        void channel_1_freq_sweep();
        void envelope_sweep();
};

#endif // APU_H