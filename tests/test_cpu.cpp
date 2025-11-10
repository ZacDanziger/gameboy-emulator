#include "../src/cpu/cpu.h"
#include "gtest/gtest.h"


TEST(CPUTest, ROMTest_1) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/01-special.gb";

    CPU cpu;
    cpu.set_logfile_suffix("_1");

    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

TEST(CPUTest, ROMTest_2) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/02-interrupts.gb";

    CPU cpu;
    cpu.set_logfile_suffix("_2");

    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

TEST(CPUTest, ROMTest_3) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb";

    CPU cpu;
    cpu.set_logfile_suffix("_3");
    
    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

TEST(CPUTest, ROMTest_4) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/04-op r,imm.gb";

    CPU cpu;
    cpu.set_logfile_suffix("_4");
    
    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

TEST(CPUTest, ROMTest_5) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/05-op rp.gb";

    CPU cpu;
    cpu.set_logfile_suffix("_5");
    
    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

TEST(CPUTest, ROMTest_6) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/06-ld r,r.gb";

    CPU cpu;
    cpu.set_logfile_suffix("_6");
    
    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

TEST(CPUTest, ROMTest_7) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb";

    CPU cpu;
    cpu.set_logfile_suffix("_7");
    
    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

TEST(CPUTest, ROMTest_8) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/08-misc instrs.gb";

    CPU cpu;
    cpu.set_logfile_suffix("_8");
    
    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

TEST(CPUTest, ROMTest_9) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/09-op r,r.gb";

    CPU cpu;
    cpu.set_logfile_suffix("_9");
    
    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

TEST(CPUTest, ROMTest_10) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/10-bit ops.gb";

    CPU cpu;
    cpu.set_logfile_suffix("_10");
    
    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

TEST(CPUTest, ROMTest_11) {
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb";

    CPU cpu;
    cpu.set_logfile_suffix("_11");
    
    EXPECT_NO_THROW(
        cpu.load_rom(filename)
    );
    std::string output = "";
    cpu.write_registers();

    for (int i = 0; i < 2 * ROM_BANK_SIZE; i++) {
        cpu.step();
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    // testing::GTEST_FLAG(filter) = "ROMTest_1";
    return RUN_ALL_TESTS();
}