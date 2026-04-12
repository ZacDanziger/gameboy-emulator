#include "channels.h"

void Channel::clear() {
    active = false;
    DAC_enabled = false;
    length_enabled = false;
    length_timer = 0;
    period_timer = 0;
    current_volume = 0;
    sample = 0x00;
}

void Channel::tick_length_timer() {
    if (length_enabled) {
        length_timer -= 1;
        if (length_timer == 0) {
            deactivate();
        }
    }
}

/**
 * CHANNEL 1
 */

Byte Channel1::read(const Address address) const {
    switch(address) {
    case NR10_REGISTER :
        return sweep | 0x80;
    case NR11_REGISTER :
        return timer_and_duty_cycle | 0x3F;
    case NR12_REGISTER :
        return volume_and_envelope;
    case NR13_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR14_REGISTER :
        return period_high_and_control| 0xBF;
    default:
        throw std::runtime_error("Channel 1 read called on wrong address.");
    };
}


void Channel1::write(const Address address, const Byte data) {
    switch(address) {
    case NR10_REGISTER : {
        sweep = data;
        if (!is_set(data, Bit::Bit3) && negate_was_used) {
            deactivate();
        }
        return;
    }
    case NR11_REGISTER :
        timer_and_duty_cycle = data;
        length_timer = 64 - (data & 0x3F);
        return;
    case NR12_REGISTER :
        volume_and_envelope = data;
        DAC_enabled = ((data & 0xF8) != 0x00);
        if (!DAC_enabled) { deactivate(); }
        return;
    case NR13_REGISTER :
        period_low = data;
        return;
    case NR14_REGISTER :
        period_high_and_control = data;
        length_enabled = is_set(data, Bit::Bit6);
        return;
    default:
        throw std::runtime_error("Channel 1 write called on wrong address.");
    };
}


void Channel1::trigger(const bool next_step_clocks_length) {
    active = true;
    negate_was_used = false;
    
    if (length_timer == 0) {
        length_timer = 64;
        if (length_enabled && !next_step_clocks_length) {
            length_timer -= 1;
        }
    }
    duty_cycle = (timer_and_duty_cycle & 0xC0) >> 6;

    current_volume = (volume_and_envelope & 0xF0) >> 4;
    period_timer = 2048 - (((period_high_and_control & 0x07) << 8) | period_low);

    envelope_timer = 0;

    shadow_period = ((period_high_and_control & 0x07) << 8) | period_low;
    sweep_timer = (sweep & 0x70) >> 4;
    if (sweep_timer == 0) {
        sweep_timer = 8;
    }

    sweep_enabled = ((sweep & 0x77) != 0x00);

    if ((sweep & 0x07) != 0x00) {
        calculate_frequency();
    }

    if (!DAC_enabled) {
        deactivate();
    }
}


void Channel1::clock() {
    if (!active) {
        return;
    }

    period_timer -= 1;
    if (period_timer == 0) {
        period_timer = 2048 - (((period_high_and_control & 0x07) << 8) | period_low);

        bool high = duty_table[duty_cycle][duty_pos % 8];
        duty_pos += 1;

        sample = high ? current_volume : 0x00;
    }
}


void Channel1::reset() {
    Channel::clear();
    sweep = 0x80;
    timer_and_duty_cycle = 0xBF;
    volume_and_envelope = 0xF3;
    period_low = 0xFF;
    period_high_and_control = 0xBF;

    duty_cycle = 0;
    duty_pos = 0;
    envelope_timer = 0;

    sweep_enabled = false;
    sweep_timer = 0;
    shadow_period = 0x0000;

    negate_was_used = false;
}


void Channel1::clear() {
    Channel::clear();
    sweep = 0x80;
    timer_and_duty_cycle = 0x00;
    volume_and_envelope = 0x00;
    period_low = 0x00;
    period_high_and_control = 0x00;

    duty_cycle = 0;
    envelope_timer = 0;

    sweep_enabled = false;
    sweep_timer = 0;
    shadow_period = 0x0000;

    negate_was_used = false;
}


