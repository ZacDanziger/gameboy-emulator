#include "../src/cpu/cpu.h"
#include "../src/utils/logger.h"
#include "gtest/gtest.h"


TEST(BlarggROMTest, CPUInstructionTest) {
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/01-special.gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/02-interrupts.gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/04-op r,imm.gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/05-op rp.gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/06-ld r,r.gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/08-misc instrs.gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/09-op r,r.gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/10-bit ops.gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb";
    // std::string filename = "../../gb-test-roms/cpu_instrs/cpu_instrs.gb";
    // std::string filename = "../../gb-test-roms/mem_timing/individual/01-read_timing.gb";
    // std::string filename = "../../gb-test-roms/instr_timing/instr_timing.gb";
    std::string filename = "../../gb-test-roms/mem_timing/mem_timing.gb";

    Memory memory;
    Timer timer;
    CPU cpu;
    CPULogger logger = CPULogger(&cpu);
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
    }

    logger.log_buffer();
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}