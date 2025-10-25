#include "memory.h"
#include <iostream>

bool test_memory_constructors() {
    Memory memory1(0);
    Memory memory2(-1);     // negative sizes should not be allowed

    std::vector<uint8_t> test_data = {0b00000000, 0b11111111, 0b11110000, 0b00001111, 0b10101010};

    Memory memory3(test_data);

    int size = memory3.get_size();

    for (int i = 0; i < size; i++) {
        std::cout << "At index " << i << " memory3 has: " << memory3.read(i) << std::endl;
    }
    
}

int main() {
    test_memory_constructors();
}