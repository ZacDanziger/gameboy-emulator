#ifndef APU_H
#define APU_H

#include "../memory_map.h"
#include "../types.h"
#include "../utils/utils.h"
#include "channels.h"

#include <cmath>

constexpr int SAMPLE_FREQUENCY = 44100;

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

            div_apu_counter(0)
        {}

        void reset();

        Byte read(const Address address) const;
        void write(const Address address, const Byte data);

        void update();
        void div_apu_tick();

        const std::vector<int16_t>& flush_audio_buffer() const { return audio_buffer; }
    private:
        std::vector<int16_t> audio_buffer;
        std::array<Byte, 16> wave_ram;           // 0xFF30 - 0xFF3F
        
        Byte audio_master_control;               // NR_52_REGISTER
        Byte sound_panning;                      // NR_51_REGISTER
        Byte master_volume_and_vin_panning;      // NR_50_REGISTER
        
        PulseChannel channel_1;
        PulseChannel channel_2;
        WaveChannel channel_3;
        NoiseChannel channel_4;
        
        int div_apu_counter;

        inline bool audio_enabled() const { return is_set(audio_master_control, Bit::Bit7); }

        void clear_registers_and_channels();

        void trigger_channel_1();
        void trigger_channel_2();
        void trigger_channel_3();
        void trigger_channel_4();

        Byte channel_1_output();
        Byte channel_2_output();
        Byte channel_3_output();
        Byte channel_4_output();

        int mix();

        void tick_length_timers();
        void channel_1_freq_sweep();
        void envelope_sweep();
};

#endif // APU_H