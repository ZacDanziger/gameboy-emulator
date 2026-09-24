#ifndef MBC0_H
#define MBC0_H

#include <vector>
#include <utility>
#include <stdexcept>

#include "cartridge.h"
#include "../types.h"
#include "../memory_map.h"

/**
 * AKA no MBC
 */
class MBC0 : public Cartridge {
    public:
        MBC0(std::vector<Byte> rom, size_t ram_size) :
            Cartridge(std::move(rom), ram_size)
        {}

        Byte read(const Address address) const override;
        void write(const Address address, const Byte data) override;
};

#endif  // MBC_0_H