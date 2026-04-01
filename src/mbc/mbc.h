#ifndef MBC_H
#define MBC_H

#include <array>
#include <vector>
#include "../types.h"
#include "../memory_map.h"
#include "../utils/utils.h"

/**
 * Memory Bank Controller
 */
class MBC {
    public:
        virtual ~MBC() = default;
        
        virtual Byte read(const Address address) const = 0;
        virtual void write(const Address address, const Byte data) = 0;

        virtual void save(const std::string& filename) const { return; };
        virtual void load(const std::string& filename) { return; };
};

#endif // MBC_H