#include "../src/emulator/emulator.h"
#include "gtest/gtest.h"

TEST(SoundTest, DMGSound) {
    Emulator emulator;
    emulator.load("../../gb-test-roms/dmg_sound/dmg_sound.gb");

    emulator.run();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}