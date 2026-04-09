#include "apu.h"

/**
 * Reset the APU to its post Boot ROM state
 */
void APU::reset() {
    audio_buffer = {0};
    wave_ram = {0};

    div_apu_counter = 0;
    sample_accumulator = 0;
    
    clear_registers_and_channels();
}


// TODO: account for write-only bits in some registers
Byte APU::read(const Address address) const {
    if (address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
        return wave_ram[address - WAVE_RAM_START];
    }

    switch(address) {
    case NR10_REGISTER :
        return channel_1.sweep;
    case NR11_REGISTER :
        return channel_1.timer_and_duty_cycle & 0xC0;
    case NR12_REGISTER :
        return channel_1.volume_and_envelope;
    case NR13_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR14_REGISTER :
        // only bit 6 is read-able
        return channel_1.period_high_and_control & 0x40;

    case NR21_REGISTER :
        return channel_2.timer_and_duty_cycle & 0xC0;
    case NR22_REGISTER :
        return channel_2.volume_and_envelope;
    case NR23_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR24_REGISTER :
        // only bit 6 is read-able
        return channel_2.period_high_and_control & 0x40;

    case NR30_REGISTER :
        return channel_3.DAC_enable;
    case NR31_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR32_REGISTER :
        return channel_3.output_level;
    case NR33_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR34_REGISTER :
        return channel_3.period_high_and_control;

    case NR41_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR42_REGISTER :
        return channel_4.volume_and_envelope;
    case NR43_REGISTER :
        return channel_4.freq_and_randomness;
    case NR44_REGISTER :
        return channel_4.control;

    case NR50_REGISTER :
        return master_volume_and_vin_panning;
    case NR51_REGISTER :
        return sound_panning;
    case NR52_REGISTER :
        return audio_master_control;
    // case PCM12_REGISTER:
    //     // TODO: change to actual value
    //     return OPEN_BUS_VALUE;
    // case PCM34_REGISTER:
    //     // TODO: change to actual value
    //     return OPEN_BUS_VALUE;
    default:
        throw std::runtime_error("APU read called on wrong address");
    }
}


void APU::write(const Address address, const Byte data) {
    if (address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
        wave_ram[address - WAVE_RAM_START] = data;
        return;
    }

    // if audio is disabled, only audio master control register is allowed to be written to
    if (!audio_enabled() && address != NR52_REGISTER) {
        return;
    }

    switch(address) {
    case NR10_REGISTER :
        channel_1.sweep = data;
        return;
    case NR11_REGISTER :
        channel_1.timer_and_duty_cycle = data;
        return;
    case NR12_REGISTER :
        channel_1.volume_and_envelope = data;
        return;
    case NR13_REGISTER :
        channel_1.period_low = data;
        return;
    case NR14_REGISTER :
        channel_1.period_high_and_control = data;
        return;

    case NR21_REGISTER :
        channel_2.timer_and_duty_cycle = data;
        return;
    case NR22_REGISTER :
        channel_2.volume_and_envelope = data;
        return;
    case NR23_REGISTER :
        channel_2.period_low = data;
        return;
    case NR24_REGISTER :
        channel_2.period_high_and_control = data;
        if (is_set(data, Bit::Bit7)) {
            trigger_channel_2();
        }
        return;

    case NR30_REGISTER :
        channel_3.DAC_enable = data;
        return;
    case NR31_REGISTER :
        channel_3.initial_length_timer = data;
        return;
    case NR32_REGISTER :
        channel_3.output_level = data;
        return;
    case NR33_REGISTER :
        channel_3.period_low = data;
        return;
    case NR34_REGISTER :
        channel_3.period_high_and_control = data;
        return;

    case NR41_REGISTER :
        channel_4.initial_length_timer = data;
        return;
    case NR42_REGISTER :
        channel_4.volume_and_envelope = data;
        return;
    case NR43_REGISTER :
        channel_4.freq_and_randomness = data;
        return;
    case NR44_REGISTER :
        channel_4.control = data;
        return;

    case NR50_REGISTER :
        master_volume_and_vin_panning = data;
        return;
    case NR51_REGISTER :
        sound_panning = data;
        return;
    case NR52_REGISTER :
        if (!is_set(data, Bit::Bit7)) {
            clear_registers_and_channels();
        } else {
            set_bit(audio_master_control, Bit::Bit7);
        }
        return;
    // case PCM12_REGISTER:
    //     // read-only
    //     return;
    // case PCM34_REGISTER:
    //     // read-only
    //     return;
    default:
        throw std::runtime_error("APU write called on wrong address");
    }
}


