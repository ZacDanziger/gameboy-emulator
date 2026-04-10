#include "apu.h"

/**
 * Reset the APU to its post Boot ROM state
 */
void APU::reset() {
    audio_buffer = {0};

    frame_sequencer_step = 0;
    sample_accumulator = 0.0f;

    hpf_capacitor_left = 0.0f;
    hpf_capacitor_right = 0.0f;
    
    channel_1.reset();
    channel_2.reset();
    channel_3.reset();
    channel_4.reset();

    audio_master_control = 0x80;
    sound_panning = 0xF3;
    master_volume_and_vin_panning = 0x77;
}


// TODO: account for write-only bits in some registers
Byte APU::read(const Address address) const {
    if (address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
        return channel_3.read(address);
    }

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
    case NR52_REGISTER : {
        Byte status = audio_master_control | 0x70;
        if (channel_1.is_active()) { set_bit(status, Bit::Bit0); }
        if (channel_2.is_active()) { set_bit(status, Bit::Bit1); }
        if (channel_3.is_active()) { set_bit(status, Bit::Bit2); }
        if (channel_4.is_active()) { set_bit(status, Bit::Bit3); }
        return status;
    }
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
        channel_1.write(address, data);
        return;
    case NR14_REGISTER : {
        trigger_logic(&channel_1, address, data);
        return;
    }

    case NR21_REGISTER :
    case NR22_REGISTER :
    case NR23_REGISTER :
        channel_2.write(address, data);
        return;
    case NR24_REGISTER : {
        trigger_logic(&channel_2, address, data);
        return;
    }

    case NR30_REGISTER :
    case NR31_REGISTER :
    case NR32_REGISTER :
    case NR33_REGISTER :
        channel_3.write(address, data);
        return;
    case NR34_REGISTER : {
        trigger_logic(&channel_3, address, data);
        return;
    }

    case NR41_REGISTER :
    case NR42_REGISTER :
    case NR43_REGISTER :
        channel_4.write(address, data);
        return;
    case NR44_REGISTER : {
        trigger_logic(&channel_4, address, data);
        return;
    }

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


void APU::frame_sequencer() {
    // sound length ticks up - 256 Hz
    if (frame_sequencer_step % 2 == 0) {
       tick_length_timers();
    }

    // channel 1 frequency sweep - 128 Hz
    if (frame_sequencer_step == 2 || frame_sequencer_step == 6) {
        channel_1.frequency_sweep();
    }

    // envelope sweep - 64 Hz
    if (frame_sequencer_step == 7) {
        envelope_sweep();
    }

    frame_sequencer_step += 1;
    frame_sequencer_step %= 8;
}


std::vector<float> APU::flush_audio_buffer() {
    std::vector<float> buffer_copy = std::move(audio_buffer);
    audio_buffer.clear();

    return buffer_copy;
}


/**
 * Write 0x00 to all APU registers
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
    if (!channel_1.DAC_is_enabled() &&
        !channel_2.DAC_is_enabled() &&
        !channel_3.DAC_is_enabled() &&
        !channel_3.DAC_is_enabled())
    {
        return;
    }

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

    // hpf
    float filtered_left = left_sample - hpf_capacitor_left;
    float filtered_right = right_sample - hpf_capacitor_right;

    hpf_capacitor_left = left_sample - filtered_left * HPF_CHARGE_FACTOR;
    hpf_capacitor_right = right_sample - filtered_right * HPF_CHARGE_FACTOR;


    audio_buffer.push_back(filtered_left);
    audio_buffer.push_back(filtered_right);
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


void APU::trigger_logic(Channel* channel, const Address address, const Byte data) {
    bool next_step_clocks_length = (frame_sequencer_step % 2 == 0); 

    bool was_enabled = channel->get_length_enabled();
    channel->write(address, data);
    bool is_enabled = channel->get_length_enabled();

    if (!was_enabled && is_enabled && !next_step_clocks_length && (channel->get_length_timer() > 0)) {
        channel->tick_length_timer();
    }

    if (is_set(data, Bit::Bit7)) {
        channel->trigger(next_step_clocks_length);
    }
}