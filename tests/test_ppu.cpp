#include "../src/ppu/ppu.h"
#include "../src/memory/mmu.h"
#include "../src/cpu/cpu.h"
#include "../emulator/emulator.h"
#include "gtest/gtest.h"


TEST(ScreenTest, DMGAcid2) {
    std::string filename = "../../gb-test-roms/dmg-acid2.gb";

    Emulator emulator(filename);

    EXPECT_NO_THROW(
        emulator.run();
    );
}

TEST(ScreenTest, CGBAcid2) {
    std::string filename = "../../gb-test-roms/cgb-acid2.gbc";

    Emulator emulator(filename);

    EXPECT_NO_THROW(
        emulator.run();
    );
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}