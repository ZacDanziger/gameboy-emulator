#ifndef CHANNELS_H
#define CHANNELS_H

#include <array>

#include "../common/types.h"
#include "../common/memory_map.h"
#include "../common/bit_utils.h"

static constexpr bool duty_table[4][8] = {
    {0, 0, 0, 0, 0, 0, 0, 1},   // 12.5%
    {1, 0, 0, 0, 0, 0, 0, 1},   // 25%
    {1, 0, 0, 0, 0, 1, 1, 1},   // 50%
    {0, 1, 1, 1, 1, 1, 1, 0}    // 75%
};

constexpr std::array<Byte, 16> DMG_WAVE_RAM_BOOT_STATE = {
    0x84, 0x40, 0x43, 0xAA, 0x2D, 0x78, 0x92, 0x3C,
    0x60, 0x59, 0x59, 0xB0, 0x34, 0xB8, 0x2E, 0xDA
};

constexpr std::array<Byte, 16> CGB_WAVE_RAM_BOOT_STATE = {
    0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
    0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF
};


class Channel {
    public:
        Channel() :
            active(false),
            DAC_enabled(false),
            length_enabled(false),
            length_timer(0),
            period_timer(0),
            current_volume(0),
            sample(0x00)
        {}

        virtual ~Channel() = default;

        virtual Byte read(const Address address) const = 0;
        virtual void write(const Address address, const Byte data) = 0;

        virtual void trigger(const bool next_step_clocks_length) = 0;
        virtual void clock() = 0;
        
        virtual void reset() = 0;
        virtual void clear();
        
        void tick_length_timer();
        
        void deactivate() { active = false; };
        Byte output() const { return active ? sample : 0x00; };
        float DAC() const { return DAC_enabled ? ((output() / 7.5f) - 1.0f): 0.0f; }
        bool DAC_is_enabled() const { return DAC_enabled; }
        bool get_length_enabled() const { return length_enabled; }
        int get_length_timer() const { return length_timer; }

        bool is_active() const { return active; }
    protected:
        bool active;
        bool DAC_enabled;
        bool length_enabled;
        int length_timer;
        int period_timer;
        int current_volume;
        Byte sample;
};


class Channel1 : public Channel {
    public:
        Channel1() :
            sweep(0x80),
            timer_and_duty_cycle(0xBF),
            volume_and_envelope(0xF3),
            period_low(0xFF),
            period_high_and_control(0xBF),

            duty_cycle(0),
            duty_pos(0),
            envelope_timer(0),

            sweep_enabled(false),
            sweep_timer(0),
            shadow_period(0x0000),

            negate_was_used(false)
        {}

        Byte read(const Address address) const override;
        void write(const Address address, const Byte data) override;

        void trigger(const bool next_step_clocks_length) override;
        void clock() override;

        void reset() override;
        void clear() override;

        void envelope_sweep();
        void frequency_sweep();
    private:
        Byte sweep;
        Byte timer_and_duty_cycle;
        Byte volume_and_envelope;
        Byte period_low;
        Byte period_high_and_control;

        int duty_cycle;
        int duty_pos;
        int envelope_timer;

        bool sweep_enabled;
        int sweep_timer;
        Word shadow_period;

        bool negate_was_used;

        Word calculate_frequency();
};

class Channel2 : public Channel {
    public:
        Channel2() :
            timer_and_duty_cycle(0x3F),
            volume_and_envelope(0x00),
            period_low(0xFF),
            period_high_and_control(0xBF),

            duty_cycle(0),
            duty_pos(0),
            envelope_timer(0)
        {}

        Byte read(const Address address) const override;
        void write(const Address address, const Byte data) override;

        void trigger(const bool next_step_clocks_length) override;
        void clock() override;

        void reset() override;
        void clear() override;

        void envelope_sweep();
    private:
        Byte timer_and_duty_cycle;
        Byte volume_and_envelope;
        Byte period_low;
        Byte period_high_and_control;

        int duty_cycle;
        int duty_pos;
        int envelope_timer;
};

class Channel3 : public Channel {
    public:
        Channel3() :
            cgb_mode(true),
            wave_ram{CGB_WAVE_RAM_BOOT_STATE},

            DAC_enable(0x7F),
            initial_length_timer(0xFF),
            output_level(0x9F),
            period_low(0xFF),
            period_high_and_control(0xBF),

            position_counter(0)
        {}

        Byte read(const Address address) const override;
        void write(const Address address, const Byte data) override;

        void trigger(const bool next_step_clocks_length) override;
        void clock() override;

        void reset() override;
        void clear() override;

        void set_cgb_mode(const bool cgb) { cgb_mode = cgb; }
    private:
        bool cgb_mode;
        std::array<Byte, 16> wave_ram;           // 0xFF30 - 0xFF3F

        Byte DAC_enable;
        Byte initial_length_timer;
        Byte output_level;
        Byte period_low;
        Byte period_high_and_control;

        int position_counter;
};


class Channel4 : public Channel {
    public:
        Channel4() :
            initial_length_timer(0xFF),
            volume_and_envelope(0x00),
            freq_and_randomness(0x00),
            control(0xBF),

            LFSR(0x0000),
            envelope_timer(0)
        {}

        Byte read(const Address address) const override;
        void write(const Address address, const Byte data) override;

        void trigger(const bool next_step_clocks_length) override;
        void clock() override;

        void reset() override;
        void clear() override;

        void envelope_sweep();
    private:
        Byte initial_length_timer;
        Byte volume_and_envelope;
        Byte freq_and_randomness;
        Byte control;

        Word LFSR;
        int envelope_timer;

        int calculate_period();
};


#endif // CHANNELS_H