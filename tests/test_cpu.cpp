#include "../src/cpu/cpu.h"
#include "gtest/gtest.h"

class CPUTest : public testing::Test {
    protected:
        CPU _cpu;

        CPUTest() {
            _cpu.reg_B = 0x01;
            _cpu.reg_C = 0x02;
            _cpu.reg_D = 0x03;
            _cpu.reg_E = 0x04;
            _cpu.reg_H = 0xD0;
            _cpu.reg_L = 0x00;
            _cpu.reg_SP = 0x0000;
            _cpu.reg_PC = WRAM_BANK_00_START;
        }
        
};

TEST_F(CPUTest, TestConstructor) {
    EXPECT_EQ(_cpu.reg_B, 0x01);
    EXPECT_EQ(_cpu.reg_C, 0x02);
    EXPECT_EQ(_cpu.reg_D, 0x03);
    EXPECT_EQ(_cpu.reg_E, 0x04);
    EXPECT_EQ(_cpu.reg_H, 0xD0);
    EXPECT_EQ(_cpu.reg_L, 0x00);
    EXPECT_EQ(_cpu.reg_SP, 0x0000);
    EXPECT_EQ(_cpu.reg_PC, WRAM_BANK_00_START);
}

TEST_F(CPUTest, TestFlags) {
    _cpu.reg_F = 0;

    _cpu.update_flag(FLAG_ZERO, true);
    EXPECT_EQ(_cpu.reg_F, 0x80);

    _cpu.update_flag(FLAG_SUB, true);
    EXPECT_EQ(_cpu.reg_F, 0xC0);

    _cpu.update_flag(FLAG_HALF_CARRY, true);
    EXPECT_EQ(_cpu.reg_F, 0xE0);

    _cpu.update_flag(FLAG_CARRY, true);
    EXPECT_EQ(_cpu.reg_F, 0xF0);

    EXPECT_EQ(_cpu.get_flag(FLAG_ZERO), true);
    _cpu.update_flag(FLAG_ZERO, false);
    EXPECT_NE(_cpu.get_flag(FLAG_ZERO), true);
}

TEST_F(CPUTest, TestPair) {
    EXPECT_EQ(_cpu.get_pair(_cpu.BC), 0x0102);
    EXPECT_EQ(_cpu.get_pair(_cpu.DE), 0x0304);
    EXPECT_EQ(_cpu.get_pair(_cpu.HL), 0xD000);

    _cpu.set_pair(_cpu.BC, 0x1234);
    EXPECT_EQ(_cpu.reg_B, 0x12);
    EXPECT_EQ(_cpu.reg_C, 0x34);

    _cpu._memory.write(WRAM_BANK_01_START, 0xAB);
    EXPECT_EQ(_cpu.read_hl(), 0xAB);
}

TEST_F(CPUTest, TestFetch) {
    _cpu._memory.write(WRAM_BANK_00_START, 0xAD);
    EXPECT_EQ(_cpu.fetch(), 0xAD);

    _cpu._memory.write(WRAM_BANK_00_START + 1, 0xEF);
    _cpu._memory.write(WRAM_BANK_00_START + 2, 0xBE);
    EXPECT_EQ(_cpu.fetch16(), 0xBEEF);

    _cpu.reg_PC = 0xFFFF;
    _cpu.fetch();
    EXPECT_EQ(_cpu.reg_PC, 0x0000);
}

TEST_F(CPUTest, TestLoads) {
    _cpu.LD(_cpu.reg_B, _cpu.reg_C);
    EXPECT_EQ(_cpu.reg_B, _cpu.reg_C);

    _cpu._memory.write(_cpu.get_pair(_cpu.HL), 0xFF);
    _cpu.LD(_cpu.reg_B, _cpu.get_pair(_cpu.HL));
    EXPECT_EQ(_cpu.reg_B, 0xFF);

    _cpu.reg_B = 0xD0;
    _cpu.reg_C = 0x00;
    _cpu.LD(_cpu.get_pair(_cpu.BC), _cpu.reg_D);
    EXPECT_EQ(_cpu._memory.read(0xD000), _cpu.reg_D);

    _cpu._memory.write(0xFF80, 0xDA);
    _cpu.reg_B = 0x80;
    _cpu.LDH(_cpu.reg_B, true);
    EXPECT_EQ(_cpu.reg_A, 0xDA);
    _cpu.reg_B = 0x81;
    EXPECT_NE(_cpu._memory.read(0xFF81), 0xDA);
    _cpu.LDH(_cpu.reg_B, false);
    EXPECT_EQ(_cpu._memory.read(0xFF81), 0xDA);
    
    Word dead = 0xDEAD;
    _cpu.LD(_cpu.reg_SP, dead);
    EXPECT_EQ(_cpu.reg_SP, 0xDEAD);

    _cpu.LD(_cpu.BC, dead);
    EXPECT_EQ(_cpu.reg_B, 0xDE);
    EXPECT_EQ(_cpu.reg_C, 0xAD);

    _cpu.write_SP(dead);
    EXPECT_EQ(_cpu._memory.read(dead), 0xAD);
    EXPECT_EQ(_cpu._memory.read(dead + 1), 0xDE);
}