void Channel1::envelope_sweep() {
    if (!active) {
        return;
    }

    envelope_timer += 1;

    int envelope_pace = volume_and_envelope & 0x07;
    if (envelope_pace == 0) {
        envelope_pace = 8;
    }

    if (envelope_timer == envelope_pace) {
        envelope_timer = 0;

        if (envelope_pace == 8) {
            return;
        }

        current_volume += is_set(volume_and_envelope, Bit::Bit3) ? 1 : -1;

        if (current_volume < 0) {
            current_volume = 0;
        }

        if (current_volume > 15) {
            current_volume = 15;
        }
    }
}


void Channel1::frequency_sweep() {
    if (!active || !sweep_enabled) {
        return;
    }

    int pace = (sweep & 0x70) >> 4;

    sweep_timer -= 1;
    if (sweep_timer == 0) {
        sweep_timer = pace;
        if (sweep_timer == 0) {
            sweep_timer = 8;
        }

        if (pace == 0) {
            return;
        }

        int shift = sweep & 0x07;
        Word new_period = calculate_frequency();

        if (shift != 0) {
            shadow_period = new_period;
            period_low = shadow_period & 0xFF;
            period_high_and_control = (period_high_and_control & 0xF8) | (shadow_period >> 8) & 0x07;

            calculate_frequency();
        }
    }
}


Word Channel1::calculate_frequency() {
    Word offset = shadow_period >> (sweep & 0x07);
    Word new_period = shadow_period + (is_set(sweep, Bit::Bit3) ? -offset : offset);

    if (is_set(sweep, Bit::Bit3)) {
        negate_was_used = true;
    }

    if (new_period > 0x07FF) {
        deactivate();
        return shadow_period;
    }

    return new_period;
}


/**
 * CHANNEL 2
 */

Byte Channel2::read(const Address address) const {
    switch(address) {
    case NR21_REGISTER :
        return timer_and_duty_cycle| 0x3F;
    case NR22_REGISTER :
        return volume_and_envelope;
    case NR23_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR24_REGISTER :
        // only bit 6 is read-able
        return period_high_and_control| 0xBF;
    default:
        throw std::runtime_error("Channel 2 read called on wrong address");
    }
}


void Channel2::write(const Address address, const Byte data) {
    switch(address) {
    case NR21_REGISTER :
        timer_and_duty_cycle = data;
        length_timer = 64 - (data & 0x3F);
        return;
    case NR22_REGISTER :
        volume_and_envelope = data;
        DAC_enabled = ((data & 0xF8) != 0x00);
        if (!DAC_enabled) { deactivate(); }
        return;
    case NR23_REGISTER :
        period_low = data;
        return;
    case NR24_REGISTER :
        period_high_and_control = data;
        length_enabled = is_set(data, Bit::Bit6);
        return;
    default:
        throw std::runtime_error("Channel 2 write called on wrong address.");
    };
}


void Channel2::trigger(const bool next_step_clocks_length) {
    active = true;

    if (length_timer == 0) {
        length_timer = 64;
        if (length_enabled && !next_step_clocks_length) {
            length_timer -= 1;
        }
    }
    duty_cycle = (timer_and_duty_cycle & 0xC0) >> 6;

    current_volume = (volume_and_envelope & 0xF0) >> 4;
    period_timer = 2048 - (((period_high_and_control & 0x07) << 8) | period_low);

    envelope_timer = 0;

    if (!DAC_enabled) {
        deactivate();
    }
}


void Channel2::clock() {
    if (!active) {
        return;
    }

    period_timer -= 1;

    if (period_timer == 0) {
        period_timer = 2048 - (((period_high_and_control & 0x07) << 8) | period_low);

        bool high = duty_table[duty_cycle][duty_pos % 8];
        duty_pos += 1;
    
        sample = high ? current_volume : 0x00;
    }

}

void Channel2::reset() {
    Channel::clear();
    timer_and_duty_cycle = 0x3F;
    volume_and_envelope = 0x00;
    period_low = 0xFF;
    period_high_and_control = 0xBF;

    duty_cycle = 0;
    duty_pos = 0;
    envelope_timer = 0;
}


