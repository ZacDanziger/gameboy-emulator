#include "../src/emulator/emulator.h"
#include "gtest/gtest.h"


bool SKIP_INDIVIDUAL_TESTS = true;
Emulator emulator;

TEST(IndividualTest, CPUTest01) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/01-special.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(IndividualTest, CPUTest02) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/02-interrupts.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(IndividualTest, CPUTest03) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(IndividualTest, CPUTest04) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/04-op r,imm.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(IndividualTest, CPUTest05) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/05-op rp.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(IndividualTest, CPUTest06) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/06-ld r,r.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(IndividualTest, CPUTest07) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(IndividualTest, CPUTest08) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/08-misc instrs.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(IndividualTest, CPUTest09) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/09-op r,r.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(IndividualTest, CPUTest10) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/10-bit ops.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(IndividualTest, CPUTest11) {
    if (SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }

    std::string filename = "../../gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}


TEST(InstructionTest, CompositeTest) {
    if (!SKIP_INDIVIDUAL_TESTS) {
        GTEST_SKIP();
    }
    std::string filename = "../../gb-test-roms/cpu_instrs/cpu_instrs.gb";

    emulator.load(filename);

    EXPECT_NO_THROW(
        emulator.run();
    );
}





int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}