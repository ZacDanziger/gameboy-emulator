#include "../src/memory/mmu.h"
#include <iostream>
#include "gtest/gtest.h"

#include <cstdlib>
#include <fstream>
#include <array>

#include "../src/utils/logger.h"

TEST(TimingTest, MemTiming01) {
    std::string filename = "../../gb-test-roms/mem_timing/individual/01-read_timing.gb";

    CPU cpu;
    MMU mmu;
    PPU ppu([&mmu](Interrupt i){ mmu.request_interrupt(i); },
            []{},
            [&mmu]{ mmu.hdma_tick(); });
    Timer timer([&mmu](Interrupt i){ mmu.request_interrupt(i); },
                [&ppu]{ ppu.update(); });

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    
    cpu.init(&timer, &mmu);
    mmu.init(&timer, &ppu);

    EXPECT_NO_THROW(
        mmu.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }

    while(!cpu.is_stopped()) {
        cpu.step();

        if (LOGGING) {
            logger.log();
        }
    }

    if (LOGGING) {
        logger.log_serial_buffer("../../build/buffer.txt");
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}