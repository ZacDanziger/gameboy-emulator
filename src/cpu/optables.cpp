#include "cpu.h"

/*
 * n8 - immediate 8-bit data
 * n16 - immediate little-endian 16-bit data
 * a8 - 8-bit unsigned data
 * a16 - little-endian 16-bit data
 * e8 - 8-bit signed data
*/

/**
 * Matches opcode to operation and executes that operation
 * 
 * @param opcode an 8-bit opcode to be executed
 * @return the number of t-states taken to execute the command
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
        case 0x09: { ADD_HL(get_pair(BC)); return 8; }             // HL += BC
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
        case 0x18: { JR(); return 12; }                            // SP += e8
        case 0x19: { ADD_HL(get_pair(DE)); return 8; }             // HL += DE
        case 0x1A: { LD(reg_A, get_pair(DE)); return 8; }          // A <- memory[DE]
        case 0x1B: { DEC(DE); return 8; }                          // DE--
        case 0x1C: { INC(reg_E); return 4; }                       // E++
        case 0x1D: { DEC(reg_E); return 4; }                       // E--
        case 0x1E: { LD(reg_E, fetch()); return 8; }               // E <- n8
        case 0x1F: { RRA(false); return 4; }                       // RRA

        case 0x20: { if(!get_flag(FLAG_ZERO)) {JR(); return 12;} return 8; }    // JR NZ, e8
        case 0x21: { LD(HL, fetch16()); return 12; }                            // LD HL, n16
        case 0x22: { LD(get_pair(HL), reg_A); INC(HL); return 8; }              // memory[HL++] <- A
        case 0x23: { INC(HL); return 8; }                                       // INC HL
        case 0x24: { INC(reg_H); return 4; }                                    // H++
        case 0x25: { DEC(reg_H); return 4; }                                    // H--
        case 0x26: { LD(reg_H, fetch()); return 8; }                            // H <- n8
        case 0x27: { DAA(); return 4; }                                         // DAA
        case 0x28: { if(get_flag(FLAG_ZERO)) {JR(); return 12;} return 8; }     // JR Z, e8
        case 0x29: { ADD_HL(get_pair(HL)); return 8; }                          // ADD HL, HL
        case 0x2A: { LD(reg_A, get_pair(HL)); INC(HL); return 8; }              // A <- memory[HL++]
        case 0x2B: { DEC(HL); return 8; }                                       // HL--
        case 0x2C: { INC(reg_L); return 4; }                                    // L++
        case 0x2D: { DEC(reg_L); return 4; }                                    // L--
        case 0x2E: { LD(reg_L, fetch()); return 8; }                            // L <- n8
        case 0x2F: { CPL(); return 4; }                                         // A <- ~A

        case 0x30: { if(!get_flag(FLAG_CARRY)) {JR(); return 12;} return 8; }   // JR NC, e8
        case 0x31: { LD(reg_SP, fetch16()); return 12; }                        // LD SP, n16
        case 0x32: { LD(get_pair(HL), reg_A); DEC(HL); return 8; }              // memory[HL--] <- A
        case 0x33: { INC(reg_SP); return 8; }                                   // SP++
        case 0x34: { INC_HL(); return 12; }                                     // memory[HL]++
        case 0x35: { DEC_HL(); return 12; }                                     // memory[HL]--
        case 0x36: { LD(get_pair(HL), fetch()); return 12; }                    // memory[HL] <- n8
        case 0x37: { SCF(); return 4; }                                         // {Z N H C} = {- 0 0 1}
        case 0x38: { if(get_flag(FLAG_CARRY)) {JR(); return 12;} return 8; }    // JR C, e8
        case 0x39: { ADD_HL(reg_SP); return 8; }                                // HL += SP
        case 0x3A: { LD(reg_A, get_pair(HL)); DEC(HL); return 8; }              // A <- memory[HL--]
        case 0x3B: { DEC(reg_SP); return 8; }                                   // SP--
        case 0x3C: { INC(reg_A); return 4; }                                    // A++
        case 0x3D: { DEC(reg_A); return 4; }                                    // A--
        case 0x3E: { LD(reg_A, fetch()); return 8; }                            // A <- n8
        case 0x3F: { CCF(); return 4; }                                         // CCF

        case 0x40: { return 4; }                                                // LD B, B - essentially a NOP
        case 0x41: { LD(reg_B, reg_C); return 4; }                              // B <- C
        case 0x42: { LD(reg_B, reg_D); return 4; }                              // B <- D
        case 0x43: { LD(reg_B, reg_E); return 4; }                              // B <- E
        case 0x44: { LD(reg_B, reg_H); return 4; }                              // B <- H
        case 0x45: { LD(reg_B, reg_L); return 4; }                              // B <- L
        case 0x46: { LD(reg_B, get_pair(HL)); return 8; }                       // B <- memory[HL]
        case 0x47: { LD(reg_B, reg_A); return 4; }                              // B <- A
        case 0x48: { LD(reg_C, reg_B); return 4; }                              // C <- B
        case 0x49: { return 4; }                                                // LD C, C - essentially a NOP
        case 0x4A: { LD(reg_C, reg_D); return 4; }                              // C <- D
        case 0x4B: { LD(reg_C, reg_E); return 4; }                              // C <- E
        case 0x4C: { LD(reg_C, reg_H); return 4; }                              // C <- H
        case 0x4D: { LD(reg_C, reg_L); return 4; }                              // C <- L
        case 0x4E: { LD(reg_C, get_pair(HL)); return 8; }                       // C <- memory[HL]
        case 0x4F: { LD(reg_C, reg_A); return 4; }                              // C <- A

        case 0x50: { LD(reg_D, reg_B); return 4; }                              // D <- B
        case 0x51: { LD(reg_D, reg_C); return 4; }                              // D <- C
        case 0x52: { return 4; }                                                // LD D, D - essentially a NOP
        case 0x53: { LD(reg_D, reg_E); return 4; }                              // D <- E
        case 0x54: { LD(reg_D, reg_H); return 4; }                              // D <- H
        case 0x55: { LD(reg_D, reg_L); return 4; }                              // D <- L
        case 0x56: { LD(reg_D, get_pair(HL)); return 8; }                       // D <- memory[HL]
        case 0x57: { LD(reg_D, reg_A); return 4; }                              // D <- A
        case 0x58: { LD(reg_E, reg_B); return 4; }                              // E <- B
        case 0x59: { LD(reg_E, reg_C); return 4; }                              // E <- C
        case 0x5A: { LD(reg_E, reg_D); return 4; }                              // E <- D
        case 0x5B: {return 4; }                                                 // LD E, E - essentially a NOP
        case 0x5C: { LD(reg_E, reg_H); return 4; }                              // E <- H
        case 0x5D: { LD(reg_E, reg_L); return 4; }                              // E <- L
        case 0x5E: { LD(reg_E, get_pair(HL)); return 8; }                       // E <- memory[HL]
        case 0x5F: { LD(reg_E, reg_A); return 4; }                              // E <- A

        case 0x60: { LD(reg_H, reg_B); return 4; }                              // H <- B
        case 0x61: { LD(reg_H, reg_C); return 4; }                              // H <- C
        case 0x62: { LD(reg_H, reg_D); return 4; }                              // H <- D
        case 0x63: { LD(reg_H, reg_E); return 4; }                              // H <- E
        case 0x64: { return 4; }                                                // LD H, H - essentially a NOP
        case 0x65: { LD(reg_H, reg_L); return 4; }                              // H <- L
        case 0x66: { LD(reg_H, get_pair(HL)); return 8; }                       // H <- memory[HL]
        case 0x67: { LD(reg_H, reg_A); return 4; }                              // H <- A
        case 0x68: { LD(reg_L, reg_B); return 4; }                              // L <- B
        case 0x69: { LD(reg_L, reg_C); return 4; }                              // L <- C
        case 0x6A: { LD(reg_L, reg_D); return 4; }                              // L <- D
        case 0x6B: { LD(reg_L, reg_E); return 4; }                              // L <- E
        case 0x6C: { LD(reg_L, reg_H); return 4; }                              // L <- H
        case 0x6D: { LD(reg_L, reg_L); return 4; }                              // L <- L
        case 0x6E: { LD(reg_L, get_pair(HL)); return 8; }                       // L <- memory[HL]
        case 0x6F: { LD(reg_L, reg_A); return 4; }                              // L <- A

        case 0x70: { LD(get_pair(HL), reg_B); return 8; }               // memory[HL] <- B
        case 0x71: { LD(get_pair(HL), reg_C); return 8; }               // memory[HL] <- C
        case 0x72: { LD(get_pair(HL), reg_D); return 8; }               // memory[HL] <- D
        case 0x73: { LD(get_pair(HL), reg_E); return 8; }               // memory[HL] <- E
        case 0x74: { LD(get_pair(HL), reg_H); return 8; }               // memory[HL] <- H
        case 0x75: { LD(get_pair(HL), reg_L); return 8; }               // memory[HL] <- L
        case 0x76: { HALT(); return 4; }                                // HALT
        case 0x77: { LD(get_pair(HL), reg_A); return 8; }               // memory[HL] <- A
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
        case 0x96: { SUB(read_hl(), false); return 8; }                 // A -= memory[HL]
        case 0x97: { SUB(reg_A, false); return 4; }                     // A -= A
        case 0x98: { SUB(reg_B, true); return 4; }                      // A -= (B + carry)
        case 0x99: { SUB(reg_C, true); return 4; }                      // A -= (C + carry)
        case 0x9A: { SUB(reg_D, true); return 4; }                      // A -= (D + carry)
        case 0x9B: { SUB(reg_E, true); return 4; }                      // A -= (E + carry)
        case 0x9C: { SUB(reg_H, true); return 4; }                      // A -= (H + carry)
        case 0x9D: { SUB(reg_L, true); return 4; }                      // A -= (L + carry)
        case 0x9E: { SUB(read_hl(), true); return 8; }                  // A -= (memory[HL] + carry)
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

        case 0xC0: { if(!get_flag(FLAG_ZERO)) {RET(); return 20;} return 8; }               // RET NZ
        case 0xC1: { POP(BC); return 12; }                                                  // C <- memory[++SP]; B <- memory[++SP]
        case 0xC2: { if(!get_flag(FLAG_ZERO)) {JP(fetch16()); return 16;} return 12; }      // JP NZ, a16
        case 0xC3: { JP(fetch16()); return 16; }                                            // JP a16
        case 0xC4: { if(!get_flag(FLAG_ZERO)) {CALL(); return 24;} return 12; }             // CALL NZ, a16
        case 0xC5: { PUSH(BC); return 16; }                                                 // memory[--SP] <- B; memory[--SP] <- C
        case 0xC6: { ADD(fetch(), false); return 8; }                                       // A += n8
        case 0xC7: { RST(interrupt_vector[0]); return 16; }                                 // memory[SP] <- PC; PC = 0x0000
        case 0xC8: { if(get_flag(FLAG_ZERO)) {RET(); return 20;} return 8; }                // RET Z
        case 0xC9: { RET(); return 16; }                                                    // RET
        case 0xCA: { if(get_flag(FLAG_ZERO)) {JP(fetch16()); return 16;} return 12; }       // JP Z, a16
        case 0xCB: { return decode_execute_cb(); }                                          // PREFIX
        case 0xCC: { if(get_flag(FLAG_ZERO)) {CALL(); return 24;} return 12; }              // CALL Z, a16
        case 0xCD: { CALL(); return 24; }                                                   // CALL a16
        case 0xCE: { ADD(fetch(), true); return 8; }                                        // A += (n8 + carry)
        case 0xCF: { RST(interrupt_vector[1]); return 16; }                                 // memory[SP] <- PC; PC = 0x0008

        case 0xD0: { if(!get_flag(FLAG_CARRY)) {RET(); return 20;} return 8; }              // RET NC
        case 0xD1: { POP(DE); return 12; }                                                  // E <- memory[++SP]; D <- memory[++SP]
        case 0xD2: { if(!get_flag(FLAG_CARRY)) {JP(fetch16()); return 16;} return 12; }     // JP NC, a16
        case 0xD3: {}   // --- BAD ---
        case 0xD4: { if(!get_flag(FLAG_CARRY)) {CALL(); return 24;} return 12; }            // CALL NC, a16
        case 0xD5: { PUSH(DE); return 16; }                                                 // memory[--SP] <- D; memory[--SP] <- E
        case 0xD6: { SUB(fetch(), false); return 8; }                                       // A -= n8
        case 0xD7: { RST(interrupt_vector[2]); return 16; }                                 //  memory[SP] <- PC; PC = 0x0000
        case 0xD8: { if(get_flag(FLAG_CARRY)) {RET(); return 20;} return 8; }               // RET C
        case 0xD9: { RET(); EI(); return 16; }                                              // RETI
        case 0xDA: { if(get_flag(FLAG_CARRY)) {JP(fetch16()); return 16;} return 12; }      // JP C, a16
        case 0xDB: {}   // --- BAD ---
        case 0xDC: { if(get_flag(FLAG_CARRY)) {CALL(); return 24;} return 12; }             // CALL C, a16
        case 0xDD: {}   // --- BAD ---
        case 0xDE: { SUB(fetch(), true); return 8; }                                        // A -= (n8 + carry)
        case 0xDF: { RST(interrupt_vector[3]); return 16; }                                 // memory[SP] <- PC; PC = 0x0018

        case 0xE0: { LDH(fetch(), false); return 12; }                                      // memory[$FF00 + n8] <- A
        case 0xE1: { POP(HL); return 12; }                                                  // L <- memory[++SP]; H <- memory[++SP]
        case 0xE2: { LDH(reg_C, false); return 8; }                                         // memory[$FF00 + C] <- A
        case 0xE3: {}   // --- BAD ---
        case 0xE4: {}   // --- BAD ---
        case 0xE5: {PUSH(HL); return 16; }                                                  // memory[--SP] <- H; memory[--SP] <- L
        case 0xE6: { AND(fetch()); return 8; }                                              // A &= n8
        case 0xE7: { RST(interrupt_vector[4]); return 16; }                                 // memory[SP] <- PC; PC = 0x0020
        case 0xE8: { reg_SP = ADD_SP(); return 16; }                                        // SP += e8
        case 0xE9: { JP(get_pair(HL)); return 4; }                                          // SP <- HL
        case 0xEA: { LD(fetch16(), reg_A); return 16; }                                     // memory[n16] <- A
        case 0xEB: {}   // --- BAD ---
        case 0xEC: {}   // --- BAD ---
        case 0xED: {}   // --- BAD ---
        case 0xEE: { XOR(fetch()); return 8; }                                              // A ^= n8
        case 0xEF: { RST(interrupt_vector[5]); return 16; }                                 // memory[SP] <- PC; PC = 0x0028

        case 0xF0: { LDH(fetch(), true); return 12; }                                       // A <- memory[$FF00 + n8]
        case 0xF1: { POP(AF); return 12; }                                                  // F <- memory[++SP]; A <- memory[++SP]
        case 0xF2: { LDH(reg_C, true); return 8; }                                          // A <- memory[$FF00 + C]
        case 0xF3: { DI(); return 4; }                                                      // DI
        case 0xF4: {}   // --- BAD ---
        case 0xF5: { PUSH(AF); return 16; }                                                 // memory[--SP] <- A; memory[--SP] <- F
        case 0xF6: { OR(fetch()); return 8; }                                               // A |= n8
        case 0xF7: { RST(interrupt_vector[6]); return 16; }                                 // memory[SP] <- PC; PC = 0x0030
        case 0xF8: { LD(HL, ADD_SP()); return 12; }                                         // HL <- SP + e8
        case 0xF9: { LD(reg_SP, get_pair(HL)); return 8; }                                  // LD SP, HL
        case 0xFA: { LD(reg_A, fetch16()); return 16; }                                     // A <- memory[n16]
        case 0xFB: { EI(); return 4; }                                                      // EI
        case 0xFC: {}   // --- BAD ---
        case 0xFD: {}   // --- BAD ---
        case 0xFE: { CP(fetch()); return 8; }                                               // Update flags for (A - imm8)
        case 0xFF: { RST(interrupt_vector[7]); return 16; }                                 // memory[SP] <- PC; PC = 0x0038

        default: {throw std::runtime_error("How did you get here?"); }
    }
}

/**
 * CBG Rotates, Shifts, and Bit Operations
 * 
 * @return the number of t-states taken to execute the command
*/
int CPU::decode_execute_cb() {
    Byte opcode = fetch();

    switch (opcode) {
        case 0x00: { RL(reg_B, true); return 8; }
        case 0x01: { RL(reg_C, true); return 8; }
        case 0x02: { RL(reg_D, true); return 8; }
        case 0x03: { RL(reg_E, true); return 8; }
        case 0x04: { RL(reg_H, true); return 8; }
        case 0x05: { RL(reg_L, true); return 8; }
        case 0x06: { RL_HL(true); return 16; }
        case 0x07: { RL(reg_A, true); return 8; }
        case 0x08: { RR(reg_B, true); return 8; }
        case 0x09: { RR(reg_C, true); return 8; }
        case 0x0A: { RR(reg_D, true); return 8; }
        case 0x0B: { RR(reg_E, true); return 8; }
        case 0x0C: { RR(reg_H, true); return 8; }
        case 0x0D: { RR(reg_L, true); return 8; }
        case 0x0E: { RR_HL(true); return 16; }
        case 0x0F: { RR(reg_A, true); return 8; }

        case 0x10: { RL(reg_B, false); return 8; }
        case 0x11: { RL(reg_C, false); return 8; }
        case 0x12: { RL(reg_D, false); return 8; }
        case 0x13: { RL(reg_E, false); return 8; }
        case 0x14: { RL(reg_H, false); return 8; }
        case 0x15: { RL(reg_L, false); return 8; }
        case 0x16: { RL_HL(false); return 16; }
        case 0x17: { RL(reg_A, false); return 8; }
        case 0x18: { RR(reg_B, false); return 8; }
        case 0x19: { RR(reg_C, false); return 8; }
        case 0x1A: { RR(reg_D, false); return 8; }
        case 0x1B: { RR(reg_E, false); return 8; }
        case 0x1C: { RR(reg_H, false); return 8; }
        case 0x1D: { RR(reg_L, false); return 8; }
        case 0x1E: { RR_HL(false); return 16; }
        case 0x1F: { RR(reg_A, false); return 8; }

        case 0x20: { SLA(reg_B); return 8; }
        case 0x21: { SLA(reg_C); return 8; }
        case 0x22: { SLA(reg_D); return 8; }
        case 0x23: { SLA(reg_E); return 8; }
        case 0x24: { SLA(reg_H); return 8; }
        case 0x25: { SLA(reg_L); return 8; }
        case 0x26: { SLA_HL(); return 16; }
        case 0x27: { SLA(reg_A); return 8; }
        case 0x28: { SRA(reg_B); return 8; }
        case 0x29: { SRA(reg_C); return 8; }
        case 0x2A: { SRA(reg_D); return 8; }
        case 0x2B: { SRA(reg_E); return 8; }
        case 0x2C: { SRA(reg_H); return 8; }
        case 0x2D: { SRA(reg_L); return 8; }
        case 0x2E: { SRA_HL(); return 16; }
        case 0x2F: { SRA(reg_A); return 8; }

        case 0x30: { SWAP(reg_B); return 8; }
        case 0x31: { SWAP(reg_C); return 8; }
        case 0x32: { SWAP(reg_D); return 8; }
        case 0x33: { SWAP(reg_E); return 8; }
        case 0x34: { SWAP(reg_H); return 8; }
        case 0x35: { SWAP(reg_L); return 8; }
        case 0x36: { SWAP_HL(); return 16; }
        case 0x37: { SWAP(reg_A); return 8; }
        case 0x38: { SRL(reg_B); return 8; }
        case 0x39: { SRL(reg_C); return 8; }
        case 0x3A: { SRL(reg_D); return 8; }
        case 0x3B: { SRL(reg_E); return 8; }
        case 0x3C: { SRL(reg_H); return 8; }
        case 0x3D: { SRL(reg_L); return 8; }
        case 0x3E: { SRL_HL(); return 16; }
        case 0x3F: { SRL(reg_A); return 8; }

        case 0x40: { BIT(0, reg_B); return 8; }
        case 0x41: { BIT(0, reg_C); return 8; }
        case 0x42: { BIT(0, reg_D); return 8; }
        case 0x43: { BIT(0, reg_E); return 8; }
        case 0x44: { BIT(0, reg_H); return 8; }
        case 0x45: { BIT(0, reg_L); return 8; }
        case 0x46: { BIT(0, read_hl()); return 12; }
        case 0x47: { BIT(0, reg_A); return 8; }
        case 0x48: { BIT(1, reg_B); return 8; }
        case 0x49: { BIT(1, reg_C); return 8; }
        case 0x4A: { BIT(1, reg_D); return 8; }
        case 0x4B: { BIT(1, reg_E); return 8; }
        case 0x4C: { BIT(1, reg_H); return 8; }
        case 0x4D: { BIT(1, reg_L); return 8; }
        case 0x4E: { BIT(1, read_hl()); return 12; }
        case 0x4F: { BIT(1, reg_A); return 8; }

        case 0x50: { BIT(2, reg_B); return 8; }
        case 0x51: { BIT(2, reg_C); return 8; }
        case 0x52: { BIT(2, reg_D); return 8; }
        case 0x53: { BIT(2, reg_E); return 8; }
        case 0x54: { BIT(2, reg_H); return 8; }
        case 0x55: { BIT(2, reg_L); return 8; }
        case 0x56: { BIT(2, read_hl()); return 12; }
        case 0x57: { BIT(2, reg_A); return 8; }
        case 0x58: { BIT(3, reg_B); return 8; }
        case 0x59: { BIT(3, reg_C); return 8; }
        case 0x5A: { BIT(3, reg_D); return 8; }
        case 0x5B: { BIT(3, reg_E); return 8; }
        case 0x5C: { BIT(3, reg_H); return 8; }
        case 0x5D: { BIT(3, reg_L); return 8; }
        case 0x5E: { BIT(3, read_hl()); return 12; }
        case 0x5F: { BIT(3, reg_A); return 8; }

        case 0x60: { BIT(4, reg_B); return 8; }
        case 0x61: { BIT(4, reg_C); return 8; }
        case 0x62: { BIT(4, reg_D); return 8; }
        case 0x63: { BIT(4, reg_E); return 8; }
        case 0x64: { BIT(4, reg_H); return 8; }
        case 0x65: { BIT(4, reg_L); return 8; }
        case 0x66: { BIT(4, read_hl()); return 12; }
        case 0x67: { BIT(4, reg_A); return 8; }
        case 0x68: { BIT(5, reg_B); return 8; }
        case 0x69: { BIT(5, reg_C); return 8; }
        case 0x6A: { BIT(5, reg_D); return 8; }
        case 0x6B: { BIT(5, reg_E); return 8; }
        case 0x6C: { BIT(5, reg_H); return 8; }
        case 0x6D: { BIT(5, reg_L); return 8; }
        case 0x6E: { BIT(5, read_hl()); return 12; }
        case 0x6F: { BIT(5, reg_A); return 8; }

        case 0x70: { BIT(6, reg_B); return 8; }
        case 0x71: { BIT(6, reg_C); return 8; }
        case 0x72: { BIT(6, reg_D); return 8; }
        case 0x73: { BIT(6, reg_E); return 8; }
        case 0x74: { BIT(6, reg_H); return 8; }
        case 0x75: { BIT(6, reg_L); return 8; }
        case 0x76: { BIT(6, read_hl()); return 12; }
        case 0x77: { BIT(6, reg_A); return 8; }
        case 0x78: { BIT(7, reg_B); return 8; }
        case 0x79: { BIT(7, reg_C); return 8; }
        case 0x7A: { BIT(7, reg_D); return 8; }
        case 0x7B: { BIT(7, reg_E); return 8; }
        case 0x7C: { BIT(7, reg_H); return 8; }
        case 0x7D: { BIT(7, reg_L); return 8; }
        case 0x7E: { BIT(7, read_hl()); return 12; }
        case 0x7F: { BIT(7, reg_A); return 8; }

        case 0x80: { RES(0, reg_B); return 8; }
        case 0x81: { RES(0, reg_C); return 8; }
        case 0x82: { RES(0, reg_D); return 8; }
        case 0x83: { RES(0, reg_E); return 8; }
        case 0x84: { RES(0, reg_H); return 8; }
        case 0x85: { RES(0, reg_L); return 8; }
        case 0x86: { RES_HL(0); return 16; }
        case 0x87: { RES(0, reg_A); return 8; }
        case 0x88: { RES(1, reg_B); return 8; }
        case 0x89: { RES(1, reg_C); return 8; }
        case 0x8A: { RES(1, reg_D); return 8; }
        case 0x8B: { RES(1, reg_E); return 8; }
        case 0x8C: { RES(1, reg_H); return 8; }
        case 0x8D: { RES(1, reg_L); return 8; }
        case 0x8E: { RES_HL(1); return 16; }
        case 0x8F: { RES(1, reg_A); return 8; }

        case 0x90: { RES(2, reg_B); return 8; }
        case 0x91: { RES(2, reg_C); return 8; }
        case 0x92: { RES(2, reg_D); return 8; }
        case 0x93: { RES(2, reg_E); return 8; }
        case 0x94: { RES(2, reg_H); return 8; }
        case 0x95: { RES(2, reg_L); return 8; }
        case 0x96: { RES_HL(2); return 16; }
        case 0x97: { RES(2, reg_A); return 8; }
        case 0x98: { RES(3, reg_B); return 8; }
        case 0x99: { RES(3, reg_C); return 8; }
        case 0x9A: { RES(3, reg_D); return 8; }
        case 0x9B: { RES(3, reg_E); return 8; }
        case 0x9C: { RES(3, reg_H); return 8; }
        case 0x9D: { RES(3, reg_L); return 8; }
        case 0x9E: { RES_HL(3); return 16; }
        case 0x9F: { RES(3, reg_A); return 8; }

        case 0xA0: { RES(4, reg_B); return 8; }
        case 0xA1: { RES(4, reg_C); return 8; }
        case 0xA2: { RES(4, reg_D); return 8; }
        case 0xA3: { RES(4, reg_E); return 8; }
        case 0xA4: { RES(4, reg_H); return 8; }
        case 0xA5: { RES(4, reg_L); return 8; }
        case 0xA6: { RES_HL(4); return 16; }
        case 0xA7: { RES(4, reg_A); return 8; }
        case 0xA8: { RES(5, reg_B); return 8; }
        case 0xA9: { RES(5, reg_C); return 8; }
        case 0xAA: { RES(5, reg_D); return 8; }
        case 0xAB: { RES(5, reg_E); return 8; }
        case 0xAC: { RES(5, reg_H); return 8; }
        case 0xAD: { RES(5, reg_L); return 8; }
        case 0xAE: { RES_HL(5); return 16; }
        case 0xAF: { RES(5, reg_A); return 8; }

        case 0xB0: { RES(6, reg_B); return 8; }
        case 0xB1: { RES(6, reg_C); return 8; }
        case 0xB2: { RES(6, reg_D); return 8; }
        case 0xB3: { RES(6, reg_E); return 8; }
        case 0xB4: { RES(6, reg_H); return 8; }
        case 0xB5: { RES(6, reg_L); return 8; }
        case 0xB6: { RES_HL(6); return 16; }
        case 0xB7: { RES(6, reg_A); return 8; }
        case 0xB8: { RES(7, reg_B); return 8; }
        case 0xB9: { RES(7, reg_C); return 8; }
        case 0xBA: { RES(7, reg_D); return 8; }
        case 0xBB: { RES(7, reg_E); return 8; }
        case 0xBC: { RES(7, reg_H); return 8; }
        case 0xBD: { RES(7, reg_L); return 8; }
        case 0xBE: { RES_HL(7); return 16; }
        case 0xBF: { RES(7, reg_A); return 8; }

        case 0xC0: { SET(0, reg_B); return 8; }
        case 0xC1: { SET(0, reg_C); return 8; }
        case 0xC2: { SET(0, reg_D); return 8; }
        case 0xC3: { SET(0, reg_E); return 8; }
        case 0xC4: { SET(0, reg_H); return 8; }
        case 0xC5: { SET(0, reg_L); return 8; }
        case 0xC6: { SET_HL(0); return 16; }
        case 0xC7: { SET(0, reg_A); return 8; }
        case 0xC8: { SET(1, reg_B); return 8; }
        case 0xC9: { SET(1, reg_C); return 8; }
        case 0xCA: { SET(1, reg_D); return 8; }
        case 0xCB: { SET(1, reg_E); return 8; }
        case 0xCC: { SET(1, reg_H); return 8; }
        case 0xCD: { SET(1, reg_L); return 8; }
        case 0xCE: { SET_HL(1); return 16; }
        case 0xCF: { SET(1, reg_A); return 8; }

        case 0xD0: { SET(2, reg_B); return 8; }
        case 0xD1: { SET(2, reg_C); return 8; }
        case 0xD2: { SET(2, reg_D); return 8; }
        case 0xD3: { SET(2, reg_E); return 8; }
        case 0xD4: { SET(2, reg_H); return 8; }
        case 0xD5: { SET(2, reg_L); return 8; }
        case 0xD6: { SET_HL(2); return 16; }
        case 0xD7: { SET(2, reg_A); return 8; }
        case 0xD8: { SET(3, reg_B); return 8; }
        case 0xD9: { SET(3, reg_C); return 8; }
        case 0xDA: { SET(3, reg_D); return 8; }
        case 0xDB: { SET(3, reg_E); return 8; }
        case 0xDC: { SET(3, reg_H); return 8; }
        case 0xDD: { SET(3, reg_L); return 8; }
        case 0xDE: { SET_HL(3); return 16; }
        case 0xDF: { SET(3, reg_A); return 8; }

        case 0xE0: { SET(4, reg_B); return 8; }
        case 0xE1: { SET(4, reg_C); return 8; }
        case 0xE2: { SET(4, reg_D); return 8; }
        case 0xE3: { SET(4, reg_E); return 8; }
        case 0xE4: { SET(4, reg_H); return 8; }
        case 0xE5: { SET(4, reg_L); return 8; }
        case 0xE6: { SET_HL(4); return 16; }
        case 0xE7: { SET(4, reg_A); return 8; }
        case 0xE8: { SET(5, reg_B); return 8; }
        case 0xE9: { SET(5, reg_C); return 8; }
        case 0xEA: { SET(5, reg_D); return 8; }
        case 0xEB: { SET(5, reg_E); return 8; }
        case 0xEC: { SET(5, reg_H); return 8; }
        case 0xED: { SET(5, reg_L); return 8; }
        case 0xEE: { SET_HL(5); return 16; }
        case 0xEF: { SET(5, reg_A); return 8; }

        case 0xF0: { SET(6, reg_B); return 8; }
        case 0xF1: { SET(6, reg_C); return 8; }
        case 0xF2: { SET(6, reg_D); return 8; }
        case 0xF3: { SET(6, reg_E); return 8; }
        case 0xF4: { SET(6, reg_H); return 8; }
        case 0xF5: { SET(6, reg_L); return 8; }
        case 0xF6: { SET_HL(6); return 16; }
        case 0xF7: { SET(6, reg_A); return 8; }
        case 0xF8: { SET(7, reg_B); return 8; }
        case 0xF9: { SET(7, reg_C); return 8; }
        case 0xFA: { SET(7, reg_D); return 8; }
        case 0xFB: { SET(7, reg_E); return 8; }
        case 0xFC: { SET(7, reg_H); return 8; }
        case 0xFD: { SET(7, reg_L); return 8; }
        case 0xFE: { SET_HL(7); return 16; }
        case 0xFF: { SET(7, reg_A); return 8; }

        default: { throw std::runtime_error("How did you get here?"); }
    }
}