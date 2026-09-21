#include "../src/app/emulator.h"
#include "gtest/gtest.h"

bool SKIP_INDIVIDUAL_TESTS = true;
int SPECIFIC_TEST = 0;
Emulator emulator;

TEST(IndividualTest, CGBTest01) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 1) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/01-registers.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest02) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 2) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/02-len ctr.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest03) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 3 ) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/03-trigger.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest04) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 4) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/04-sweep.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest05) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 5) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/05-sweep details.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest06) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 6) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/06-overflow on trigger.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest07) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 7) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/07-len sweep period sync.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest08) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 8) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/08-len ctr during power.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest09) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 9) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/09-wave read while on.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest10) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 10) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/10-wave trigger while on.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest11) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 11) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/11-regs after power.gb");

    emulator.run();
}

TEST(IndividualTest, CGBTest12) {
    if (SKIP_INDIVIDUAL_TESTS && SPECIFIC_TEST != 12) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/rom_singles/12-wave.gb");

    emulator.run();
}

TEST(SoundTest, CompositeTest) {
    if (!SKIP_INDIVIDUAL_TESTS || SPECIFIC_TEST != 0) {
        GTEST_SKIP();
    }

    emulator.load_rom("../../gb-test-roms/cgb_sound/cgb_sound.gb");

    emulator.run();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}