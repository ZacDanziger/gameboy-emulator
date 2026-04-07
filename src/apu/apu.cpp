#include "apu.h"

/**
 * Reset the APU to its post Boot ROM state
 */
void APU::reset() {
    audio_buffer = {0};
    wave_ram = {0};
    
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
        return channel_1.period_high_and_control;

    case NR21_REGISTER :
        return channel_2.timer_and_duty_cycle & 0xC0;
    case NR22_REGISTER :
        return channel_2.volume_and_envelope;
    case NR23_REGISTER :
        return channel_2.period_low;
    case NR24_REGISTER :
        return channel_2.period_high_and_control;

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
    case PCM12_REGISTER:
        // TODO: change to actual value
        return OPEN_BUS_VALUE;
    case PCM34_REGISTER:
        // TODO: change to actual value
        return OPEN_BUS_VALUE;
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
        if (is_set(channel_1.period_high_and_control, Bit::Bit7)) {

        }
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
        return;

    case NR30_REGISTER :
        channel_3.DAC_enable = data;
        return;
    case NR31_REGISTER :
        channel_3.length_timer = data;
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
        channel_4.length_timer = data;
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
        channel_1_freq_sweep();
    }

    // envelope sweep - 64 Hz
    if (div_apu_counter % 8 == 0) {
        envelope_sweep();
    }
}


/**
 * Write 0x00 to all APU registers
 */
void APU::clear_registers_and_channels() {
    audio_master_control = 0x00;
    sound_panning = 0x00;
    master_volume_and_vin_panning = 0x00;
    
    channel_1 = PulseChannel{};
    channel_2 = PulseChannel{};
    channel_3 = WaveChannel{};
    channel_4 = NoiseChannel{};

    div_apu_counter = 0;
}

void APU::trigger_channel_1() {
    channel_1.active = true;
    channel_1.shadow_period = ((channel_1.period_high_and_control & 0x07) << 8) | (channel_1.period_low);
    channel_1.sweep_timer = 0;

    // check that either sweep pace or individual step are non-zero
    channel_1.sweep_enabled = ((channel_1.sweep & 0x77) != 0x00);

}


Byte APU::channel_1_output() {
    if (!is_set(channel_1.period_high_and_control, Bit::Bit7)) {
        return 0x00;
    }
}

void APU::tick_length_timers() {
    Byte current_length_timer = 0x00;

    // if channel 1 is enabled, tick length timer
    if (is_set(channel_1.period_high_and_control, Bit::Bit7)) {
        current_length_timer = channel_1.timer_and_duty_cycle & 0x1F;
        channel_1.timer_and_duty_cycle &= ~0x1F;

        current_length_timer += 1;

        if (current_length_timer == 64) {
            reset_bit(channel_1.period_high_and_control, Bit::Bit7);
            reset_bit(audio_master_control, Bit::Bit0);
        } else {
            channel_1.timer_and_duty_cycle |= current_length_timer;
        }
    }

    // if channel 2 is enabled, tick length timer
    if (is_set(channel_2.period_high_and_control, Bit::Bit7)) {
        current_length_timer = channel_2.timer_and_duty_cycle & 0x1F;
        channel_2.timer_and_duty_cycle &= ~0x1F;

        current_length_timer += 1;

        if (current_length_timer == 64) {
            reset_bit(channel_2.period_high_and_control, Bit::Bit7);
            reset_bit(audio_master_control, Bit::Bit1);
        } else {
            channel_2.timer_and_duty_cycle |= current_length_timer;
        }
    }

    // if channel 3 is enabled, tick length timer
    if (is_set(channel_3.period_high_and_control, Bit::Bit7)) {
        channel_3.length_timer += 1;

        if (channel_3.length_timer == 0x00) {
            reset_bit(channel_3.period_high_and_control, Bit::Bit7);
            reset_bit(audio_master_control, Bit::Bit2);
        }
    }

    // if channel 4 is enabled, tick length timer
    if (is_set(channel_4.control, Bit::Bit7)) {
        channel_4.length_timer += 1;


        if (channel_4.length_timer == 64) {
            channel_4.length_timer = 0;
            reset_bit(channel_4.control, Bit::Bit7);
            reset_bit(audio_master_control, Bit::Bit3);
        } 
    }
}


void APU::channel_1_freq_sweep() {
    int step = channel_1.sweep & 0x07;
    Word offset = channel_1.shadow_period >> step;

    Word new_period = is_set(channel_1.sweep, Bit::Bit3)
        ? channel_1.shadow_period - offset
        : channel_1.shadow_period + offset;

    if (new_period > 2047) {
        reset_bit(channel_1.period_high_and_control, Bit::Bit7);
        reset_bit(audio_master_control, Bit::Bit0);
        return;
    }

    if (step != 0) {
        channel_1.shadow_period = new_period;
        channel_1.period_low = static_cast<Byte>(new_period & 0x00FF);
        channel_1.period_high_and_control &= ~0x07;
        channel_1.period_high_and_control |= static_cast<Byte>((new_period >> 8) & 0x07);
    }
}


void APU::envelope_sweep() {
    if (is_set(channel_1.period_high_and_control, Bit::Bit7)) {
        int pace = channel_1.volume_and_envelope & 0x07;

        if (pace != 0) {
            channel_1.current_volume += is_set(channel_1.volume_and_envelope, Bit::Bit3) ? 1 : -1;
    
            if (channel_1.current_volume == 0x10) {
                channel_1.current_volume = 0x0F;
            }
    
            if (channel_1.current_volume == 0xFF) {
                channel_1.current_volume = 0x00;
            }
        }
    }

    if (is_set(channel_2.period_high_and_control, Bit::Bit7)) {
        int pace = channel_2.volume_and_envelope & 0x07;
        
        if (pace != 0) {
            channel_2.current_volume += is_set(channel_2.volume_and_envelope, Bit::Bit3) ? 1 : -1;
    
            if (channel_2.current_volume == 0x10) {
                channel_2.current_volume = 0x0F;
            }
    
            if (channel_2.current_volume == 0xFF) {
                channel_2.current_volume = 0x00;
            }
        }
    }

    if (is_set(channel_3.period_high_and_control, Bit::Bit7)) {

    }

    if (is_set(channel_4.control, Bit::Bit7)) {

    }
}