TEST_F(CPUTest, TestArithmetic) {
    _cpu.set_pair(_cpu.AF, 0);
    
    _cpu.ADD(_cpu.reg_B, false);
    EXPECT_EQ(_cpu.reg_A, _cpu.reg_B);

    _cpu.update_flag(FLAG_CARRY, true);

    _cpu.ADD(_cpu.reg_B, true);
    EXPECT_EQ(_cpu.reg_A, 0x03);
    
    _cpu.SUB(_cpu.reg_D, false);
    EXPECT_EQ(_cpu.reg_A, 0);
    EXPECT_EQ(_cpu.get_flag(FLAG_SUB), true);
    EXPECT_EQ(_cpu.get_flag(FLAG_ZERO), true);
    
    _cpu.update_flag(FLAG_CARRY, true);
    _cpu.SUB(_cpu.reg_E, true);
    EXPECT_EQ(_cpu.reg_A, 0xFB);
    EXPECT_EQ(_cpu.get_flag(FLAG_HALF_CARRY), true);
    EXPECT_EQ(_cpu.get_flag(FLAG_CARRY), true);

    _cpu.AND(0x02);
    EXPECT_EQ(_cpu.reg_A, 0x02);

    _cpu.XOR(_cpu.reg_D);
    EXPECT_EQ(_cpu.reg_A, 0x01);

    _cpu.OR(_cpu.reg_E);
    EXPECT_EQ(_cpu.reg_A, 0x05);

    EXPECT_NE(_cpu.get_flag(FLAG_SUB), true);
    _cpu.CP(_cpu.reg_H);
    EXPECT_EQ(_cpu.reg_A, 0x05);
    EXPECT_EQ(_cpu.get_flag(FLAG_SUB), true);

    _cpu.INC(_cpu.reg_A);
    EXPECT_EQ(_cpu.reg_A, 0x06);

    _cpu._memory.write(_cpu.get_pair(_cpu.HL), 0xFF);
    _cpu.INC_HL();
    EXPECT_EQ(_cpu._memory.read(_cpu.get_pair(_cpu.HL)), 0x00);

    _cpu.DEC_HL();
    EXPECT_EQ(_cpu._memory.read(_cpu.get_pair(_cpu.HL)), 0xFF);

    _cpu.DEC(_cpu.reg_A);
    EXPECT_EQ(_cpu.reg_A, 0x05);

    _cpu.ADD_HL(0x0EAD);
    EXPECT_EQ(_cpu.reg_H, 0xDE);
    EXPECT_EQ(_cpu.reg_L, 0xAD);

    _cpu.reg_SP = 0xBEEF;
    _cpu._memory.write(_cpu.reg_PC, 0xEF);
    EXPECT_EQ(_cpu.ADD_SP(), 0xBEDE);

    _cpu.INC(_cpu.reg_SP);
    EXPECT_EQ(_cpu.reg_SP, 0xBEF0);

    _cpu.reg_L = 0xFF;
    _cpu.INC(_cpu.HL);
    EXPECT_EQ(_cpu.get_pair(_cpu.HL), 0xDF00);

    _cpu.DEC(_cpu.reg_SP);
    EXPECT_EQ(_cpu.reg_SP, 0xBEEF);

    _cpu.DEC(_cpu.HL);
    EXPECT_EQ(_cpu.get_pair(_cpu.HL), 0xDEFF);
}

TEST_F(CPUTest, TestRotates) {
    _cpu.reg_A = 0x7F;
    _cpu.RLA(true);
    EXPECT_EQ(_cpu.reg_A, 0xFE);

    _cpu.update_flag(FLAG_CARRY, true);
    _cpu.RLA(false);
    EXPECT_EQ(_cpu.reg_A, 0xFD);

    _cpu.RRA(true);
    EXPECT_EQ(_cpu.reg_A, 0xFE);

    _cpu.RRA(false);
    EXPECT_EQ(_cpu.reg_A, 0xFF);

    _cpu.reg_B = 0x7F;
    _cpu.RL(_cpu.reg_B, true);
    EXPECT_EQ(_cpu.reg_B, 0xFE);

    _cpu.update_flag(FLAG_CARRY, true);
    _cpu.RL(_cpu.reg_B, false);
    EXPECT_EQ(_cpu.reg_B, 0xFD);

    _cpu.RR(_cpu.reg_B, true);
    EXPECT_EQ(_cpu.reg_B, 0xFE);

    _cpu.RR(_cpu.reg_B, false);
    EXPECT_EQ(_cpu.reg_B, 0xFF);

    _cpu.reg_C = 0xFE;
    _cpu.SLA(_cpu.reg_C);
    EXPECT_EQ(_cpu.reg_C, 0xFC);

    _cpu.LD(_cpu.get_pair(_cpu.HL), 0xFE);
    _cpu.SLA_HL();
    EXPECT_EQ(_cpu.read_hl(), 0xFC);

    _cpu.SRA(_cpu.reg_C);
    EXPECT_EQ(_cpu.reg_C, 0xFE);

    _cpu.SRA_HL();
    EXPECT_EQ(_cpu.read_hl(), 0xFE);

    _cpu.SRL(_cpu.reg_C);
    EXPECT_EQ(_cpu.reg_C, 0x7F);

    _cpu.SRL_HL();
    EXPECT_EQ(_cpu.read_hl(), 0x7F);
}

