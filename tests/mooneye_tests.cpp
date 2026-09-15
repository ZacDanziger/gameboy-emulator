#include "../src/core/emulation_core.h"
#include "gtest/gtest.h"

Emulator emulator;

TEST(TestBits, MemOAM) {
    GTEST_SKIP();
    emulator.load("../../gb-test-roms/acceptance/bits/mem_oam.gb");

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(TestBits, regF) {
    GTEST_SKIP();
    emulator.load("../../gb-test-roms/acceptance/bits/reg_f.gb");

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(TestInstr, DAA) {
    emulator.load("../../gb-test-roms/acceptance/instr/daa.gb");

    EXPECT_NO_THROW(
        emulator.run()
    );
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}