#ifndef MBC_H
#define MBC_H

#include <string>

#include "../types.h"

/**
 * Memory Bank Controller
 */
class MBC {
    public:
        virtual ~MBC() = default;
        
        virtual Byte read(const Address address) const = 0;
        virtual void write(const Address address, const Byte data) = 0;

        virtual void save() const { return; };
        virtual void load(const std::string& filename) { return; };

        std::string get_save_filename() const { return rom_filename.substr(0, rom_filename.find_last_of('.')) + ".sav"; } 
    protected:
        std::string rom_filename;
};

#endif // MBC_H