TEST_F(CPUTest, TestBitOps) {
    _cpu.reg_A = 0x10;
    _cpu.BIT(4, _cpu.reg_A);
    EXPECT_EQ(_cpu.get_flag(FLAG_ZERO), true);

    _cpu.BIT(3, _cpu.reg_A);
    EXPECT_EQ(_cpu.get_flag(FLAG_ZERO), false);

    _cpu.SET(3, _cpu.reg_A);
    _cpu.BIT(3, _cpu.reg_A);
    EXPECT_EQ(_cpu.get_flag(FLAG_ZERO), true);

    _cpu.RES(3, _cpu.reg_A);
    _cpu.BIT(3, _cpu.reg_A);
    EXPECT_EQ(_cpu.get_flag(FLAG_ZERO), false);

    _cpu.LD(_cpu.get_pair(_cpu.HL), 0x10);
    _cpu.SET_HL(3);
    _cpu.BIT(3, _cpu.read_hl());
    EXPECT_EQ(_cpu.get_flag(FLAG_ZERO), true);

    _cpu.RES_HL(3);
    _cpu.BIT(3, _cpu.read_hl());
    EXPECT_EQ(_cpu.get_flag(FLAG_ZERO), false);
}

TEST_F(CPUTest, TestJumps) {
    EXPECT_NE(_cpu.reg_PC, WRAM_BANK_01_START);
    _cpu.JP(WRAM_BANK_01_START);
    EXPECT_EQ(_cpu.reg_PC, WRAM_BANK_01_START);


    _cpu.LD(WRAM_BANK_01_START, 0xFF);  // -1
    _cpu.JR();
    EXPECT_EQ(_cpu.reg_PC, 0xCFFF);

    _cpu.reg_SP = 0xFFFE;
    _cpu.LD(0xCFFF, 0xC0);
    _cpu.CALL();
    EXPECT_EQ(_cpu.reg_PC, 0xFFC0);
    EXPECT_EQ(_cpu._memory.read(_cpu.reg_SP), 0xFF);
    EXPECT_EQ(_cpu._memory.read(_cpu.reg_SP + 1), 0xCF);
    _cpu.RET();
    EXPECT_EQ(_cpu.reg_PC, 0xCFFF);

    _cpu.RST(_cpu.interrupt_vector[1]);
    EXPECT_EQ(_cpu.reg_PC, 0x0008);
    EXPECT_EQ(_cpu.reg_SP, 0xFFFC);
    _cpu.RET();
    EXPECT_EQ(_cpu.reg_PC, 0xCFFF);
}

TEST_F(CPUTest, TestMisc) {
    _cpu.reg_A = 0xFE;
    _cpu.SWAP(_cpu.reg_A);
    EXPECT_EQ(_cpu.reg_A, 0xEF);

    _cpu.LD(_cpu.get_pair(_cpu.HL), 0xFE);
    _cpu.SWAP_HL();
    EXPECT_EQ(_cpu.read_hl(), 0xEF);

    _cpu.reg_F = 0;
    _cpu.reg_A = 0x42;
    _cpu.DAA();
    EXPECT_EQ(_cpu.reg_A, 0x42);

    _cpu.reg_A = 0xA1;
    _cpu.DAA();
    EXPECT_EQ(_cpu.reg_A, 0x01);
    EXPECT_TRUE(_cpu.get_flag(FLAG_CARRY));

    _cpu.update_flag(FLAG_CARRY, false);
    _cpu.reg_A = 0x9B;
    _cpu.DAA();
    EXPECT_EQ(_cpu.reg_A, 0x01);
    EXPECT_TRUE(_cpu.get_flag(FLAG_CARRY));

    _cpu.CPL();
    EXPECT_EQ(_cpu.reg_A, 0xFE);

    _cpu.reg_SP = 0xFFFE;
    _cpu.reg_F = 0x80;
    _cpu.PUSH(_cpu.AF);
    EXPECT_EQ(_cpu._memory.read(_cpu.reg_SP), _cpu.reg_F);
    EXPECT_EQ(_cpu._memory.read(_cpu.reg_SP + 1), _cpu.reg_A);

    _cpu.set_pair(_cpu.AF, 0x00);
    _cpu.POP(_cpu.AF);
    EXPECT_EQ(_cpu.reg_A, 0xFE);
    EXPECT_EQ(_cpu.reg_F, 0x80);
}

TEST_F(CPUTest, BlarggROMTests) {

}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}