#include "gtest/gtest.h"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <array>

#include "../src/emulator/emulator.h"

Emulator emulator;

TEST(TimingTest, MemTiming01) {
    std::string filename = "../../gb-test-roms/mem_timing/mem_timing.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run();
    );
}

TEST(TimingTest, MemTiming02) {
    std::string filename = "../../gb-test-roms/mem_timing-2/mem_timing.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run();
    );
} 


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}