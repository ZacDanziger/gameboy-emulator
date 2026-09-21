#include "joypad.h"

/**
 * Read the joypad register, which returns the state of the buttons selected 
 *     by the upper-nibble of the register
 */
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

/**
 * Write to the joypad register, which selects which set of buttons to read from
 * 
 * @param data the value to write to the joypad register (upper-nibble only, lower-nibble is read-only)
 */
void Joypad::write(const Byte data) {
    // lower nibble is read-only
    joypad_register = data & 0xF0;
}


/**
 * Given a key press, reset the corresponding bit in the 2x4 joypad register (0s indicate pressed buttons)
 * 
 * @param key the key that has been changed
 * @param pressed true if pressed, false if released
 */
void Joypad::set_button_state(const ButtonState& state) {
    Byte old_button_keys = button_keys;
    Byte old_direction_keys = direction_keys;

    button_keys = 0x0F;
    direction_keys = 0x0F;

    if (state.a) reset_bit(button_keys, Bit::Bit0);
    if (state.b) reset_bit(button_keys, Bit::Bit1);
    if (state.select) reset_bit(button_keys, Bit::Bit2);
    if (state.start) reset_bit(button_keys, Bit::Bit3);

    if (state.right) reset_bit(direction_keys, Bit::Bit0);
    if (state.left) reset_bit(direction_keys, Bit::Bit1);
    if (state.up) reset_bit(direction_keys, Bit::Bit2);
    if (state.down) reset_bit(direction_keys, Bit::Bit3);

    if ((old_button_keys & ~button_keys) || (old_direction_keys & ~direction_keys)) {
        interrupt.request_interrupt(Interrupt::Joypad);
    }
}