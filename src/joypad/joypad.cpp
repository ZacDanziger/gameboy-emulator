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
void Joypad::set_key(Key key, bool pressed) {
    Byte* target = nullptr;
    Bit bit = Bit::Bit0;

    switch(key) {
    case Key::A:
        target = &button_keys;
        bit = Bit::Bit0;
        break;
    case Key::B:
        target = &button_keys;
        bit = Bit::Bit1;
        break;
    case Key::Select:
        target = &button_keys;
        bit = Bit::Bit2;
        break;
    case Key::Start:
        target = &button_keys;
        bit = Bit::Bit3;
        break;
    case Key::Right:
        target = &direction_keys;
        bit = Bit::Bit0;
        break;
    case Key::Left:
        target = &direction_keys;
        bit = Bit::Bit1;
        break;
    case Key::Up:
        target = &direction_keys;
        bit = Bit::Bit2;
        break;
    case Key::Down:
        target = &direction_keys;
        bit = Bit::Bit3;
        break;
    case Key::SAVE:
        // save();
        return;
    }   

    if (pressed) {
        reset_bit(*target, bit);
        interrupt.request_interrupt(Interrupt::Joypad);
    } else {
        set_bit(*target, bit);
    }
}