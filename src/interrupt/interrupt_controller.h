#ifndef INTERRUPT_CONTROLLER_H
#define INTERRUPT_CONTROLLER_H

#include "../types.h"
#include "../utils/utils.h"
#include "../memory/memory_map.h"

class InterruptController {
    public:
        InterruptController() :
            if_register(0xE1),
            ie_register(0x00)
        {}

        Byte read(const Address address) const;
        void write(const Address address, const Byte data);

        bool interrupt_pending() { return (if_register & ie_register) != 0; }
        void request_interrupt(Interrupt interrupt) { set_bit(if_register, static_cast<Bit>(interrupt)); }

        Interrupt acknowledge_interrupt();

    private:
        Byte if_register;           // IF_REGISTER
        Byte ie_register;           // IE_REGISTER



};

#endif// INTERRUPT_CONTROLLER_H