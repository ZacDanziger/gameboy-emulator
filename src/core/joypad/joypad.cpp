#include "joypad.h"

Byte Joypad::read() const {
    switch(joypad_register & 0x30) {
        case 0x00:
            return 0x0F;
        case 0x10:
            return (0x10 | button_keys);
        case 0x20:
            return (0x20 | direction_keys);
        case 0x30:
            return 0x3F;
        default:
            throw std::runtime_error("How did you get here?");
    }
}


void Joypad::write(const Byte data) {
    // lower nibble is read-only
    joypad_register = data & 0xF0;
}


/**
 * Given a key press or release, set or reset the corresponding bit in the 2x4 joypad register
 * 
 * @param key the key that has been changed
 * @param pressed true if pressed, false if released
 */
void Joypad::set_key(InputEvent e) {
    Byte* target = nullptr;
    Bit bit = Bit::Bit0;

    switch(e.button) {
    case Button::A:
        target = &button_keys;
        bit = Bit::Bit0;
        break;
    case Button::B:
        target = &button_keys;
        bit = Bit::Bit1;
        break;
    case Button::Select:
        target = &button_keys;
        bit = Bit::Bit2;
        break;
    case Button::Start:
        target = &button_keys;
        bit = Bit::Bit3;
        break;
    case Button::Right:
        target = &direction_keys;
        bit = Bit::Bit0;
        break;
    case Button::Left:
        target = &direction_keys;
        bit = Bit::Bit1;
        break;
    case Button::Up:
        target = &direction_keys;
        bit = Bit::Bit2;
        break;
    case Button::Down:
        target = &direction_keys;
        bit = Bit::Bit3;
        break;
    }   

    if (e.pressed) {
        reset_bit(*target, bit);
        interrupt.request_interrupt(Interrupt::Joypad);
    } else {
        set_bit(*target, bit);
    }
}


bool Joypad::take_save_request() {
    bool request = save_requested;
    save_requested = false;
    return request;
}