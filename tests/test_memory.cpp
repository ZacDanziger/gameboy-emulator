#include "../src/memory/mmu.h"
#include <iostream>
#include "gtest/gtest.h"

#include <cstdlib>
#include <fstream>
#include <array>

class MemoryTest : public testing::Test {
    protected:
        MemoryTest() : mmu() {};
        MMU mmu;
};


// TEST_F(MemoryTest, TestLoad) {
//     create_data<ROM_BANK_SIZE>();

//     EXPECT_THROW(
//         _mem.load(ROM_BANK_00_START, "bumbers.txt"), std::runtime_error
//     );

//     EXPECT_NO_THROW(
//         _mem.load(ROM_BANK_00_START, "numbers.txt");
//     );

//     for (int i = 0; i < ROM_BANK_SIZE; i++) {
//         EXPECT_EQ(data[i], _mem.read(i));
//     }

//     EXPECT_NO_THROW(
//         _mem.load(ROM_BANK_NN_START, "numbers.txt");
//     );

//     for (int i = 0; i < ROM_BANK_SIZE; i++) {
//         EXPECT_EQ(data[i], _mem.read(i + ROM_BANK_NN_START));
//     }

//     EXPECT_NO_THROW(
//         _mem.load(VRAM_START, "numbers.txt");
//     );

//     for (int i = 0; i < VRAM_SIZE; i++) {
//         EXPECT_EQ(data[i], _mem.read(i + VRAM_START));
//     }

//     EXPECT_NO_THROW(
//         _mem.load(WRAM_BANK_00_START, "numbers.txt");
//     );

//     for (int i = 0; i < WRAM_BANK_SIZE; i++) {
//         EXPECT_EQ(data[i], _mem.read(i + WRAM_BANK_00_START));
//     }

//     EXPECT_NO_THROW(
//         _mem.load(WRAM_BANK_NN_START, "numbers.txt");
//     );

//     for (int i = 0; i < WRAM_BANK_SIZE; i++) {
//         EXPECT_EQ(data[i], _mem.read(i + WRAM_BANK_NN_START));
//     }

//     EXPECT_NO_THROW(
//         _mem.load(IO_START, "numbers.txt");
//     );

//     for (int i = 0; i < IO_REG_SIZE; i++) {
//         EXPECT_EQ(data[i], _mem.read(i + IO_START)) << "at index " << i;
//     }

//     EXPECT_NO_THROW(
//         _mem.load(HRAM_START, "numbers.txt");
//     );

//     for (int i = 0; i < HRAM_SIZE; i++) {
//         EXPECT_EQ(data[i], _mem.read(i + HRAM_START));
//     }

// }

// NO WRITING TO RAM !!!

// TEST_F(MemoryTest, HandlesWrite) {
//     int index = std::rand() % ROM_BANK_SIZE;

//     uint8_t old_data = _mem.read(index);
//     _mem.write(index, old_data + 1);

//     EXPECT_NE(_mem.read(index), old_data);
// }



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}