#include "../src/core/emulation_core.h"
#include "gtest/gtest.h"

Emulator emulator;

TEST(ScreenTest, DMGAcid2) {
    std::string filename = "../../gb-test-roms/dmg-acid2.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run();
    );
}

TEST(ScreenTest, CGBAcid2) {
    std::string filename = "../../gb-test-roms/cgb-acid2.gbc";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run();
    );
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}