void Channel2::clear() {
    Channel::clear();
    timer_and_duty_cycle = 0x00;
    volume_and_envelope = 0x00;
    period_low = 0x00;
    period_high_and_control = 0x00;

    duty_cycle = 0;
    envelope_timer = 0;
}


void Channel2::envelope_sweep() {
    if (!active) {
        return;
    }

    envelope_timer += 1;

    int envelope_pace = volume_and_envelope & 0x07;
    if (envelope_pace == 0) {
        envelope_pace = 8;
    }

    if (envelope_timer == envelope_pace) {
        envelope_timer = 0;

        if (envelope_pace == 8) {
            return;
        }

        current_volume += is_set(volume_and_envelope, Bit::Bit3) ? 1 : -1;

        if (current_volume < 0) {
            current_volume = 0;
        }

        if (current_volume > 15) {
            current_volume = 15;
        }
    }
}

/**
 * CHANNEL 3
 */

Byte Channel3::read(const Address address) const {
    if (address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
        if (active) {
            return cgb_mode ? wave_ram[position_counter >> 1] : OPEN_BUS_VALUE;
        }
        return wave_ram[address - WAVE_RAM_START];
    }

    switch(address) {
    case NR30_REGISTER :
        return DAC_enable | 0x7F;
    case NR31_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR32_REGISTER :
        return output_level | 0x9F;
    case NR33_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR34_REGISTER :
        return (period_high_and_control & 0x40) | 0xBF;
    default:
        throw std::runtime_error("Channel 3 read called on wrong address");
    }
}


void Channel3::write(const Address address, const Byte data) {
    if (address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
        wave_ram[address - WAVE_RAM_START] = data;
        return;
    }

    switch(address) {
    case NR30_REGISTER :
        DAC_enable = data;
        DAC_enabled = is_set(data, Bit::Bit7);
        if (!DAC_enabled) { deactivate(); }
        return;
    case NR31_REGISTER :
        initial_length_timer = data;
        length_timer = 256 - data;
        return;
    case NR32_REGISTER :
        output_level = data;
        return;
    case NR33_REGISTER :
        period_low = data;
        return;
    case NR34_REGISTER :
        period_high_and_control = data;
        length_enabled = is_set(data, Bit::Bit6);
        return;
    default:
        throw std::runtime_error("Channel 3 write called on wrong address.");
    }
}

void Channel3::trigger(const bool next_step_clocks_length) {
    active = true;

    if (length_timer == 0) {
        length_timer = 256;
        if (length_enabled && !next_step_clocks_length) {
            length_timer -= 1;
        }
    }

    current_volume = (output_level >> 5) & 0x03;
    period_timer = (2048 - (((period_high_and_control & 0x07) << 8) | period_low)) / 2;
    position_counter = 0;

    if (!DAC_enabled) {
        deactivate();
    }
}


void Channel3::clock() {
    if (!active) {
        return;
    }

    period_timer -= 1;

    if (period_timer == 0) {
        period_timer = (2048 - (((period_high_and_control & 0x07) << 8) | period_low)) / 2;
    
        position_counter += 1;
        position_counter %= 32;
    
        sample = wave_ram[position_counter >> 1];
    
        if (position_counter % 2 == 1) {
            sample = sample & 0x0F;
        } else {
            sample = (sample >> 4) & 0x0F;
        }
        
        current_volume = (output_level >> 5) & 0x03;
        if (current_volume == 0) {
            sample = 0x00;
        } else {
            sample = sample >> (current_volume - 1);
        }
    }
}


void Channel3::reset() {
    Channel::clear();
    DAC_enable = 0x7F;
    initial_length_timer = 0xFF;
    output_level = 0x9F;
    period_low = 0xFF;
    period_high_and_control = 0xBF;

    position_counter = 0;

    wave_ram = cgb_mode ? CGB_WAVE_RAM_BOOT_STATE : DMG_WAVE_RAM_BOOT_STATE;
}

