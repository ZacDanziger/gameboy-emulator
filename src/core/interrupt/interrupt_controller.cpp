#include "interrupt_controller.h"

Byte InterruptController::read(const Address address) const {
    if (address == IF_REGISTER) {
        return if_register;
    }

    if (address == IE_REGISTER) {
        return ie_register;
    }

    throw std::runtime_error("Interrupt Controller read called on wrong address");
}

void InterruptController::write(const Address address, const Byte data) {
    if (address == IF_REGISTER) {
        if_register = data & 0x1F;
        return;
    }

    if (address == IE_REGISTER) {
        ie_register = data & 0x1F;
        return;
    }

    throw std::runtime_error("Interrupt Controller write called on wrong address");
}

Interrupt InterruptController::acknowledge_interrupt() {
    if (!interrupt_pending()) {
        throw std::runtime_error("Interrupt handler called when no interrupt is pending");
    }

    //V-Blank interrupt
    if ((ie_register & 0x01) & (if_register & 0x01)) {
        if_register &= ~0x01;
        return Interrupt::VBlank;
    }

    // LCD interrupt
    if ((ie_register & 0x02) & (if_register & 0x02)) {
        if_register &= ~0x02;
        return Interrupt::LCDStat;
    }

    // Timer interrupt
    if ((ie_register & 0x04) & (if_register & 0x04)) {
        if_register &= ~0x04;
        return Interrupt::Timer;
    }

    // Serial Interrupt
    if ((ie_register & 0x08) & (if_register & 0x08)) {
        if_register &= ~0x08;
        return Interrupt::Serial;
    }

    // Joypad Interrupt
    if ((ie_register & 0x10) & (if_register & 0x10)) {
        if_register &= ~0x10;
        return Interrupt::Joypad;
    }

    throw std::runtime_error("How did you get here?");
}