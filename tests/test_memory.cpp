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


TEST_F(MemoryTest, LoadROM) {
    create_data();

    EXPECT_NO_THROW(
        _mem.load_rom("numbers.txt");
    );

    for (int i = 0; i < ROM_BANK_SIZE; i++) {
        EXPECT_EQ(_data[i], _mem.read(i));
    }   
}


TEST_F(MemoryTest, HandlesWrite) {
    int index = std::rand() % ROM_BANK_SIZE;

    uint8_t old_data = _mem.read(index);
    _mem.write(index, old_data + 1);

    EXPECT_NE(_mem.read(index), old_data);
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}