#include "../src/ppu/ppu.h"
#include "../src/memory/mmu.h"
#include "../src/cpu/cpu.h"
#include "../emulator/emulator.h"
#include "gtest/gtest.h"

bool CGB_CHECK = true;

TEST(BasicTest, TestReadWrite) {
    GTEST_SKIP();

    MMU mmu;
    PPU ppu([&mmu](Interrupt i){ mmu.request_interrupt(i); },
            []{},
            [&mmu]{ mmu.hdma_tick(); });

    EXPECT_NO_THROW(
        ppu.write(LCDC_REGISTER, 0x01);
        ppu.write(STAT_REGISTER, 0x01);
        ppu.write(SCY_REGISTER, 0x01);
        ppu.write(SCX_REGISTER, 0x01);
        ppu.write(LY_REGISTER, 0x01);
        ppu.write(LYC_REGISTER, 0x01);
        ppu.write(DMA_REGISTER, 0x01);
        ppu.write(BGP_REGISTER, 0x01);
        ppu.write(OBP0_REGISTER, 0x01);
        ppu.write(OBP1_REGISTER, 0x01);
        ppu.write(WY_REGISTER, 0x01);
        ppu.write(WX_REGISTER, 0x01);
    );

    EXPECT_THROW(
        ppu.write(WRAM_BANK_00_START, 0x00), std::runtime_error
    );

    EXPECT_THROW(
        ppu.read(WRAM_BANK_00_START), std::runtime_error
    );

    EXPECT_EQ(ppu.read(LCDC_REGISTER), 0x01);
    EXPECT_EQ(ppu.read(STAT_REGISTER), 0x01);
    EXPECT_EQ(ppu.read(SCY_REGISTER), 0x01);
    EXPECT_EQ(ppu.read(SCX_REGISTER), 0x01);

    EXPECT_NE(ppu.read(LY_REGISTER), 0x01);

    EXPECT_EQ(ppu.read(LYC_REGISTER), 0x01);
    EXPECT_EQ(ppu.read(DMA_REGISTER), 0x01);
    EXPECT_EQ(ppu.read(BGP_REGISTER), 0x01);
    EXPECT_EQ(ppu.read(OBP0_REGISTER), 0x01);
    EXPECT_EQ(ppu.read(OBP1_REGISTER), 0x01);
    EXPECT_EQ(ppu.read(WY_REGISTER), 0x01);
    EXPECT_EQ(ppu.read(WX_REGISTER), 0x01);

}


TEST(ScreenTest, DMGAcid2) {
    if (CGB_CHECK) {
        GTEST_SKIP();
    }
    std::string filename = "../../gb-test-roms/dmg-acid2.gb";

    Emulator emulator(filename);
    emulator.run();
}

TEST(ScreenTest, CGBAcid2) {
    if (!CGB_CHECK) {
        GTEST_SKIP();
    }
    std::string filename = "../../gb-test-roms/cgb-acid2.gbc";

    Emulator emulator(filename);
    emulator.run();
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}