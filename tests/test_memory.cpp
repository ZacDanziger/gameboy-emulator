#include "../src/memory/memory.h"
#include <iostream>
#include <gtest/gtest.h>

#include <cstdlib>
#include <fstream>
#include <array>


TEST(MemoryTest, LoadROM) {
    Memory mem;

    std::srand(std::time(NULL));
    std::ofstream out;
    out.open("numbers.txt", std::fstream::binary);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open file");
    }

    std::array<uint8_t, ROM_BANK_SIZE> nums = {0};

    for (int i = 0; i < ROM_BANK_SIZE; i++) {
        nums[i] = std::rand() % 0xFFFF;
    }

    for (uint8_t num : nums) {
        out << num << '\n';
    }

    out.close();

    EXPECT_NO_THROW(
        mem.load_rom("numbers.txt");
    );

    
}

TEST(MemoryTest, HandlesRead) {

}

TEST(MemoryTest, HandlesWrite) {

}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}