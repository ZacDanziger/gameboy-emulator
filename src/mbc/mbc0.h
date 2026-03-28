#ifndef MBC0_H
#define MBC0_H

#include "mbc.h"

/**
 * AKA no MBC
 */
class MBC0 : public MBC {
    public:
        MBC0(std::vector<Byte> rom, size_t ram_size) :
            rom(std::move(rom)),
            eram(ram_size, 0xFF)
        {}

        Byte read(const Address address) const override;
        void write(const Address address, const Byte data) override;

        void save(const std::string& filename) const override { return; }
        void load(const std::string& filename) override { return; }
    private:
        std::vector<Byte> rom;
        std::vector<Byte> eram;
};

#endif  // MBC_0_H