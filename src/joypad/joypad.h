#ifndef JOYPAD_H
#define JOYPAD_H

#include <stdexcept>
#include <functional>

#include "../types.h"
#include "../utils/utils.h"
#include "../interrupt/interrupt_controller.h"

class Joypad {
    public:
        Joypad(InterruptController& i) :
            interrupt(i),

            joypad_register(0x00),

            save_requested(false),

            button_keys(0x0F),
            direction_keys(0x0F)
        {}

        Byte read() const;
        void write(const Byte data);

        void set_key(Key key, bool pressed);
        bool any_button_pressed() const { return ((button_keys != 0x0F) || (direction_keys != 0x0F)); }

        bool take_save_request();

        void reset() { joypad_register = 0x00; button_keys = 0x0F; direction_keys = 0x0F; }
    private:
        InterruptController& interrupt;

        Byte joypad_register;   // JOYP_REGISTER

        bool save_requested;

        // 2x4 grid of values for JOYP
        Byte button_keys;
        Byte direction_keys;
};

#endif // JOYPAD_H