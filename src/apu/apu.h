#ifndef APU_H
#define APU_H

#include "../memory_map.h"
#include "../types.h"
#include "../utils/utils.h"
#include "channels.h"


// output sample rate
constexpr float CYCLES_PER_SECOND = 1048576.0f;
constexpr float SAMPLES_PER_SECOND = 44100.0f;
constexpr float CYCLES_PER_SAMPLE = CYCLES_PER_SECOND / SAMPLES_PER_SECOND;



/**
 * Audio Processing Unit
 * --- IN PROGRESS ---
 * 
 */
class APU {
    public:
        APU() :
            audio_buffer{},

            audio_master_control(0x77),
            sound_panning(0xF3),
            master_volume_and_vin_panning(0xF1),
            
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
        
        int div_apu_counter;

        float sample_accumulator;

        void clear_registers_and_channels();

        void push_sample();

        void tick_length_timers();
        void envelope_sweep();
};

#endif // APU_H