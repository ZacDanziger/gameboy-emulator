#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>
#include <vector>
#include <memory>

#include "../types.h"
#include "../memory_map.h"
#include "../utils/file_io.h"


/**
 * Cartridge
 */
class Cartridge {
    public:
        Cartridge(std::vector<Byte> rom, size_t ram_size) :
            rom(std::move(rom)),
            eram(ram_size, 0xFF),

            cgb_mode(false)
        {}

        virtual ~Cartridge() = default;

        // Static factory method
        static std::unique_ptr<Cartridge> load_from_file(const std::string& rom_file);

        bool is_cgb() const { return cgb_mode;}
        
        virtual Byte read(const Address address) const = 0;
        virtual void write(const Address address, const Byte data) = 0;

        virtual void save() const { return; };
        virtual void load(const std::string& filename) { return; };

        std::string get_save_filename() const { return rom_filename.substr(0, rom_filename.find_last_of('.')) + ".sav"; } 
    protected:
        std::string rom_filename;

        std::vector<Byte> rom;
        std::vector<Byte> eram;

        bool cgb_mode;
};

#endif // CARTRIDGE_H