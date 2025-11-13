#include "../src/cpu/cpu.h"
#include "../src/utils/logger.h"
#include "gtest/gtest.h"


TEST(BlarggROMTest, CPUInstructionTest) {
    std::string filename = "../../gb-test-roms/cpu_instrs/cpu_instrs.gb";

    Memory memory;
    Timer timer;
    CPU cpu;
    // CPULogger logger = CPULogger(&cpu);
    // logger.update_filename("/Users/zacdanziger/Documents/01_Personal/Coding/gameboy-emulator/build/cpu_log.txt");
    
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        // logger.log_buffer();
        i++;
    }

    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}