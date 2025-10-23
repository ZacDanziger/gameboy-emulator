#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include <stdexcept>
#include <vector>

// Needs Error Handling
class Memory {
    public:
        Memory(size_t size);
        Memory(const std::vector<uint8_t>& data);

        uint8_t read(uint16_t address);
        virtual void write(uint16_t address, uint8_t data);
        
    private:
        size_t _size;
        std::vector<uint8_t> _bytes;
};

class ROM : public Memory {
    public:
        ROM(const std::vector<uint8_t>& data);

        void write(uint16_t address, uint8_t data) override;
};

#endif