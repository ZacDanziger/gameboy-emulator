#ifndef APU_H
#define APU_H

#include "../memory/memory_map.h"
#include "../types.h"
#include "../utils/bit_utils.h"
#include "channels.h"
#include "../common/output.h"

#include <iostream>

// output sample rate
constexpr float CYCLES_PER_SAMPLE = CYCLES_PER_SECOND / SAMPLES_PER_SECOND;

constexpr float HPF_CHARGE_FACTOR = 0.996f;

/**
 * Audio Processing Unit
 */
class APU {
    public:
        APU() :
            audio_buffer{},

            audio_master_control(0x80),
            sound_panning(0xF3),
            master_volume_and_vin_panning(0x77),
            
            channel_1(),
            channel_2(),
            channel_3(),
            channel_4(),

            frame_sequencer_step(0),

            sample_count(0),
            sample_accumulator(0.0f),

            left_accumulator(0.0f),
            right_accumulator(0.0f),

            hpf_capacitor_left(0.0f),
            hpf_capacitor_right(0.0f)
        {}

        void reset();

        Byte read(const Address address) const;
        void write(const Address address, const Byte data);

        void update();
        void frame_sequencer();

        std::vector<float> flush_audio_buffer();
        bool audio_enabled() const { return is_set(audio_master_control, Bit::Bit7); }

        void set_cgb_mode(const bool cgb) { channel_3.set_cgb_mode(cgb); }
    private:
        std::vector<float> audio_buffer;
        
        Byte audio_master_control;               // NR_52_REGISTER
        Byte sound_panning;                      // NR_51_REGISTER
        Byte master_volume_and_vin_panning;      // NR_50_REGISTER
        
        Channel1 channel_1;
        Channel2 channel_2;
        Channel3 channel_3;
        Channel4 channel_4;
        
        int frame_sequencer_step;

        int sample_count;
        float sample_accumulator;

        float left_accumulator;
        float right_accumulator;

        float hpf_capacitor_left;
        float hpf_capacitor_right;

        void clear_registers_and_channels();

        void push_sample();

        void tick_length_timers();
        void envelope_sweep();

        void trigger_logic(Channel* channel, const Address address, const Byte data);
};

#endif // APU_H