void Channel3::clear() {
    Channel::clear();
    DAC_enable = 0x00;
    initial_length_timer = 0x00;
    output_level = 0x00;
    period_low = 0x00;
    period_high_and_control = 0x00;

    position_counter = 0;
}


/**
 * CHANNEL 4
 */

Byte Channel4::read(const Address address) const {
    switch(address) {
    case NR41_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR42_REGISTER :
        return volume_and_envelope;
    case NR43_REGISTER :
        return freq_and_randomness;
    case NR44_REGISTER :
        return control | 0xBF;
    default:
        throw std::runtime_error("Channel 4 read called on wrong address");
    }
}


void Channel4::write(const Address address, const Byte data) {
    switch(address) {
    case NR41_REGISTER :
        initial_length_timer = data;
        length_timer = 64 - (data & 0x3F);
        return;
    case NR42_REGISTER :
        volume_and_envelope = data;
        DAC_enabled = ((data & 0xF8) != 0x00);
        if (!DAC_enabled) { deactivate(); }
        return;
    case NR43_REGISTER :
        freq_and_randomness = data;
        return;
    case NR44_REGISTER :
        control = data;
        length_enabled = is_set(data, Bit::Bit6);
        return;
    default:
        throw std::runtime_error("Channel 4 write called on wrong address");
    }
}


void Channel4::trigger(const bool next_step_clocks_length) {
    active = true;

    if (length_timer == 0) {
        length_timer = 64;
        if (length_enabled && !next_step_clocks_length) {
            length_timer -= 1;
        }
    }

    current_volume = (volume_and_envelope & 0xF0) >> 4;

    LFSR = 0x0000;

    period_timer = calculate_period();

    envelope_timer = 0;

    if (!DAC_enabled) {
        deactivate();
    }
}

void Channel4::clock() {
    if (!active) {
        return;
    }

    int shift = (freq_and_randomness & 0xF0) >> 4;

    // shift values of 14 or 15 keep the channel from being clocked entirely
    if (shift >= 14) {
        return;
    }
    
    period_timer -= 1;

    if (period_timer == 0) {
        period_timer = calculate_period();

        bool bit0 = is_set(LFSR, Bit::Bit0);
        bool bit1 = is_set(LFSR, Bit::Bit1);

        bool set = (bit0 == bit1);

        if (set) {
            LFSR |= 0x8000;
        } else {
            LFSR &= ~0x8000;
        }

        LFSR = LFSR >> 1;

        if (is_set(freq_and_randomness, Bit::Bit3)) {
            LFSR &= ~0x0080;

            if (set) {
                LFSR |= 0x0080;
            }
        }

        sample = bit1 ? current_volume : 0x00;
    }
}


void Channel4::reset() {
    Channel::clear();
    initial_length_timer = 0xFF;
    volume_and_envelope = 0x00;
    freq_and_randomness = 0x00;
    control = 0xBF;

    LFSR = 0x0000;
    envelope_timer = 0;
}

void Channel4::clear() {
    Channel::clear();
    initial_length_timer = 0x00;
    volume_and_envelope = 0x00;
    freq_and_randomness = 0x00;
    control = 0x00;

    LFSR = 0x0000;
    envelope_timer = 0;
}


void Channel4::envelope_sweep() {
    if (!active) {
        return;
    }

    envelope_timer += 1;

    int envelope_pace = volume_and_envelope & 0x07;
    if (envelope_pace == 0) {
        envelope_pace = 8;
    }

    if (envelope_timer == envelope_pace) {
        envelope_timer = 0;

        if (envelope_pace == 8) {
            return;
        }

        current_volume += is_set(volume_and_envelope, Bit::Bit3) ? 1 : -1;

        if (current_volume < 0) {
            current_volume = 0;
        }

        if (current_volume > 15) {
            current_volume = 15;
        }
    }
}


int Channel4::calculate_period() {
    int shift = (freq_and_randomness & 0xF0) >> 4;
    int divider = freq_and_randomness & 0x07;

    if (divider == 0) {
        return 1 << (shift + 1);
    } else {
        return divider << (shift + 2);
    }
}