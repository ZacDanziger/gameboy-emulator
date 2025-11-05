#include "cpu.h"

/**
 * n8 - immediate 8-bit data
 * n16 - immediate little-endian 16-bit data
 * a8 - 8-bit unsigned data
 * a16 - little-endian 16-bit data
 * e8 - 8-bit signed data
*/

int CPU::decode_execute(Byte opcode) {
    switch(opcode) {
        case 0x00: { return 4; }                                   // NOP
        case 0x01: { LD(BC, fetch16()); return 12; }               // B <- n16[15:8], C <- n16[7:0]
        case 0x02: { LD(get_pair(BC), reg_A); return 8; }          // memory[BC] <- A
        case 0x03: { INC(BC); return 8; }                          // INC BC++
        case 0x04: { INC(reg_B); return 4; }                       // B++
        case 0x05: { DEC(reg_B); return 4; }                       // B--
        case 0x06: { LD(reg_B, fetch()); return 8; }               // B <- n8
        case 0x07: { RLA(true); return 4; }                        // RLCA
        case 0x08: { write_SP(fetch16()); return 20; }             // LD [a16], SP
        case 0x09: { ADD(get_pair(BC)); return 8; }                // HL += BC
        case 0x0A: { LD(reg_A, get_pair(BC)); return 8; }          // A <- memory[BC]
        case 0x0B: { DEC(BC); return 8; }                          // BC--
        case 0x0C: { INC(reg_C); return 4; }                       // C++
        case 0x0D: { DEC(reg_C); return 4; }                       // C--
        case 0x0E: { LD(reg_C, fetch()); return 4; }               // C <- n8
        case 0x0F: { RRA(true); return 4; }                        // RRCA

        case 0x10: {}   // STOP (n8 ???)
        case 0x11: { LD(DE, fetch16()); return 12; }               // D <- n16[15:8], E <- n16[7:0]
        case 0x12: { LD(get_pair(DE), reg_A); return 8; }          // memory[DE] <- A
        case 0x13: { INC(DE); return 8; }                          // DE++
        case 0x14: { INC(reg_D); return 4; }                       // D++
        case 0x15: { DEC(reg_D); return 4; }                       // D--
        case 0x16: { LD(reg_D, fetch()); return 8; }               // D <- n8
        case 0x17: { RLA(false); return 4; }                       // RLA
        case 0x18: {}   // JR e8
        case 0x19: { ADD(get_pair(DE)); return 8; }                // HL += DE
        case 0x1A: { LD(reg_A, get_pair(DE)); return 8; }          // A <- memory[DE]
        case 0x1B: { DEC(DE); return 8; }                          // DE--
        case 0x1C: { INC(reg_E); return 4; }                       // E++
        case 0x1D: { DEC(reg_E); return 4; }                       // E--
        case 0x1E: { LD(reg_E, fetch()); return 8; }               // E <- n8
        case 0x1F: { RRA(false); return 4; }                       // RRA

        case 0x20: {}   // JR NZ, e8
        case 0x21: { LD(HL, fetch16()); return 12; }               // LD HL, n16
        case 0x22: { _memory.write(get_pair(HL), reg_A); INC(HL); return 8; }    // memory[HL++] <- A
        case 0x23: { INC(HL); return 8; }                                // INC HL
        case 0x24: { INC(reg_H); return 4; }                             // H++
        case 0x25: { DEC(reg_H); return 4; }                             // H--
        case 0x26: { LD(reg_H, fetch()); return 8; }                     // H <- n8
        case 0x27: {}   // DAA
        case 0x28: {}   // JR Z, e8
        case 0x29: { ADD(get_pair(HL)); return 8; }                      // ADD HL, HL
        case 0x2A: { LD(reg_A, get_pair(HL)); INC(HL); return 8; }       // A <- memory[HL++]
        case 0x2B: { DEC(HL); return 8; }                                // HL--
        case 0x2C: { INC(reg_L); return 4; }                             // L++
        case 0x2D: { DEC(reg_L); return 4; }                             // L--
        case 0x2E: { LD(reg_L, fetch()); return 8; }                     // L <- n8
        case 0x2F: { CPL(); return 4; }                                  // A <- ~A

        case 0x30: {}   // JR NC, e8
        case 0x31: { LD(reg_SP, fetch16()); return 12; }                 //LD SP, n16
        case 0x32: { _memory.write(get_pair(HL), reg_A); DEC(HL); return 8; }    // memory[HL--] <- A
        case 0x33: { INC(reg_SP); return 8; }                            // SP++
        case 0x34: { INC_HL(); return 12; }                              // memory[HL]++
        case 0x35: { DEC_HL(); return 12; }                              // memory[HL]--
        case 0x36: { _memory.write(get_pair(HL), fetch()); return 12; }  // memory[HL] <- n8
        case 0x37: { SCF(); return 4; }                                  // {Z N H C} = {- 0 0 1}
        case 0x38: {}   // JR C, e8
        case 0x39: { ADD(reg_SP); return 8; }                            // HL += SP
        case 0x3A: { LD(reg_A, get_pair(HL)); DEC(HL); return 8; }       // A <- memory[HL--]
        case 0x3B: { DEC(reg_SP); return 8; }                            // SP--
        case 0x3C: { INC(reg_A); return 4; }                             // A++
        case 0x3D: { DEC(reg_A); return 4; }                             // A--
        case 0x3E: { LD(reg_A, fetch()); return 8; }                     // A <- n8
        case 0x3F: { CCF(); return 4; }                                  // CCF

        case 0x40: { return 4; }                                        // LD B, B - essentially a NOP
        case 0x41: { LD(reg_B, reg_C); return 4; }                      // B <- C
        case 0x42: { LD(reg_B, reg_D); return 4; }                      // B <- D
        case 0x43: { LD(reg_B, reg_E); return 4; }                      // B <- E
        case 0x44: { LD(reg_B, reg_H); return 4; }                      // B <- H
        case 0x45: { LD(reg_B, reg_L); return 4; }                      // B <- L
        case 0x46: { LD(reg_B, get_pair(HL)); return 8; }               // B <- memory[HL]
        case 0x47: { LD(reg_B, reg_A); return 4; }                      // B <- A
        case 0x48: { LD(reg_C, reg_B); return 4; }                      // C <- B
        case 0x49: { return 4; }                                        // LD C, C - essentially a NOP
        case 0x4A: { LD(reg_C, reg_D); return 4; }                      // C <- D
        case 0x4B: { LD(reg_C, reg_E); return 4; }                      // C <- E
        case 0x4C: { LD(reg_C, reg_H); return 4; }                      // C <- H
        case 0x4D: { LD(reg_C, reg_L); return 4; }                      // C <- L
        case 0x4E: { LD(reg_C, get_pair(HL)); return 8; }               // C <- memory[HL]
        case 0x4F: { LD(reg_C, reg_A); return 4; }                      // C <- A

        case 0x50: { LD(reg_D, reg_B); return 4; }                      // D <- B
        case 0x51: { LD(reg_D, reg_C); return 4; }                      // D <- C
        case 0x52: { return 4; }                                        // LD D, D - essentially a NOP
        case 0x53: { LD(reg_D, reg_E); return 4; }                      // D <- E
        case 0x54: { LD(reg_D, reg_H); return 4; }                      // D <- H
        case 0x55: { LD(reg_D, reg_L); return 4; }                      // D <- L
        case 0x56: { LD(reg_D, get_pair(HL)); return 8; }               // D <- memory[HL]
        case 0x57: { LD(reg_D, reg_A); return 4; }                      // D <- A
        case 0x58: { LD(reg_E, reg_B); return 4; }                      // E <- B
        case 0x59: { LD(reg_E, reg_C); return 4; }                      // E <- C
        case 0x5A: { LD(reg_E, reg_D); return 4; }                      // E <- D
        case 0x5B: {return 4; }                                         // LD E, E - essentially a NOP
        case 0x5C: { LD(reg_E, reg_H); return 4; }                      // E <- H
        case 0x5D: { LD(reg_E, reg_L); return 4; }                      // E <- L
        case 0x5E: { LD(reg_E, get_pair(HL)); return 8; }               // E <- memory[HL]
        case 0x5F: { LD(reg_E, reg_A); return 4; }                      // E <- A

        case 0x60: { LD(reg_H, reg_B); return 4; }                      // H <- B
        case 0x61: { LD(reg_H, reg_C); return 4; }                      // H <- C
        case 0x62: { LD(reg_H, reg_D); return 4; }                      // H <- D
        case 0x63: { LD(reg_H, reg_E); return 4; }                      // H <- E
        case 0x64: { return 4; }                                        // LD H, H - essentially a NOP
        case 0x65: { LD(reg_H, reg_L); return 4; }                      // H <- L
        case 0x66: { LD(reg_H, get_pair(HL)); return 8; }               // H <- memory[HL]
        case 0x67: { LD(reg_H, reg_A); return 4; }                      // H <- A
        case 0x68: { LD(reg_L, reg_B); return 4; }                      // L <- B
        case 0x69: { LD(reg_L, reg_C); return 4; }                      // L <- C
        case 0x6A: { LD(reg_L, reg_D); return 4; }                      // L <- D
        case 0x6B: { LD(reg_L, reg_E); return 4; }                      // L <- E
        case 0x6C: { LD(reg_L, reg_H); return 4; }                      // L <- H
        case 0x6D: { LD(reg_L, reg_L); return 4; }                      // L <- L
        case 0x6E: { LD(reg_L, get_pair(HL)); return 8; }               // L <- memory[HL]
        case 0x6F: { LD(reg_L, reg_A); return 4; }                      // L <- A

        case 0x70: { _memory.write(get_pair(HL), reg_B); return 8; }    // memory[HL] <- B
        case 0x71: { _memory.write(get_pair(HL), reg_C); return 8; }    // memory[HL] <- C
        case 0x72: { _memory.write(get_pair(HL), reg_D); return 8; }    // memory[HL] <- D
        case 0x73: { _memory.write(get_pair(HL), reg_E); return 8; }    // memory[HL] <- E
        case 0x74: { _memory.write(get_pair(HL), reg_H); return 8; }    // memory[HL] <- H
        case 0x75: { _memory.write(get_pair(HL), reg_L); return 8; }    // memory[HL] <- L
        case 0x76: {}   // HALT
        case 0x77: { _memory.write(get_pair(HL), reg_A); return 8; }    // memory[HL] <- A
        case 0x78: { LD(reg_A, reg_B); return 4; }                      // A <- B
        case 0x79: { LD(reg_A, reg_C); return 4; }                      // A <- C
        case 0x7A: { LD(reg_A, reg_D); return 4; }                      // A <- D
        case 0x7B: { LD(reg_A, reg_E); return 4; }                      // A <- E
        case 0x7C: { LD(reg_A, reg_H); return 4; }                      // A <- H
        case 0x7D: { LD(reg_A, reg_L); return 4; }                      // A <- L
        case 0x7E: { LD(reg_A, get_pair(HL)); return 8; }               // A <- memory[HL]
        case 0x7F: { return 4; }                                        // LD A, A - essentially a NOP

        case 0x80: { ADD(reg_B, false); return 4; }                     // A += B
        case 0x81: { ADD(reg_C, false); return 4; }                     // A += C
        case 0x82: { ADD(reg_D, false); return 4; }                     // A += D
        case 0x83: { ADD(reg_E, false); return 4; }                     // A += E
        case 0x84: { ADD(reg_H, false); return 4; }                     // A += H
        case 0x85: { ADD(reg_L, false); return 4; }                     // A += L
        case 0x86: { ADD(read_hl(), false); return 8; }                 // A += memory[HL]
        case 0x87: { ADD(reg_A, false); return 4; }                     // A += A
        case 0x88: { ADD(reg_B, true); return 4; }                      // A += (B + carry)
        case 0x89: { ADD(reg_C, true); return 4; }                      // A += (C + carry)
        case 0x8A: { ADD(reg_D, true); return 4; }                      // A += (D + carry)
        case 0x8B: { ADD(reg_E, true); return 4; }                      // A += (E + carry)
        case 0x8C: { ADD(reg_H, true); return 4; }                      // A += (H + carry)
        case 0x8D: { ADD(reg_L, true); return 4; }                      // A += (L + carry)
        case 0x8E: { ADD(read_hl(), true); return 8; }                  // A += (memory[HL] + carry)
        case 0x8F: { ADD(reg_A, true); return 4; }                      // A += (A + carry)

        case 0x90: { SUB(reg_B, false); return 4; }                     // A -= B
        case 0x91: { SUB(reg_C, false); return 4; }                     // A -= C
        case 0x92: { SUB(reg_D, false); return 4; }                     // A -= D
        case 0x93: { SUB(reg_E, false); return 4; }                     // A -= E
        case 0x94: { SUB(reg_H, false); return 4; }                     // A -= H
        case 0x95: { SUB(reg_L, false); return 4; }                     // A -= L
        case 0x96: { SUB(read_hl(), false); return 8; }                  // A -= memory[HL]
        case 0x97: { SUB(reg_A, false); return 4; }                     // A -= A
        case 0x98: { SUB(reg_B, true); return 4; }                      // A -= (B + carry)
        case 0x99: { SUB(reg_C, true); return 4; }                      // A -= (C + carry)
        case 0x9A: { SUB(reg_D, true); return 4; }                      // A -= (D + carry)
        case 0x9B: { SUB(reg_E, true); return 4; }                      // A -= (E + carry)
        case 0x9C: { SUB(reg_H, true); return 4; }                      // A -= (H + carry)
        case 0x9D: { SUB(reg_L, true); return 4; }                      // A -= (L + carry)
        case 0x9E: { SUB(read_hl(), true); return 8; }                   // A -= (memory[HL] + carry)
        case 0x9F: { SUB(reg_A, true); return 4; }                      // A -= (A + carry)

        case 0xA0: { AND(reg_B); return 4; }                            // A &= B
        case 0xA1: { AND(reg_C); return 4; }                            // A &= C
        case 0xA2: { AND(reg_D); return 4; }                            // A &= D
        case 0xA3: { AND(reg_E); return 4; }                            // A &= E
        case 0xA4: { AND(reg_H); return 4; }                            // A &= H
        case 0xA5: { AND(reg_L); return 4; }                            // A &= L
        case 0xA6: { AND(read_hl()); return 8; }                        // A &= memory[HL]
        case 0xA7: { AND(reg_A); return 4; }                            // A &= A
        case 0xA8: { XOR(reg_B); return 4; }                            // A ^= B
        case 0xA9: { XOR(reg_C); return 4; }                            // A ^= C
        case 0xAA: { XOR(reg_D); return 4; }                            // A ^= D
        case 0xAB: { XOR(reg_E); return 4; }                            // A ^= E
        case 0xAC: { XOR(reg_H); return 4; }                            // A ^= H
        case 0xAD: { XOR(reg_L); return 4; }                            // A ^= L
        case 0xAE: { XOR(read_hl()); return 8; }                        // A ^= memory[HL]
        case 0xAF: { XOR(reg_A); return 4; }                            // A ^= A

        case 0xB0: { OR(reg_B); return 4; }                             // A |= B
        case 0xB1: { OR(reg_C); return 4; }                             // A |= C
        case 0xB2: { OR(reg_D); return 4; }                             // A |= D
        case 0xB3: { OR(reg_E); return 4; }                             // A |= E
        case 0xB4: { OR(reg_H); return 4; }                             // A |= H
        case 0xB5: { OR(reg_L); return 4; }                             // A |= L
        case 0xB6: { OR(read_hl()); return 8; }                         // A |= memory[HL]
        case 0xB7: { OR(reg_A); return 4; }                             // A |= A
        case 0xB8: { CP(reg_B); return 4; }                             // Update flags for (A - B)
        case 0xB9: { CP(reg_C); return 4; }                             // Update flags for (A - C)
        case 0xBA: { CP(reg_D); return 4; }                             // Update flags for (A - D)
        case 0xBB: { CP(reg_E); return 4; }                             // Update flags for (A - E)
        case 0xBC: { CP(reg_H); return 4; }                             // Update flags for (A - H)
        case 0xBD: { CP(reg_L); return 4; }                             // Update flags for (A - L)
        case 0xBE: { CP(read_hl()); return 8; }                         // Update flags for (A - memory[HL])
        case 0xBF: { CP(reg_A); return 4; }                             // Update flags for (A - A)

        case 0xC0: {}   // RET NZ
        case 0xC1: {}   // POP BC
        case 0xC2: {}   // JP NZ, a16
        case 0xC3: {}   // JP a16
        case 0xC4: {}   // CALL NZ, a16
        case 0xC5: {}   // PUSH BC
        case 0xC6: { ADD(fetch(), false); return 8; }   // A += n8
        case 0xC7: {}   // RST $00
        case 0xC8: {}   // RET Z
        case 0xC9: {}   // RET
        case 0xCA: {}   // JP Z, a16
        case 0xCB: { return decode_execute_cb(); }   // PREFIX
        case 0xCC: {}   // CALL Z, a16
        case 0xCD: {}   // CALL a16
        case 0xCE: { ADD(fetch(), true); return 8; }    // A += (n8 + carry)
        case 0xCF: {}   // RST $08

        case 0xD0: {}   // RET NC
        case 0xD1: {}   // POP DE
        case 0xD2: {}   // JP NC, a16
        case 0xD3: {}   // --- BAD ---
        case 0xD4: {}   // CALL NC, a16
        case 0xD5: {}   // PUSH DE
        case 0xD6: { SUB(fetch(), false); return 8; }   // A -= n8
        case 0xD7: {}   // RST $10
        case 0xD8: {}   // RET C
        case 0xD9: {}   // RETI
        case 0xDA: {}   // JP C, a16
        case 0xDB: {}   // --- BAD ---
        case 0xDC: {}   // CALL C, a16
        case 0xDD: {}   // --- BAD ---
        case 0xDE: { SUB(fetch(), true); return 8; }    // A -= (n8 + carry)
        case 0xDF: {}   // RST $18

        case 0xE0: { LDH(fetch(), false); return 12; }  // memory[$FF00 + n8] <- A
        case 0xE1: {}   // POP HL
        case 0xE2: { LDH(reg_C, false); return 8; }     // memory[$FF00 + C] <- A
        case 0xE3: {}   // --- BAD ---
        case 0xE4: {}   // --- BAD ---
        case 0xE5: {}   // PUSH HL
        case 0xE6: { AND(fetch()); return 8; }      // A &= n8
        case 0xE7: {}   // RST $20
        case 0xE8: { reg_SP = ADD(); return 16; }   // SP += e8
        case 0xE9: {}   // JP HL
        case 0xEA: { LD(fetch16(), reg_A); return 16; }   // memory[n16] <- A
        case 0xEB: {}   // --- BAD ---
        case 0xEC: {}   // --- BAD ---
        case 0xED: {}   // --- BAD ---
        case 0xEE: { XOR(fetch()); return 8; }  // A ^= n8
        case 0xEF: {}   //RST $28

        case 0xF0: { LDH(fetch(), true); return 12; }   // A <- memory[$FF00 + n8]
        case 0xF1: {}   // POP AF
        case 0xF2: { LDH(reg_C, true); return 8; }  // A <- memory[$FF00 + C]
        case 0xF3: {}   // DI
        case 0xF4: {}   // --- BAD ---
        case 0xF5: {}   // PUSH AF
        case 0xF6: { OR(fetch()); return 8; }   // A |= n8
        case 0xF7: {}   // RST $30
        case 0xF8: { LD(HL, ADD()); return 12; }   // HL <- SP + e8
        case 0xF9: { LD(reg_SP, get_pair(HL)); return 8; }   // LD SP, HL
        case 0xFA: { LD(reg_A, fetch16()); return 16; }   // A <- memory[n16]
        case 0xFB: {}   // EI
        case 0xFC: {}   // --- BAD ---
        case 0xFD: {}   // --- BAD ---
        case 0xFE: { CP(fetch()); return 8; }   
        case 0xFF: {}   // RST $38

        default: {throw std::runtime_error("Invalid Opcode"); }
    }
}

