#include "../src/core/emulation_core.h"
#include "gtest/gtest.h"

Emulator emulator;

TEST(TimingTest, InstrTiming) {
    // GTEST_SKIP();
    std::string filename = "../../gb-test-roms/instr_timing/instr_timing.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(TimingTests, InterruptTime) {
    std::string filename = "../../gb-test-roms/instr_timing/instr_timing.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run();
    );
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}