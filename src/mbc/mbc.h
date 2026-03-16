#ifndef MBC_H
#define MBC_H

#include <array>
#include <vector>
#include "../types.h"
#include "../memory/memory_map.h"
#include "../utils/utils.h"

/**
 * Abstract Base Class
 */
class MBC {
    public:
        virtual ~MBC() = default;
        virtual Byte read(const Address address) const = 0;
        virtual void write(const Address address, const Byte data) = 0;
        virtual void save(const std::string& filename) const = 0;
        virtual void load(const std::string& filename) = 0;
};

#endif // MBC_H