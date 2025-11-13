#include "../src/memory/memory.h"
#include <iostream>
#include "gtest/gtest.h"

#include <cstdlib>
#include <fstream>
#include <array>

class MemoryTest : public testing::Test {
    protected:
        MemoryTest() : _mem(), _data{0} {};
        Memory _mem;
        std::array<uint8_t, ROM_BANK_SIZE> _data;

        void create_data() {
            std::srand(std::time(NULL));
            std::ofstream out("numbers.txt", std::fstream::binary);

            if (!out.is_open()) {
                throw std::runtime_error("Cannot open file");
            }

    
            for (int i = 0; i < ROM_BANK_SIZE; i++) {
                _data[i] = static_cast<uint8_t> (std::rand() % 0xFF);
            }

            out.write(reinterpret_cast<const char*>(_data.data()), _data.size());
            out.close();
        }
};


TEST_F(MemoryTest, TestLoad) {
    create_data();

    EXPECT_THROW(
        _mem.load(ROM_BANK_00_START, "bumbers.txt"), std::runtime_error
    );

    EXPECT_NO_THROW(
        _mem.load(ROM_BANK_00_START, "numbers.txt");
    );

    for (int i = 0; i < ROM_BANK_SIZE; i++) {
        EXPECT_EQ(_data[i], _mem.read(i));
    }

    EXPECT_NO_THROW(
        _mem.load(ROM_BANK_NN_START, "numbers.txt");
    );

    for (int i = 0; i < ROM_BANK_SIZE; i++) {
        EXPECT_EQ(_data[i], _mem.read(i + ROM_BANK_NN_START));
    }

    EXPECT_NO_THROW(
        _mem.load(VRAM_START, "numbers.txt");
    );

    for (int i = 0; i < VRAM_SIZE; i++) {
        EXPECT_EQ(_data[i], _mem.read(i + VRAM_START));
    }

    EXPECT_NO_THROW(
        _mem.load(WRAM_BANK_00_START, "numbers.txt");
    );

    for (int i = 0; i < WRAM_BANK_SIZE; i++) {
        EXPECT_EQ(_data[i], _mem.read(i + WRAM_BANK_00_START));
    }

    EXPECT_NO_THROW(
        _mem.load(WRAM_BANK_NN_START, "numbers.txt");
    );

    for (int i = 0; i < WRAM_BANK_SIZE; i++) {
        EXPECT_EQ(_data[i], _mem.read(i + WRAM_BANK_NN_START));
    }

    EXPECT_NO_THROW(
        _mem.load(IO_START, "numbers.txt");
    );

    for (int i = 0; i < IO_REG_SIZE; i++) {
        EXPECT_EQ(_data[i], _mem.read(i + IO_START)) << "at index " << i;
    }

    EXPECT_NO_THROW(
        _mem.load(HRAM_START, "numbers.txt");
    );

    for (int i = 0; i < HRAM_SIZE; i++) {
        EXPECT_EQ(_data[i], _mem.read(i + HRAM_START));
    }

}

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