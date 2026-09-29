#include "../src/app/emulator.h"
#include "gtest/gtest.h"

bool RUN_CGB_TESTS = true;
Emulator emulator;

TEST(MooneyeAcceptance, add_sp_e_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/add_sp_e_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_div_S) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_div-S.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_div_dmg0) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_div-dmg0.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_div_dmgABCmgb) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_div-dmgABCmgb.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_div2_S) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_div2-S.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_hwio_S) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_hwio-S.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_hwio_dmg0) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_hwio-dmg0.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_hwio_dmgABCmgb) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_hwio-dmgABCmgb.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_regs_dmg0) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_regs-dmg0.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_regs_dmgABC) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_regs-dmgABC.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_regs_mgb) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_regs-mgb.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_regs_sgb) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_regs-sgb.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, boot_regs_sgb2) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/boot_regs-sgb2.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, call_cc_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/call_cc_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, call_cc_timing2) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/call_cc_timing2.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, call_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/call_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, call_timing2) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/call_timing2.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, di_timing_GS) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/di_timing-GS.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, div_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/div_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, ei_sequence) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ei_sequence.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, ei_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ei_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, halt_ime0_ei) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/halt_ime0_ei.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, halt_ime0_nointr_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/halt_ime0_nointr_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, halt_ime1_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/halt_ime1_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, halt_ime1_timing2_GS) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/halt_ime1_timing2-GS.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, if_ie_registers) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/if_ie_registers.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, intr_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/intr_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, jp_cc_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/jp_cc_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, jp_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/jp_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, ld_hl_sp_e_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ld_hl_sp_e_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, oam_dma_restart) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/oam_dma_restart.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, oam_dma_start) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/oam_dma_start.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, oam_dma_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/oam_dma_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, pop_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/pop_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, push_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/push_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, rapid_di_ei) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/rapid_di_ei.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, ret_cc_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ret_cc_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, ret_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ret_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, reti_intr_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/reti_intr_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, reti_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/reti_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeAcceptance, rst_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/rst_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, div_write) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/div_write.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, rapid_toggle) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/rapid_toggle.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tim00) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tim00.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tim00_div_trigger) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tim00_div_trigger.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tim01) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tim01.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tim01_div_trigger) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tim01_div_trigger.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tim10) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tim10.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tim10_div_trigger) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tim10_div_trigger.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tim11) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tim11.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tim11_div_trigger) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tim11_div_trigger.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tima_reload) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tima_reload.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tima_write_reloading) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tima_write_reloading.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeTimer, tma_write_reloading) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/timer/tma_write_reloading.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeBits, mem_oam) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/bits/mem_oam.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeBits, reg_f) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/bits/reg_f.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeBits, unused_hwio_GS) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/bits/unused_hwio-GS.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeInterrupts, ie_push) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/interrupts/ie_push.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeOam_dma, basic) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/oam_dma/basic.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeOam_dma, reg_read) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/oam_dma/reg_read.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeOam_dma, sources_GS) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/oam_dma/sources-GS.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, hblank_ly_scx_timing_GS) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/ppu/hblank_ly_scx_timing-GS.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, intr_1_2_timing_GS) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/ppu/intr_1_2_timing-GS.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, intr_2_0_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ppu/intr_2_0_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, intr_2_mode0_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ppu/intr_2_mode0_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, intr_2_mode0_timing_sprites) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ppu/intr_2_mode0_timing_sprites.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, intr_2_mode3_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ppu/intr_2_mode3_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, intr_2_oam_ok_timing) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ppu/intr_2_oam_ok_timing.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, lcdon_timing_GS) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/ppu/lcdon_timing-GS.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, lcdon_write_timing_GS) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/ppu/lcdon_write_timing-GS.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, stat_irq_blocking) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ppu/stat_irq_blocking.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, stat_lyc_onoff) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/ppu/stat_lyc_onoff.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyePpu, vblank_stat_intr_GS) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/ppu/vblank_stat_intr-GS.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeInstr, daa) {
    // Runs on both CGB and DMG

    std::string filename = "../../gb-test-roms/acceptance/instr/daa.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

TEST(MooneyeSerial, boot_sclk_align_dmgABCmgb) {
    if (RUN_CGB_TESTS) { GTEST_SKIP(); }

    std::string filename = "../../gb-test-roms/acceptance/serial/boot_sclk_align-dmgABCmgb.gb";

    emulator.load_rom(filename);

    EXPECT_NO_THROW(
        emulator.run()
    );
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
