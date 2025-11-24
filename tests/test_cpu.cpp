#include "../src/cpu/cpu.h"
#include "../src/utils/logger.h"
#include "gtest/gtest.h"

#include <vector>


bool SKIP_INDIVIDUAL_TESTS = true;

TEST(IndividualTest, CPUTest01) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/01-special.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(IndividualTest, CPUTest02) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }
    
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/02-interrupts.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(IndividualTest, CPUTest03) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }
    
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(IndividualTest, CPUTest04) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }
    
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/04-op r,imm.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(IndividualTest, CPUTest05) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }
    
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/05-op rp.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(IndividualTest, CPUTest06) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }
    
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/06-ld r,r.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(IndividualTest, CPUTest07) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }
    
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(IndividualTest, CPUTest08) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }
    
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/08-misc instrs.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(IndividualTest, CPUTest09) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }
    
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/09-op r,r.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(IndividualTest, CPUTest10) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }
    
    std::string filename = "../../gb-test-roms/cpu_instrs/individual/10-bit ops.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(IndividualTest, CPUTest11) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb";
    // std::string filename = "../../gb-test-roms/instr_timing/instr_timing.gb";
    // std::string filename = "../../gb-test-roms/mem_timing/mem_timing.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
    }
    int i = 0;
    while(!cpu.is_stopped()) {
        cpu.step();
        i++;
        if (LOGGING) {
            logger.log();
        }
    }


    // logger.log_buffer();
    if (LOGGING) {
        logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(TimingTest, InstrTiming) {
    std::string filename = "../../gb-test-roms/instr_timing/instr_timing.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = true;
    CPULogger logger = CPULogger(&cpu);
    TimerLogger t_logger = TimerLogger(&timer);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
        t_logger.init_ofstream("../../build/timer_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
    );

    if (LOGGING) {
        logger.log();
        t_logger.log();
    }

    while(!cpu.is_stopped()) {
        cpu.step();

        if (LOGGING) {
            logger.log();
            t_logger.log();
        }
    }

    if (LOGGING) {
        logger.log_serial_buffer("../../build/buffer.txt");
        logger.teardown();
        t_logger.teardown();
    }
    EXPECT_EQ(cpu.serial_buffer.substr(cpu.serial_buffer.length() - 6), "Passed");
}

TEST(TimingTest, MemTiming) {
    GTEST_SKIP();
    std::string filename = "../../gb-test-roms/mem_timing/mem_timing.gb";

    Memory memory;
    Timer timer;
    CPU cpu;

    bool LOGGING = false;
    CPULogger logger = CPULogger(&cpu);
    if (LOGGING) {
        logger.init_ofstream("../../build/cpu_log.txt");
    }
    
    timer.init(&memory);
    cpu.init(&timer, &memory);

    EXPECT_NO_THROW(
        memory.load_rom(filename)
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