// step the APU forward 1 m-cycle
void APU::update() {
    //
    if (channel_2.active) {
        channel_2.period_divider += 1;
        if (channel_2.period_divider == 0x0800) {
            channel_2.period_divider = ((channel_2.period_high_and_control & 0x07) << 8) | channel_2.period_low;

            channel_2_output();
        }
    }

    sample_accumulator += 1.0f;
    if (sample_accumulator >= CYCLES_PER_SAMPLE) {
        sample_accumulator -= CYCLES_PER_SAMPLE;

        push_sample();
    }
}


/**
 * TODO: add pace counters
 */
void APU::div_apu_tick() {
    div_apu_counter += 1;

    // sound length ticks up - 256 Hz
    if (div_apu_counter % 2 == 0) {
       tick_length_timers();
    }

    // channel 1 frequency sweep - 128 Hz
    if (div_apu_counter % 4 == 0) {
        // channel_1_freq_sweep();
    }

    // envelope sweep - 64 Hz
    if (div_apu_counter % 8 == 0) {
        // envelope_sweep();
    }
}


std::vector<float> APU::flush_audio_buffer() {
    std::vector<float> buffer_copy = std::move(audio_buffer);
    audio_buffer.clear();

    return buffer_copy;
}

/**
 * Write 0x00 to all APU registers
 * 
 * NOTE: duty step timer cannot be reset - figure out how to implement
 */
void APU::clear_registers_and_channels() {
    audio_master_control = 0x00;
    sound_panning = 0x00;
    master_volume_and_vin_panning = 0x00;
    
    channel_1 = PulseChannel{};
    channel_2 = PulseChannel{};
    channel_3 = WaveChannel{};
    channel_4 = NoiseChannel{};
}


void APU::trigger_channel_2() {
    channel_2.active = true;
    channel_2.length_timer = channel_2.timer_and_duty_cycle & 0x3F;
    channel_2.duty_cycle = (channel_2.timer_and_duty_cycle & 0xC0) >> 6;

    channel_2.current_volume = (channel_2.volume_and_envelope & 0xF0) >> 4;
    channel_2.period_divider = ((channel_2.period_high_and_control & 0x07) << 8) | channel_2.period_low;

    set_bit(audio_master_control, Bit::Bit1);
}


void APU::channel_2_output() {
    bool high = duty_table[channel_2.duty_cycle][channel_2.duty_pos % 8];
    channel_2.duty_pos += 1;

    Byte sample =  high ? channel_2.current_volume : 0x00; // in range [0x00, 0x0F]

    channel_2.output = ((sample / 15.0f) * 2.0) - 1.0;  // in range [-1.0, 1.0]
}


void APU::push_sample() {
    float left_sample = 0.0;
    float right_sample = 0.0;

    // mix
    left_sample += is_set(sound_panning, Bit::Bit5) ? channel_2.output : 0;
    right_sample += is_set(sound_panning, Bit::Bit1) ? channel_2.output : 0;

    // master volume
    int left_volume = ((master_volume_and_vin_panning & 0x70) >> 4) + 1;
    int right_volume = (master_volume_and_vin_panning & 0x07) + 1;

    left_sample *= left_volume;
    right_sample *= right_volume;

    // TODO: hpf

    // push to buffer


    audio_buffer.push_back(left_sample);
    audio_buffer.push_back(right_sample);
}


void APU::tick_length_timers() {
    if (channel_2.active && is_set(channel_2.period_high_and_control, Bit::Bit6)) {
        channel_2.length_timer += 1;
        if (channel_2.length_timer == 64) {
            channel_2.active = false;
            reset_bit(audio_master_control, Bit::Bit1);
        }
    }
}