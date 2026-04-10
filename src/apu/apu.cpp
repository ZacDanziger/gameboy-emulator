#include "apu.h"

/**
 * Reset the APU to its post Boot ROM state
 */
void APU::reset() {
    audio_buffer = {0};

    div_apu_counter = 0;
    sample_accumulator = 0;
    
    channel_1.reset();
    channel_2.reset();
    channel_3.reset();
    channel_4.reset();

    master_volume_and_vin_panning = 0x77;
    sound_panning = 0xF3;
    audio_master_control = 0xF1;
}


// TODO: account for write-only bits in some registers
Byte APU::read(const Address address) const {
    if (address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
        return channel_3.read(address);
    }

    // just for NR 52, can't initialize in a switch statement
    Byte res = 0x00;

    switch(address) {
    case NR10_REGISTER :
    case NR11_REGISTER :
    case NR12_REGISTER :
    case NR13_REGISTER :
    case NR14_REGISTER :
        return channel_1.read(address);

    case NR21_REGISTER :
    case NR22_REGISTER :
    case NR23_REGISTER :
    case NR24_REGISTER :
        return channel_2.read(address);

    case NR30_REGISTER :
    case NR31_REGISTER :
    case NR32_REGISTER :
    case NR33_REGISTER :
    case NR34_REGISTER :
        return channel_3.read(address);

    case NR41_REGISTER :
    case NR42_REGISTER :
    case NR43_REGISTER :
    case NR44_REGISTER :
        return channel_4.read(address);

    case NR50_REGISTER :
        return master_volume_and_vin_panning;
    case NR51_REGISTER :
        return sound_panning;
    case NR52_REGISTER :
        res = audio_master_control | 0x70;
        if (channel_1.is_active()) { set_bit(res, Bit::Bit0); }
        if (channel_2.is_active()) { set_bit(res, Bit::Bit1); }
        if (channel_3.is_active()) { set_bit(res, Bit::Bit2); }
        if (channel_4.is_active()) { set_bit(res, Bit::Bit3); }
        return res;
    case PCM12_REGISTER:
        return channel_1.output() << 4 | channel_2.output();
    case PCM34_REGISTER:
        return channel_3.output() << 4 | channel_4.output();
    default:
        throw std::runtime_error("APU read called on wrong address");
    }
}


void APU::write(const Address address, const Byte data) {
    if (address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
        channel_3.write(address, data);
        return;
    }

    // if audio is disabled, only audio master control register is allowed to be written to
    if (!audio_enabled() && address != NR52_REGISTER) {
        return;
    }

    switch(address) {
    case NR10_REGISTER :
    case NR11_REGISTER :
    case NR12_REGISTER :
    case NR13_REGISTER :
    case NR14_REGISTER :
        channel_1.write(address, data);
        return;

    case NR21_REGISTER :
    case NR22_REGISTER :
    case NR23_REGISTER :
    case NR24_REGISTER :
        channel_2.write(address, data);
        return;

    case NR30_REGISTER :
    case NR31_REGISTER :
    case NR32_REGISTER :
    case NR33_REGISTER :
    case NR34_REGISTER :
        channel_3.write(address, data);
        return;

    case NR41_REGISTER :
    case NR42_REGISTER :
    case NR43_REGISTER :
    case NR44_REGISTER :
        channel_4.write(address, data);
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
    case PCM12_REGISTER:
        // read-only
        return;
    case PCM34_REGISTER:
        // read-only
        return;
    default:
        throw std::runtime_error("APU write called on wrong address");
    }
}


// step the APU forward 1 m-cycle
void APU::update() {
    channel_1.clock();
    channel_2.clock();
    channel_3.clock();
    channel_4.clock();

    sample_accumulator += 1.0f;
    if (sample_accumulator >= CYCLES_PER_SAMPLE) {
        sample_accumulator -= CYCLES_PER_SAMPLE;

        push_sample();
    }
}


void APU::div_apu_tick() {
    div_apu_counter += 1;

    // sound length ticks up - 256 Hz
    if (div_apu_counter % 2 == 0) {
       tick_length_timers();
    }

    // channel 1 frequency sweep - 128 Hz
    if (div_apu_counter % 4 == 0) {
        channel_1.frequency_sweep();
    }

    // envelope sweep - 64 Hz
    if (div_apu_counter % 8 == 0) {
        envelope_sweep();
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
    
    channel_1.clear();
    channel_2.clear();
    channel_3.clear();
    channel_4.clear();
}



void APU::push_sample() {
    float left_sample = 0.0;
    float right_sample = 0.0;

    // mix
    left_sample += is_set(sound_panning, Bit::Bit4) ? channel_1.DAC() : 0.0f;
    left_sample += is_set(sound_panning, Bit::Bit5) ? channel_2.DAC() : 0.0f;
    left_sample += is_set(sound_panning, Bit::Bit6) ? channel_3.DAC() : 0.0f;
    left_sample += is_set(sound_panning, Bit::Bit7) ? channel_4.DAC() : 0.0f;
    right_sample += is_set(sound_panning, Bit::Bit0) ? channel_1.DAC() : 0.0f;
    right_sample += is_set(sound_panning, Bit::Bit1) ? channel_2.DAC() : 0.0f;
    right_sample += is_set(sound_panning, Bit::Bit2) ? channel_3.DAC() : 0.0f;
    right_sample += is_set(sound_panning, Bit::Bit3) ? channel_4.DAC() : 0.0f;

    left_sample /= 4.0f;
    right_sample /= 4.0f;

    // master volume
    int left_volume = ((master_volume_and_vin_panning & 0x70) >> 4) + 1;
    int right_volume = (master_volume_and_vin_panning & 0x07) + 1;

    left_sample *= left_volume;
    right_sample *= right_volume;

    left_sample /= 8.0f;
    right_sample /= 8.0f;

    // TODO: hpf


    audio_buffer.push_back(left_sample);
    audio_buffer.push_back(right_sample);
}


void APU::tick_length_timers() {
    channel_1.tick_length_timer();
    channel_2.tick_length_timer();
    channel_3.tick_length_timer();
    channel_4.tick_length_timer();
}


void APU::envelope_sweep() {
    channel_1.envelope_sweep();
    channel_2.envelope_sweep();
    channel_4.envelope_sweep();
}