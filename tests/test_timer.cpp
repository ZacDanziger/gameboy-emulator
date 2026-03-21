#include "../src/timer/timer.h"
#include "../src/memory/mmu.h"
#include "gtest/gtest.h"


TEST(TimerTest, TestReadWrite) {
        MMU mmu;
    PPU ppu([&mmu](Interrupt i){ mmu.request_interrupt(i); },
            []{},
            [&mmu]{ mmu.hdma_tick(); });
    Timer timer([&mmu](Interrupt i){ mmu.request_interrupt(i); },
                [&ppu]{ ppu.update(); });

    EXPECT_NO_THROW(
        timer.write(TAC_REGISTER, 0x05);
        timer.write(TMA_REGISTER, 0xFF);
        timer.write(DIV_REGISTER, 0xFF);
    );

    EXPECT_THROW(
        timer.write(WRAM_BANK_00_START, 0x00), std::runtime_error
    );

    EXPECT_THROW(
        timer.read(WRAM_BANK_00_START), std::runtime_error
    );

    EXPECT_EQ(timer.read(DIV_REGISTER), 0x00);
    EXPECT_EQ(timer.read(TAC_REGISTER), 0x05);
    EXPECT_EQ(timer.read(TMA_REGISTER), 0xFF);
}


TEST(TimerTest, TestTick) {
    MMU mmu;
    PPU ppu([&mmu](Interrupt i){ mmu.request_interrupt(i); },
            []{},
            [&mmu]{ mmu.hdma_tick(); });
    Timer timer([&mmu](Interrupt i){ mmu.request_interrupt(i); },
                [&ppu]{ ppu.update(); });

    timer.write(TAC_REGISTER, 0x05);

    EXPECT_EQ(timer.read(TIMA_REGISTER), 0x00);

    for (int i = 0; i < 3; i++) {
        timer.tick();
    }

    EXPECT_EQ(timer.read(TIMA_REGISTER), 0x00);

    timer.tick();

    EXPECT_EQ(timer.read(TIMA_REGISTER), 0x01);
}

TEST(TimerTest, TestOverflow) {
    MMU mmu;
    PPU ppu([&mmu](Interrupt i){ mmu.request_interrupt(i); },
            []{},
            [&mmu]{ mmu.hdma_tick(); });
    Timer timer([&mmu](Interrupt i){ mmu.request_interrupt(i); },
                [&ppu]{ ppu.update(); });

    timer.write(TAC_REGISTER, 0x05);
    timer.write(TMA_REGISTER, 0xFF);
    timer.write(TIMA_REGISTER, 0xFF);

    for (int i = 0; i < 4; i++) {
        timer.tick();
    }

    EXPECT_EQ(timer.read(TIMA_REGISTER), 0x00);

    // expect interrupt request to be delayed 1 m-cycle
    EXPECT_EQ(mmu.read(IF_REGISTER), 0x00);
    timer.tick();
    EXPECT_EQ(mmu.read(IF_REGISTER), 0x04);

    // expect write during reload cycle to fail
    timer.write(TIMA_REGISTER, 0xDE);
    EXPECT_NE(timer.read(TIMA_REGISTER), 0xDE);

    mmu.write(IF_REGISTER, 0x00);

    // expect this to end on timer reload cycle
    for (int i = 0; i < 4; i++) {
        timer.tick();
    }

    // expect TMA write during reload cycle to also write to TIMA
    timer.write(TMA_REGISTER, 0xDE);
    EXPECT_EQ(timer.read(TIMA_REGISTER), 0xDE);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}