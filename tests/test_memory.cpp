#include "../src/memory/mmu.h"
#include <iostream>
#include "gtest/gtest.h"

#include <cstdlib>
#include <fstream>
#include <array>


TEST(MemoryTest, TestLoad) {
    MMU mmu;
    PPU ppu([&mmu](Interrupt i){ mmu.request_interrupt(i); },
            []{});
    Timer timer([&mmu](Interrupt i){ mmu.request_interrupt(i); },
                [&ppu]{ ppu.update(); });

    mmu.init(&timer, &ppu);
    create_data<ROM_BANK_SIZE>();

    std::vector<Byte> data = read_file("numbers.txt");

    EXPECT_THROW(
        mmu.load(ROM_BANK_00_START, "bumbers.txt"), std::runtime_error
    );

    EXPECT_NO_THROW(
        mmu.load(ROM_BANK_00_START, "numbers.txt");
    );

    for (int i = 0; i < ROM_BANK_SIZE; i++) {
        EXPECT_EQ(data[i], mmu.read(i));
    }

    EXPECT_NO_THROW(
        mmu.load(ROM_BANK_NN_START, "numbers.txt");
    );

    for (int i = 0; i < ROM_BANK_SIZE; i++) {
        EXPECT_EQ(data[i], mmu.read(i + ROM_BANK_NN_START));
    }

    EXPECT_NO_THROW(
        mmu.load(VRAM_START, "numbers.txt");
    );

    for (int i = 0; i < VRAM_SIZE; i++) {
        EXPECT_EQ(data[i], mmu.read(i + VRAM_START));
    }

    EXPECT_NO_THROW(
        mmu.load(WRAM_BANK_00_START, "numbers.txt");
    );

    for (int i = 0; i < WRAM_BANK_SIZE; i++) {
        EXPECT_EQ(data[i], mmu.read(i + WRAM_BANK_00_START));
    }

    EXPECT_NO_THROW(
        mmu.load(WRAM_BANK_NN_START, "numbers.txt");
    );

    for (int i = 0; i < WRAM_BANK_SIZE; i++) {
        EXPECT_EQ(data[i], mmu.read(i + WRAM_BANK_NN_START));
    }

    EXPECT_NO_THROW(
        mmu.load(IO_START, "numbers.txt");
    );

    for (int i = 0; i < IO_REG_SIZE; i++) {
        if ((i == 4) || (i == 68)) {
            EXPECT_EQ(mmu.read(i + IO_START), 0x00) << "at index " << i;
        } else {
            EXPECT_EQ(data[i], mmu.read(i + IO_START)) << "at index " << i;
        }
    }

    EXPECT_NO_THROW(
        mmu.load(HRAM_START, "numbers.txt");
    );

    for (int i = 0; i < HRAM_SIZE; i++) {
        EXPECT_EQ(data[i], mmu.read(i + HRAM_START));
    }

}

// NO WRITING TO RAM !!!

// TEST_F(MemoryTest, HandlesWrite) {
//     int index = std::rand() % ROM_BANK_SIZE;

//     uint8_t old_data = mmu.read(index);
//     mmu.write(index, old_data + 1);

//     EXPECT_NE(mmu.read(index), old_data);
// }



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}