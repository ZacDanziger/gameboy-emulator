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

Byte InterruptController::acknowledge_interrupt() {
    if (!interrupt_pending()) {
        throw std::runtime_error("Interrupt handler called when no interrupt is pending");
    }

    Byte interrupt = (if_register & ie_register);
    int IVT_index = 0;

    for (int i = 0; i < 5; i++) {
        if (interrupt & (1 << i)) {
            IVT_index = i;

            // acknowledge the interrupt
            if_register &= ~(1 << i);
            break;
        }
    }

    return interrupt_vector[IVT_index];
}