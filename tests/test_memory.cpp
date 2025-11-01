#include "../src/memory/memory.h"
#include <iostream>
#include "gtest/gtest.h"

#include <cstdlib>
#include <fstream>
#include <array>


TEST(MemoryTest, LoadROM) {
    Memory mem;

    /*
    * Fill numbers.txt with random numbers
    */
    std::srand(std::time(NULL));
    std::ofstream out("numbers.txt", std::fstream::binary);
    EXPECT_NO_THROW(
        if (!out.is_open()) {
        throw std::runtime_error("Cannot open file");
    }
    );
    

    std::array<uint8_t, ROM_BANK_SIZE> nums = {0};

    for (int i = 0; i < ROM_BANK_SIZE; i++) {
        nums[i] = static_cast<uint8_t> (std::rand() % 0xFFFF);
    }

    out.write(reinterpret_cast<const char*>(nums.data()), nums.size());

    out.close();

    EXPECT_NO_THROW(
        mem.load_rom("numbers.txt");
    );

    for (int i = 0; i < ROM_BANK_SIZE; i++) {
        EXPECT_EQ(nums[i], mem.read(i));
    }

    
}

// TEST(MemoryTest, HandlesRead) {

// }

// TEST(MemoryTest, HandlesWrite) {

// }



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}