int CPU::decode_execute_cb() {
    Byte opcode = fetch();

    switch (opcode) {
        case 0x00: {}
        case 0x01: {}
        case 0x02: {}
        case 0x03: {}
        case 0x04: {}
        case 0x05: {}
        case 0x06: {}
        case 0x07: {}
        case 0x08: {}
        case 0x09: {}
        case 0x0A: {}
        case 0x0B: {}
        case 0x0C: {}
        case 0x0D: {}
        case 0x0E: {}
        case 0x0F: {}
        case 0x10: {}
        case 0x11: {}
        case 0x12: {}
        case 0x13: {}
        case 0x14: {}
        case 0x15: {}
        case 0x16: {}
        case 0x17: {}
        case 0x18: {}
        case 0x19: {}
        case 0x1A: {}
        case 0x1B: {}
        case 0x1C: {}
        case 0x1D: {}
        case 0x1E: {}
        case 0x1F: {}
        case 0x20: {}
        case 0x21: {}
        case 0x22: {}
        case 0x23: {}
        case 0x24: {}
        case 0x25: {}
        case 0x26: {}
        case 0x27: {}
        case 0x28: {}
        case 0x29: {}
        case 0x2A: {}
        case 0x2B: {}
        case 0x2C: {}
        case 0x2D: {}
        case 0x2E: {}
        case 0x2F: {}
        case 0x30: {}
        case 0x31: {}
        case 0x32: {}
        case 0x33: {}
        case 0x34: {}
        case 0x35: {}
        case 0x36: {}
        case 0x37: {}
        case 0x38: {}
        case 0x39: {}
        case 0x3A: {}
        case 0x3B: {}
        case 0x3C: {}
        case 0x3D: {}
        case 0x3E: {}
        case 0x3F: {}
        case 0x40: {}
        case 0x41: {}
        case 0x42: {}
        case 0x43: {}
        case 0x44: {}
        case 0x45: {}
        case 0x46: {}
        case 0x47: {}
        case 0x48: {}
        case 0x49: {}
        case 0x4A: {}
        case 0x4B: {}
        case 0x4C: {}
        case 0x4D: {}
        case 0x4E: {}
        case 0x4F: {}
        case 0x50: {}
        case 0x51: {}
        case 0x52: {}
        case 0x53: {}
        case 0x54: {}
        case 0x55: {}
        case 0x56: {}
        case 0x57: {}
        case 0x58: {}
        case 0x59: {}
        case 0x5A: {}
        case 0x5B: {}
        case 0x5C: {}
        case 0x5D: {}
        case 0x5E: {}
        case 0x5F: {}
        case 0x60: {}
        case 0x61: {}
        case 0x62: {}
        case 0x63: {}
        case 0x64: {}
        case 0x65: {}
        case 0x66: {}
        case 0x67: {}
        case 0x68: {}
        case 0x69: {}
        case 0x6A: {}
        case 0x6B: {}
        case 0x6C: {}
        case 0x6D: {}
        case 0x6E: {}
        case 0x6F: {}
        case 0x70: {}
        case 0x71: {}
        case 0x72: {}
        case 0x73: {}
        case 0x74: {}
        case 0x75: {}
        case 0x76: {}
        case 0x77: {}
        case 0x78: {}
        case 0x79: {}
        case 0x7A: {}
        case 0x7B: {}
        case 0x7C: {}
        case 0x7D: {}
        case 0x7E: {}
        case 0x7F: {}
        case 0x80: {}
        case 0x81: {}
        case 0x82: {}
        case 0x83: {}
        case 0x84: {}
        case 0x85: {}
        case 0x86: {}
        case 0x87: {}
        case 0x88: {}
        case 0x89: {}
        case 0x8A: {}
        case 0x8B: {}
        case 0x8C: {}
        case 0x8D: {}
        case 0x8E: {}
        case 0x8F: {}
        case 0x90: {}
        case 0x91: {}
        case 0x92: {}
        case 0x93: {}
        case 0x94: {}
        case 0x95: {}
        case 0x96: {}
        case 0x97: {}
        case 0x98: {}
        case 0x99: {}
        case 0x9A: {}
        case 0x9B: {}
        case 0x9C: {}
        case 0x9D: {}
        case 0x9E: {}
        case 0x9F: {}
        case 0xA0: {}
        case 0xA1: {}
        case 0xA2: {}
        case 0xA3: {}
        case 0xA4: {}
        case 0xA5: {}
        case 0xA6: {}
        case 0xA7: {}
        case 0xA8: {}
        case 0xA9: {}
        case 0xAA: {}
        case 0xAB: {}
        case 0xAC: {}
        case 0xAD: {}
        case 0xAE: {}
        case 0xAF: {}
        case 0xB0: {}
        case 0xB1: {}
        case 0xB2: {}
        case 0xB3: {}
        case 0xB4: {}
        case 0xB5: {}
        case 0xB6: {}
        case 0xB7: {}
        case 0xB8: {}
        case 0xB9: {}
        case 0xBA: {}
        case 0xBB: {}
        case 0xBC: {}
        case 0xBD: {}
        case 0xBE: {}
        case 0xBF: {}
        case 0xC0: {}
        case 0xC1: {}
        case 0xC2: {}
        case 0xC3: {}
        case 0xC4: {}
        case 0xC5: {}
        case 0xC6: {}
        case 0xC7: {}
        case 0xC8: {}
        case 0xC9: {}
        case 0xCA: {}
        case 0xCB: {}
        case 0xCC: {}
        case 0xCD: {}
        case 0xCE: {}
        case 0xCF: {}
        case 0xD0: {}
        case 0xD1: {}
        case 0xD2: {}
        case 0xD3: {}
        case 0xD4: {}
        case 0xD5: {}
        case 0xD6: {}
        case 0xD7: {}
        case 0xD8: {}
        case 0xD9: {}
        case 0xDA: {}
        case 0xDB: {}
        case 0xDC: {}
        case 0xDD: {}
        case 0xDE: {}
        case 0xDF: {}
        case 0xE0: {}
        case 0xE1: {}
        case 0xE2: {}
        case 0xE3: {}
        case 0xE4: {}
        case 0xE5: {}
        case 0xE6: {}
        case 0xE7: {}
        case 0xE8: {}
        case 0xE9: {}
        case 0xEA: {}
        case 0xEB: {}
        case 0xEC: {}
        case 0xED: {}
        case 0xEE: {}
        case 0xEF: {}
        case 0xF0: {}
        case 0xF1: {}
        case 0xF2: {}
        case 0xF3: {}
        case 0xF4: {}
        case 0xF5: {}
        case 0xF6: {}
        case 0xF7: {}
        case 0xF8: {}
        case 0xF9: {}
        case 0xFA: {}
        case 0xFB: {}
        case 0xFC: {}
        case 0xFD: {}
        case 0xFE: {}
        case 0xFF: {}

        default: { throw std::runtime_error("Invalid Opcode"); }
    }
}