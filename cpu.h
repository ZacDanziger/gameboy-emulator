#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stddef.h>
#include <vector>

class CPU {
    public:
        CPU();

        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        
    private:
        std::vector<uint8_t> bytes arr(SIZE);
};


#endif