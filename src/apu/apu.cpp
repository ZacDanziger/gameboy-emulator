#include "apu.h"

// TODO: account for write-only bits in some registers
Byte APU::read(const Address address) const {
    if (address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
        return wave_ram[address - WAVE_RAM_START];
    }

    switch(address) {
    case NR10_REGISTER :
        return channel_1_sweep;
    case NR11_REGISTER :
        return channel_1_timer_and_duty_cycle;
    case NR12_REGISTER :
        return channel_1_volume_and_envelope;
    case NR13_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR14_REGISTER :
        return channel_1_period_high;

    case NR21_REGISTER :
        return channel_2_timer_and_duty_cycle;
    case NR22_REGISTER :
        return channel_2_volume_and_envelope;
    case NR23_REGISTER :
        return channel_2_period_low;
    case NR24_REGISTER :
        return channel_2_period_high;

    case NR30_REGISTER :
        return channel_3_DAC_enable;
    case NR31_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR32_REGISTER :
        return channel_3_output_level;
    case NR33_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR34_REGISTER :
        return channel_3_period_high;

    case NR41_REGISTER :
        // write only
        return OPEN_BUS_VALUE;
    case NR42_REGISTER :
        return channel_4_volume_and_envelope;
    case NR43_REGISTER :
        return channel_4_freq_and_randomness;
    case NR44_REGISTER :
        return channel_4_control;

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
        channel_1_sweep = data;
        return;
    case NR11_REGISTER :
        channel_1_timer_and_duty_cycle = data;
        return;
    case NR12_REGISTER :
        channel_1_volume_and_envelope = data;
        return;
    case NR13_REGISTER :
        channel_1_period_low = data;
        return;
    case NR14_REGISTER :
        channel_1_period_high = data;
        return;

    case NR21_REGISTER :
        channel_2_timer_and_duty_cycle = data;
        return;
    case NR22_REGISTER :
        channel_2_volume_and_envelope = data;
        return;
    case NR23_REGISTER :
        channel_2_period_low = data;
        return;
    case NR24_REGISTER :
        channel_2_period_high = data;
        return;

    case NR30_REGISTER :
        channel_3_DAC_enable = data;
        return;
    case NR31_REGISTER :
        channel_3_length_timer = data;
        return;
    case NR32_REGISTER :
        channel_3_output_level = data;
        return;
    case NR33_REGISTER :
        channel_3_period_low = data;
        return;
    case NR34_REGISTER :
        channel_3_period_high = data;
        return;

    case NR41_REGISTER :
        channel_4_length_timer = data;
        return;
    case NR42_REGISTER :
        channel_4_volume_and_envelope = data;
        return;
    case NR43_REGISTER :
        channel_4_freq_and_randomness = data;
        return;
    case NR44_REGISTER :
        channel_4_control = data;
        return;

    case NR50_REGISTER :
        master_volume_and_vin_panning = data;
        return;
    case NR51_REGISTER :
        sound_panning = data;
        return;
    case NR52_REGISTER :
        if (!is_set(data, Bit::Bit7)) {
            clear_registers();
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


// called whenever the Timer's DIV-APU bit goes low
void APU::tick() {
    div_apu_counter += 1;

    // every 8 ticks, do envelope sweep

    // every 2 ticks, update sound length

    // every 4 ticks, update ch1 freq sweep
}


/**
 * Write 0x00 to all APU registers
 */
void APU::clear_registers() {
        audio_master_control           = 0x00;
        sound_panning                  = 0x00;
        master_volume_and_vin_panning  = 0x00;

        channel_1_sweep                = 0x00;
        channel_1_timer_and_duty_cycle = 0x00;
        channel_1_volume_and_envelope  = 0x00;
        channel_1_period_low           = 0x00;
        channel_1_period_high          = 0x00;

        channel_2_timer_and_duty_cycle = 0x00;
        channel_2_volume_and_envelope  = 0x00;
        channel_2_period_low           = 0x00;
        channel_2_period_high          = 0x00;

        channel_3_DAC_enable           = 0x00;
        channel_3_length_timer         = 0x00;
        channel_3_output_level         = 0x00;
        channel_3_period_low           = 0x00;
        channel_3_period_high          = 0x00;

        channel_4_length_timer         = 0x00;
        channel_4_volume_and_envelope  = 0x00;
        channel_4_freq_and_randomness  = 0x00;
        channel_4_control              = 0x00;
}