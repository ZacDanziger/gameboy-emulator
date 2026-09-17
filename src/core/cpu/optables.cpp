#include "cpu.h"
#include "../gameboy.h"

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
*/
bool CPU::decode_execute(const Byte opcode, GameBoy& gameboy) {
    switch(opcode) {
        case 0x00: { // NOP                                                   
            return true; 
        }
        case 0x01: { // B <- n16[15:8], C <- n16[7:0]                                                    
            switch(m_cycle) {
                case 0: return false;
                case 1: reg_C = gameboy.read(reg_PC++); return false;
                case 2: reg_B = gameboy.read(reg_PC++); return true;
            } 
        }
        case 0x02: { // memory[BC] <- A                                                    
            switch(m_cycle) {
                case 0: return false;
                case 1: gameboy.write(get_pair(BC), reg_A); return true;
            } 
        }
        case 0x03: { // INC BC++                                                              
            switch(m_cycle) {
                case 0: return false;
                case 1: INC(BC); return true;
            }
        }  
        case 0x04: { // B++                                                    
            INC(reg_B); return true; 
        }               
        case 0x05: { // B--                                                      
            DEC(reg_B); return true; 
        } 
        case 0x06: { // B <- n8                                                    
            switch(m_cycle) {
                case 0: return false;
                case 1: reg_B = gameboy.read(reg_PC++); return true;
            }
        }    
        case 0x07: { // RLCA                                                
            RLA(true); return true;
        }           
        case 0x08: { // memory[n16] <- SP[7:0], memory[n16+1] <- SP[15:8]                   
            switch(m_cycle) {
                case 0: return false;
                case 1: scratch_register |= gameboy.read(reg_PC++); return false;
                case 2: scratch_register |= (gameboy.read(reg_PC++) << 8); return false;
                case 3: reg_SP = 0x0000 | (scratch_register & 0x00FF); return false;
                case 4: reg_SP = 0x0000 | (scratch_register & 0xFF00) >> 8; return true;
            }
        }
        case 0x09: { // HL += BC
            switch(m_cycle) {
                case 0: return false;
                case 1: ADD_HL(get_pair(BC)); return true;
            }
        
        }    
        case 0x0A: { // A <- memory[BC]
            switch(m_cycle) {
                case 0: return false;
                case 1: reg_A = gameboy.read(get_pair(BC)); return true;
            }
        }   
        case 0x0B: { // BC--
            switch (m_cycle) {
                case 0: return false;
                case 1: DEC(BC); return true;
            }
        }               
        case 0x0C: { // C++
            INC(reg_C); return true; 
        }     
        case 0x0D: { // C--
            DEC(reg_C); return true;
        }      
        case 0x0E: { // C <- n8
            switch (m_cycle) {
                case 0: return false;
                case 1: reg_C = gameboy.read(reg_PC++); return true;
            }
        }
        case 0x0F: { // RRCA
            RRA(true); return true;
        }                           


        case 0x10: { // STOP
            // TODO: Look at STOP in the refactor
            return true;
        }                              
        case 0x11: { // D <- n16[15:8], E <- n16[7:0]
            switch (m_cycle) {
                case 0: return false;
                case 1: reg_E = gameboy.read(reg_PC++); return false;
                case 2: reg_D = gameboy.read(reg_PC++); return true;
            }
        }                   
        case 0x12: { // memory[DE] <- A
            switch (m_cycle) {
                case 0: return false;
                case 1: gameboy.write(get_pair(DE), reg_A); return true;
            }
        }          
        
        // STOPPED HERE SEP 16th

        case 0x13: { INC(DE); timer.tick(); return; }               // DE++
        case 0x14: { INC(reg_D); return; }                          // D++
        case 0x15: { DEC(reg_D); return; }                          // D--
        case 0x16: { LD(reg_D, fetch()); return; }                  // D <- n8
        case 0x17: { RLA(false); return; }                          // RLA
        case 0x18: { JR(); return; }                                // SP += e8
        case 0x19: { ADD_HL(get_pair(DE)); return; }                // HL += DE
        case 0x1A: { LD(reg_A, get_pair(DE)); return; }             // A <- memory[DE]
        case 0x1B: { DEC(DE); timer.tick(); return; }               // DE--
        case 0x1C: { INC(reg_E); return; }                          // E++
        case 0x1D: { DEC(reg_E); return; }                          // E--
        case 0x1E: { LD(reg_E, fetch()); return; }                  // E <- n8
        case 0x1F: { RRA(false); return; }                          // RRA

        case 0x20: { (JR_IF(FLAG_ZERO, false)); return; }           // JR NZ, e8
        case 0x21: { LD(HL, fetch16()); return; }                   // LD HL, n16
        case 0x22: { LD(get_pair(HL), reg_A); INC(HL); return; }    // memory[HL++] <- A
        case 0x23: { INC(HL); timer.tick(); return; }               // INC HL
        case 0x24: { INC(reg_H); return; }                          // H++
        case 0x25: { DEC(reg_H); return; }                          // H--
        case 0x26: { LD(reg_H, fetch()); return; }                  // H <- n8
        case 0x27: { DAA(); return; }                               // DAA
        case 0x28: { (JR_IF(FLAG_ZERO, true)); return; }            // JR Z, e8
        case 0x29: { ADD_HL(get_pair(HL)); return; }                // ADD HL, HL
        case 0x2A: { LD(reg_A, get_pair(HL)); INC(HL); return; }    // A <- memory[HL++]
        case 0x2B: { DEC(HL); timer.tick(); return; }               // HL--
        case 0x2C: { INC(reg_L); return; }                          // L++
        case 0x2D: { DEC(reg_L); return; }                          // L--
        case 0x2E: { LD(reg_L, fetch()); return; }                  // L <- n8
        case 0x2F: { CPL(); return; }                               // A <- ~A

        case 0x30: { (JR_IF(FLAG_CARRY, false)); return; }          // JR NC, e8
        case 0x31: { LD(reg_SP, fetch16()); return; }               // LD SP, n16
        case 0x32: { LD(get_pair(HL), reg_A); DEC(HL); return; }    // memory[HL--] <- A
        case 0x33: { INC(reg_SP); return; }                         // SP++
        case 0x34: { INC_HL(); return; }                            // memory[HL]++
        case 0x35: { DEC_HL(); return; }                            // memory[HL]--
        case 0x36: { LD(get_pair(HL), fetch()); return; }           // memory[HL] <- n8
        case 0x37: { SCF(); return; }                               // {Z N H C} = {- 0 0}
        case 0x38: { (JR_IF(FLAG_CARRY, true)); return; }           // JR C, e8
        case 0x39: { ADD_HL(reg_SP); return; }                      // HL += SP
        case 0x3A: { LD(reg_A, get_pair(HL)); DEC(HL); return; }    // A <- memory[HL--]
        case 0x3B: { DEC(reg_SP); return; }                         // SP--
        case 0x3C: { INC(reg_A); return; }                          // A++
        case 0x3D: { DEC(reg_A); return; }                          // A--
        case 0x3E: { LD(reg_A, fetch()); return; }                  // A <- n8
        case 0x3F: { CCF(); return; }                               // CCF

        case 0x40: { return; }                                      // LD B, B - essentially a NOP
        case 0x41: { LD(reg_B, reg_C); return; }                    // B <- C
        case 0x42: { LD(reg_B, reg_D); return; }                    // B <- D
        case 0x43: { LD(reg_B, reg_E); return; }                    // B <- E
        case 0x44: { LD(reg_B, reg_H); return; }                    // B <- H
        case 0x45: { LD(reg_B, reg_L); return; }                    // B <- L
        case 0x46: { LD(reg_B, get_pair(HL)); return; }             // B <- memory[HL]
        case 0x47: { LD(reg_B, reg_A); return; }                    // B <- A
        case 0x48: { LD(reg_C, reg_B); return; }                    // C <- B
        case 0x49: { return; }                                      // LD C, C - essentially a NOP
        case 0x4A: { LD(reg_C, reg_D); return; }                    // C <- D
        case 0x4B: { LD(reg_C, reg_E); return; }                    // C <- E
        case 0x4C: { LD(reg_C, reg_H); return; }                    // C <- H
        case 0x4D: { LD(reg_C, reg_L); return; }                    // C <- L
        case 0x4E: { LD(reg_C, get_pair(HL)); return; }             // C <- memory[HL]
        case 0x4F: { LD(reg_C, reg_A); return; }                    // C <- A

        case 0x50: { LD(reg_D, reg_B); return; }                    // D <- B
        case 0x51: { LD(reg_D, reg_C); return; }                    // D <- C
        case 0x52: { return; }                                      // LD D, D - essentially a NOP
        case 0x53: { LD(reg_D, reg_E); return; }                    // D <- E
        case 0x54: { LD(reg_D, reg_H); return; }                    // D <- H
        case 0x55: { LD(reg_D, reg_L); return; }                    // D <- L
        case 0x56: { LD(reg_D, get_pair(HL)); return; }             // D <- memory[HL]
        case 0x57: { LD(reg_D, reg_A); return; }                    // D <- A
        case 0x58: { LD(reg_E, reg_B); return; }                    // E <- B
        case 0x59: { LD(reg_E, reg_C); return; }                    // E <- C
        case 0x5A: { LD(reg_E, reg_D); return; }                    // E <- D
        case 0x5B: {return; }                                       // LD E, E - essentially a NOP
        case 0x5C: { LD(reg_E, reg_H); return; }                    // E <- H
        case 0x5D: { LD(reg_E, reg_L); return; }                    // E <- L
        case 0x5E: { LD(reg_E, get_pair(HL)); return; }             // E <- memory[HL]
        case 0x5F: { LD(reg_E, reg_A); return; }                    // E <- A

        case 0x60: { LD(reg_H, reg_B); return; }                    // H <- B
        case 0x61: { LD(reg_H, reg_C); return; }                    // H <- C
        case 0x62: { LD(reg_H, reg_D); return; }                    // H <- D
        case 0x63: { LD(reg_H, reg_E); return; }                    // H <- E
        case 0x64: { return; }                                      // LD H, H - essentially a NOP
        case 0x65: { LD(reg_H, reg_L); return; }                    // H <- L
        case 0x66: { LD(reg_H, get_pair(HL)); return; }             // H <- memory[HL]
        case 0x67: { LD(reg_H, reg_A); return; }                    // H <- A
        case 0x68: { LD(reg_L, reg_B); return; }                    // L <- B
        case 0x69: { LD(reg_L, reg_C); return; }                    // L <- C
        case 0x6A: { LD(reg_L, reg_D); return; }                    // L <- D
        case 0x6B: { LD(reg_L, reg_E); return; }                    // L <- E
        case 0x6C: { LD(reg_L, reg_H); return; }                    // L <- H
        case 0x6D: { LD(reg_L, reg_L); return; }                    // L <- L
        case 0x6E: { LD(reg_L, get_pair(HL)); return; }             // L <- memory[HL]
        case 0x6F: { LD(reg_L, reg_A); return; }                    // L <- A

        case 0x70: { LD(get_pair(HL), reg_B); return; }             // memory[HL] <- B
        case 0x71: { LD(get_pair(HL), reg_C); return; }             // memory[HL] <- C
        case 0x72: { LD(get_pair(HL), reg_D); return; }             // memory[HL] <- D
        case 0x73: { LD(get_pair(HL), reg_E); return; }             // memory[HL] <- E
        case 0x74: { LD(get_pair(HL), reg_H); return; }             // memory[HL] <- H
        case 0x75: { LD(get_pair(HL), reg_L); return; }             // memory[HL] <- L
        case 0x76: { HALT(); return; }                              // HALT
        case 0x77: { LD(get_pair(HL), reg_A); return; }             // memory[HL] <- A
        case 0x78: { LD(reg_A, reg_B); return; }                    // A <- B
        case 0x79: { LD(reg_A, reg_C); return; }                    // A <- C
        case 0x7A: { LD(reg_A, reg_D); return; }                    // A <- D
        case 0x7B: { LD(reg_A, reg_E); return; }                    // A <- E
        case 0x7C: { LD(reg_A, reg_H); return; }                    // A <- H
        case 0x7D: { LD(reg_A, reg_L); return; }                    // A <- L
        case 0x7E: { LD(reg_A, get_pair(HL)); return; }             // A <- memory[HL]
        case 0x7F: { return; }                                      // LD A, A - essentially a NOP

        case 0x80: { ADD(reg_B, false); return; }                   // A += B
        case 0x81: { ADD(reg_C, false); return; }                   // A += C
        case 0x82: { ADD(reg_D, false); return; }                   // A += D
        case 0x83: { ADD(reg_E, false); return; }                   // A += E
        case 0x84: { ADD(reg_H, false); return; }                   // A += H
        case 0x85: { ADD(reg_L, false); return; }                   // A += L
        case 0x86: { ADD(read_hl(), false); return; }               // A += memory[HL]
        case 0x87: { ADD(reg_A, false); return; }                   // A += A
        case 0x88: { ADD(reg_B, true); return; }                    // A += (B + carry)
        case 0x89: { ADD(reg_C, true); return; }                    // A += (C + carry)
        case 0x8A: { ADD(reg_D, true); return; }                    // A += (D + carry)
        case 0x8B: { ADD(reg_E, true); return; }                    // A += (E + carry)
        case 0x8C: { ADD(reg_H, true); return; }                    // A += (H + carry)
        case 0x8D: { ADD(reg_L, true); return; }                    // A += (L + carry)
        case 0x8E: { ADD(read_hl(), true); return; }                // A += (memory[HL] + carry)
        case 0x8F: { ADD(reg_A, true); return; }                    // A += (A + carry)

        case 0x90: { SUB(reg_B, false); return; }                   // A -= B
        case 0x91: { SUB(reg_C, false); return; }                   // A -= C
        case 0x92: { SUB(reg_D, false); return; }                   // A -= D
        case 0x93: { SUB(reg_E, false); return; }                   // A -= E
        case 0x94: { SUB(reg_H, false); return; }                   // A -= H
        case 0x95: { SUB(reg_L, false); return; }                   // A -= L
        case 0x96: { SUB(read_hl(), false); return; }               // A -= memory[HL]
        case 0x97: { SUB(reg_A, false); return; }                   // A -= A
        case 0x98: { SUB(reg_B, true); return; }                    // A -= (B + carry)
        case 0x99: { SUB(reg_C, true); return; }                    // A -= (C + carry)
        case 0x9A: { SUB(reg_D, true); return; }                    // A -= (D + carry)
        case 0x9B: { SUB(reg_E, true); return; }                    // A -= (E + carry)
        case 0x9C: { SUB(reg_H, true); return; }                    // A -= (H + carry)
        case 0x9D: { SUB(reg_L, true); return; }                    // A -= (L + carry)
        case 0x9E: { SUB(read_hl(), true); return; }                // A -= (memory[HL] + carry)
        case 0x9F: { SUB(reg_A, true); return; }                    // A -= (A + carry)

        case 0xA0: { AND(reg_B); return; }                          // A &= B
        case 0xA1: { AND(reg_C); return; }                          // A &= C
        case 0xA2: { AND(reg_D); return; }                          // A &= D
        case 0xA3: { AND(reg_E); return; }                          // A &= E
        case 0xA4: { AND(reg_H); return; }                          // A &= H
        case 0xA5: { AND(reg_L); return; }                          // A &= L
        case 0xA6: { AND(read_hl()); return; }                      // A &= memory[HL]
        case 0xA7: { AND(reg_A); return; }                          // A &= A
        case 0xA8: { XOR(reg_B); return; }                          // A ^= B
        case 0xA9: { XOR(reg_C); return; }                          // A ^= C
        case 0xAA: { XOR(reg_D); return; }                          // A ^= D
        case 0xAB: { XOR(reg_E); return; }                          // A ^= E
        case 0xAC: { XOR(reg_H); return; }                          // A ^= H
        case 0xAD: { XOR(reg_L); return; }                          // A ^= L
        case 0xAE: { XOR(read_hl()); return; }                      // A ^= memory[HL]
        case 0xAF: { XOR(reg_A); return; }                          // A ^= A

        case 0xB0: { OR(reg_B); return; }                           // A |= B
        case 0xB1: { OR(reg_C); return; }                           // A |= C
        case 0xB2: { OR(reg_D); return; }                           // A |= D
        case 0xB3: { OR(reg_E); return; }                           // A |= E
        case 0xB4: { OR(reg_H); return; }                           // A |= H
        case 0xB5: { OR(reg_L); return; }                           // A |= L
        case 0xB6: { OR(read_hl()); return; }                       // A |= memory[HL]
        case 0xB7: { OR(reg_A); return; }                           // A |= A
        case 0xB8: { CP(reg_B); return; }                           // Update flags for (A - B)
        case 0xB9: { CP(reg_C); return; }                           // Update flags for (A - C)
        case 0xBA: { CP(reg_D); return; }                           // Update flags for (A - D)
        case 0xBB: { CP(reg_E); return; }                           // Update flags for (A - E)
        case 0xBC: { CP(reg_H); return; }                           // Update flags for (A - H)
        case 0xBD: { CP(reg_L); return; }                           // Update flags for (A - L)
        case 0xBE: { CP(read_hl()); return; }                       // Update flags for (A - memory[HL])
        case 0xBF: { CP(reg_A); return; }                           // Update flags for (A - A)

        case 0xC0: { (RET_IF(FLAG_ZERO, false)); return; }          // RET NZ
        case 0xC1: { POP(BC); return; }                             // C <- memory[SP++]; B <- memory[SP++]
        case 0xC2: { (JP_IF(FLAG_ZERO, false)); return; }           // JP NZ, a16
        case 0xC3: { JP(fetch16()); timer.tick(); return; }         // JP a16
        case 0xC4: { (CALL_IF(FLAG_ZERO, false)); return; }         // CALL NZ, a16
        case 0xC5: { PUSH(BC); return; }                            // memory[--SP] <- B; memory[--SP] <- C
        case 0xC6: { ADD(fetch(), false); return; }                 // A += n8
        case 0xC7: { RST(reset_vector[0]); return; }                // memory[SP] <- PC; PC = 0x0000
        case 0xC8: { (RET_IF(FLAG_ZERO, true)); return; }           // RET Z
        case 0xC9: { RET(); return; }                               // RET
        case 0xCA: { (JP_IF(FLAG_ZERO, true)); return; }            // JP Z, a16
        case 0xCB: { decode_execute_cb(); return; }                 // PREFIX
        case 0xCC: { (CALL_IF(FLAG_ZERO, true)); return; }          // CALL Z, a16
        case 0xCD: { CALL(); return; }                              // CALL a16
        case 0xCE: { ADD(fetch(), true); return; }                  // A += (n8 + carry)
        case 0xCF: { RST(reset_vector[1]); return; }                // memory[SP] <- PC; PC = 0x0008

        case 0xD0: { (RET_IF(FLAG_CARRY, false)); return; }         // RET NC
        case 0xD1: { POP(DE); return; }                             // E <- memory[++SP]; D <- memory[++SP]
        case 0xD2: { (JP_IF(FLAG_CARRY, false)); return; }          // JP NC, a16
        case 0xD3: { throw std::runtime_error("Opcode: D3 is bad"); } // --- BAD ---
        case 0xD4: { (CALL_IF(FLAG_CARRY, false)); return; }        // CALL NC, a16
        case 0xD5: { PUSH(DE); return; }                            // memory[--SP] <- D; memory[--SP] <- E
        case 0xD6: { SUB(fetch(), false); return; }                 // A -= n8
        case 0xD7: { RST(reset_vector[2]); return; }                // memory[SP] <- PC; PC = 0x0010
        case 0xD8: { (RET_IF(FLAG_CARRY, true)); return; }          // RET C
        case 0xD9: { RET(); interrupts_enabled = true; return; }    // RETI
        case 0xDA: { (JP_IF(FLAG_CARRY, true)); return; }           // JP C, a16
        case 0xDB: { throw std::runtime_error("Opcode: DB is bad"); } // --- BAD ---
        case 0xDC: { (CALL_IF(FLAG_CARRY, true)); return; }         // CALL C, a16
        case 0xDD: { throw std::runtime_error("Opcode: DD is bad"); } // --- BAD ---
        case 0xDE: { SUB(fetch(), true); return; }                  // A -= (n8 + carry)
        case 0xDF: { RST(reset_vector[3]); return; }                // memory[SP] <- PC; PC = 0x0018

        case 0xE0: { LDH(fetch(), false); return; }                 // memory[$FF00 + n8] <- A
        case 0xE1: { POP(HL); return; }                             // L <- memory[++SP]; H <- memory[++SP]
        case 0xE2: { LDH(reg_C, false); return; }                   // memory[$FF00 + C] <- A
        case 0xE3: { throw std::runtime_error("Opcode: E3 is bad"); } // --- BAD ---
        case 0xE4: { throw std::runtime_error("Opcode: E4 is bad"); } // --- BAD ---
        case 0xE5: { PUSH(HL); return; }                            // memory[--SP] <- H; memory[--SP] <- L
        case 0xE6: { AND(fetch()); return; }                        // A &= n8
        case 0xE7: { RST(reset_vector[4]); return; }                // memory[SP] <- PC; PC = 0x0020
        case 0xE8: { reg_SP = ADD_SP(); timer.tick(); return; }    // SP += e8
        case 0xE9: { JP(get_pair(HL)); return; }                    // PC <- HL
        case 0xEA: { LD(fetch16(), reg_A); return; }                // memory[n16] <- A
        case 0xEB: { throw std::runtime_error("Opcode: EB is bad"); } // --- BAD ---
        case 0xEC: { throw std::runtime_error("Opcode: EC is bad"); } // --- BAD ---
        case 0xED: { throw std::runtime_error("Opcode: ED is bad"); } // --- BAD ---
        case 0xEE: { XOR(fetch()); return; }                        // A ^= n8
        case 0xEF: { RST(reset_vector[5]); return; }                // memory[SP] <- PC; PC = 0x0028

        case 0xF0: { LDH(fetch(), true); return; }                  // A <- memory[$FF00 + n8]
        case 0xF1: { POP(AF); reg_F &= 0xF0; return; }              // F <- memory[++SP]; A <- memory[++SP]
        case 0xF2: { LDH(reg_C, true); return; }                    // A <- memory[$FF00 + C]
        case 0xF3: { DI(); return; }                                // DI
        case 0xF4: { throw std::runtime_error("Opcode: F4 is bad"); } // --- BAD ---
        case 0xF5: { PUSH(AF); return; }                            // memory[--SP] <- A; memory[--SP] <- F
        case 0xF6: { OR(fetch()); return; }                         // A |= n8
        case 0xF7: { RST(reset_vector[6]); return; }                // memory[SP] <- PC; PC = 0x0030
        case 0xF8: { LD(HL, ADD_SP()); return; }                    // HL <- SP + e8
        case 0xF9: { LD(reg_SP, get_pair(HL)); timer.tick(); return; } // LD SP, HL
        case 0xFA: { LD(reg_A, fetch16()); return; }                // A <- memory[n16]
        case 0xFB: { EI(); return; }                                // EI
        case 0xFC: { throw std::runtime_error("Opcode: FC is bad"); } // --- BAD ---
        case 0xFD: { throw std::runtime_error("Opcode: FD is bad"); } // --- BAD ---
        case 0xFE: { CP(fetch()); return; }                         // Update flags for (A - imm8)
        case 0xFF: { RST(reset_vector[7]); return; }                // memory[SP] <- PC; PC = 0x0038

        default: {throw std::runtime_error("How did you get here?"); }
    } 
}

/**
 * CB Prefix Rotates, Shifts, and Bit Operations
*/
void CPU::decode_execute_cb() {
    Byte opcode = fetch();

    switch (opcode) {
        case 0x00: { RL(reg_B, true); return; }
        case 0x01: { RL(reg_C, true); return; }
        case 0x02: { RL(reg_D, true); return; }
        case 0x03: { RL(reg_E, true); return; }
        case 0x04: { RL(reg_H, true); return; }
        case 0x05: { RL(reg_L, true); return; }
        case 0x06: { RL_HL(true); return; }
        case 0x07: { RL(reg_A, true); return; }
        case 0x08: { RR(reg_B, true); return; }
        case 0x09: { RR(reg_C, true); return; }
        case 0x0A: { RR(reg_D, true); return; }
        case 0x0B: { RR(reg_E, true); return; }
        case 0x0C: { RR(reg_H, true); return; }
        case 0x0D: { RR(reg_L, true); return; }
        case 0x0E: { RR_HL(true); return; }
        case 0x0F: { RR(reg_A, true); return; }

        case 0x10: { RL(reg_B, false); return; }
        case 0x11: { RL(reg_C, false); return; }
        case 0x12: { RL(reg_D, false); return; }
        case 0x13: { RL(reg_E, false); return; }
        case 0x14: { RL(reg_H, false); return; }
        case 0x15: { RL(reg_L, false); return; }
        case 0x16: { RL_HL(false); return; }
        case 0x17: { RL(reg_A, false); return; }
        case 0x18: { RR(reg_B, false); return; }
        case 0x19: { RR(reg_C, false); return; }
        case 0x1A: { RR(reg_D, false); return; }
        case 0x1B: { RR(reg_E, false); return; }
        case 0x1C: { RR(reg_H, false); return; }
        case 0x1D: { RR(reg_L, false); return; }
        case 0x1E: { RR_HL(false); return; }
        case 0x1F: { RR(reg_A, false); return; }

        case 0x20: { SLA(reg_B); return; }
        case 0x21: { SLA(reg_C); return; }
        case 0x22: { SLA(reg_D); return; }
        case 0x23: { SLA(reg_E); return; }
        case 0x24: { SLA(reg_H); return; }
        case 0x25: { SLA(reg_L); return; }
        case 0x26: { SLA_HL(); return; }
        case 0x27: { SLA(reg_A); return; }
        case 0x28: { SRA(reg_B); return; }
        case 0x29: { SRA(reg_C); return; }
        case 0x2A: { SRA(reg_D); return; }
        case 0x2B: { SRA(reg_E); return; }
        case 0x2C: { SRA(reg_H); return; }
        case 0x2D: { SRA(reg_L); return; }
        case 0x2E: { SRA_HL(); return; }
        case 0x2F: { SRA(reg_A); return; }

        case 0x30: { SWAP(reg_B); return; }
        case 0x31: { SWAP(reg_C); return; }
        case 0x32: { SWAP(reg_D); return; }
        case 0x33: { SWAP(reg_E); return; }
        case 0x34: { SWAP(reg_H); return; }
        case 0x35: { SWAP(reg_L); return; }
        case 0x36: { SWAP_HL(); return; }
        case 0x37: { SWAP(reg_A); return; }
        case 0x38: { SRL(reg_B); return; }
        case 0x39: { SRL(reg_C); return; }
        case 0x3A: { SRL(reg_D); return; }
        case 0x3B: { SRL(reg_E); return; }
        case 0x3C: { SRL(reg_H); return; }
        case 0x3D: { SRL(reg_L); return; }
        case 0x3E: { SRL_HL(); return; }
        case 0x3F: { SRL(reg_A); return; }

        case 0x40: { BIT(reg_B, Bit::Bit0); return; }
        case 0x41: { BIT(reg_C, Bit::Bit0); return; }
        case 0x42: { BIT(reg_D, Bit::Bit0); return; }
        case 0x43: { BIT(reg_E, Bit::Bit0); return; }
        case 0x44: { BIT(reg_H, Bit::Bit0); return; }
        case 0x45: { BIT(reg_L, Bit::Bit0); return; }
        case 0x46: { BIT(read_hl(), Bit::Bit0); return; }
        case 0x47: { BIT(reg_A, Bit::Bit0); return; }
        case 0x48: { BIT(reg_B, Bit::Bit1); return; }
        case 0x49: { BIT(reg_C, Bit::Bit1); return; }
        case 0x4A: { BIT(reg_D, Bit::Bit1); return; }
        case 0x4B: { BIT(reg_E, Bit::Bit1); return; }
        case 0x4C: { BIT(reg_H, Bit::Bit1); return; }
        case 0x4D: { BIT(reg_L, Bit::Bit1); return; }
        case 0x4E: { BIT(read_hl(), Bit::Bit1); return; }
        case 0x4F: { BIT(reg_A, Bit::Bit1); return; }

        case 0x50: { BIT(reg_B, Bit::Bit2); return; }
        case 0x51: { BIT(reg_C, Bit::Bit2); return; }
        case 0x52: { BIT(reg_D, Bit::Bit2); return; }
        case 0x53: { BIT(reg_E, Bit::Bit2); return; }
        case 0x54: { BIT(reg_H, Bit::Bit2); return; }
        case 0x55: { BIT(reg_L, Bit::Bit2); return; }
        case 0x56: { BIT(read_hl(), Bit::Bit2); return; }
        case 0x57: { BIT(reg_A, Bit::Bit2); return; }
        case 0x58: { BIT(reg_B, Bit::Bit3); return; }
        case 0x59: { BIT(reg_C, Bit::Bit3); return; }
        case 0x5A: { BIT(reg_D, Bit::Bit3); return; }
        case 0x5B: { BIT(reg_E, Bit::Bit3); return; }
        case 0x5C: { BIT(reg_H, Bit::Bit3); return; }
        case 0x5D: { BIT(reg_L, Bit::Bit3); return; }
        case 0x5E: { BIT(read_hl(), Bit::Bit3); return; }
        case 0x5F: { BIT(reg_A, Bit::Bit3); return; }

        case 0x60: { BIT(reg_B, Bit::Bit4); return; }
        case 0x61: { BIT(reg_C, Bit::Bit4); return; }
        case 0x62: { BIT(reg_D, Bit::Bit4); return; }
        case 0x63: { BIT(reg_E, Bit::Bit4); return; }
        case 0x64: { BIT(reg_H, Bit::Bit4); return; }
        case 0x65: { BIT(reg_L, Bit::Bit4); return; }
        case 0x66: { BIT(read_hl(), Bit::Bit4); return; }
        case 0x67: { BIT(reg_A, Bit::Bit4); return; }
        case 0x68: { BIT(reg_B, Bit::Bit5); return; }
        case 0x69: { BIT(reg_C, Bit::Bit5); return; }
        case 0x6A: { BIT(reg_D, Bit::Bit5); return; }
        case 0x6B: { BIT(reg_E, Bit::Bit5); return; }
        case 0x6C: { BIT(reg_H, Bit::Bit5); return; }
        case 0x6D: { BIT(reg_L, Bit::Bit5); return; }
        case 0x6E: { BIT(read_hl(), Bit::Bit5); return; }
        case 0x6F: { BIT(reg_A, Bit::Bit5); return; }

        case 0x70: { BIT(reg_B, Bit::Bit6); return; }
        case 0x71: { BIT(reg_C, Bit::Bit6); return; }
        case 0x72: { BIT(reg_D, Bit::Bit6); return; }
        case 0x73: { BIT(reg_E, Bit::Bit6); return; }
        case 0x74: { BIT(reg_H, Bit::Bit6); return; }
        case 0x75: { BIT(reg_L, Bit::Bit6); return; }
        case 0x76: { BIT(read_hl(), Bit::Bit6); return; }
        case 0x77: { BIT(reg_A, Bit::Bit6); return; }
        case 0x78: { BIT(reg_B, Bit::Bit7); return; }
        case 0x79: { BIT(reg_C, Bit::Bit7); return; }
        case 0x7A: { BIT(reg_D, Bit::Bit7); return; }
        case 0x7B: { BIT(reg_E, Bit::Bit7); return; }
        case 0x7C: { BIT(reg_H, Bit::Bit7); return; }
        case 0x7D: { BIT(reg_L, Bit::Bit7); return; }
        case 0x7E: { BIT(read_hl(), Bit::Bit7); return; }
        case 0x7F: { BIT(reg_A, Bit::Bit7); return; }

        case 0x80: { RES(reg_B, Bit::Bit0); return; }
        case 0x81: { RES(reg_C, Bit::Bit0); return; }
        case 0x82: { RES(reg_D, Bit::Bit0); return; }
        case 0x83: { RES(reg_E, Bit::Bit0); return; }
        case 0x84: { RES(reg_H, Bit::Bit0); return; }
        case 0x85: { RES(reg_L, Bit::Bit0); return; }
        case 0x86: { RES_HL(Bit::Bit0); return; }
        case 0x87: { RES(reg_A, Bit::Bit0); return; }
        case 0x88: { RES(reg_B, Bit::Bit1); return; }
        case 0x89: { RES(reg_C, Bit::Bit1); return; }
        case 0x8A: { RES(reg_D, Bit::Bit1); return; }
        case 0x8B: { RES(reg_E, Bit::Bit1); return; }
        case 0x8C: { RES(reg_H, Bit::Bit1); return; }
        case 0x8D: { RES(reg_L, Bit::Bit1); return; }
        case 0x8E: { RES_HL(Bit::Bit1); return; }
        case 0x8F: { RES(reg_A, Bit::Bit1); return; }

        case 0x90: { RES(reg_B, Bit::Bit2); return; }
        case 0x91: { RES(reg_C, Bit::Bit2); return; }
        case 0x92: { RES(reg_D, Bit::Bit2); return; }
        case 0x93: { RES(reg_E, Bit::Bit2); return; }
        case 0x94: { RES(reg_H, Bit::Bit2); return; }
        case 0x95: { RES(reg_L, Bit::Bit2); return; }
        case 0x96: { RES_HL(Bit::Bit2); return; }
        case 0x97: { RES(reg_A, Bit::Bit2); return; }
        case 0x98: { RES(reg_B, Bit::Bit3); return; }
        case 0x99: { RES(reg_C, Bit::Bit3); return; }
        case 0x9A: { RES(reg_D, Bit::Bit3); return; }
        case 0x9B: { RES(reg_E, Bit::Bit3); return; }
        case 0x9C: { RES(reg_H, Bit::Bit3); return; }
        case 0x9D: { RES(reg_L, Bit::Bit3); return; }
        case 0x9E: { RES_HL(Bit::Bit3); return; }
        case 0x9F: { RES(reg_A, Bit::Bit3); return; }

        case 0xA0: { RES(reg_B, Bit::Bit4); return; }
        case 0xA1: { RES(reg_C, Bit::Bit4); return; }
        case 0xA2: { RES(reg_D, Bit::Bit4); return; }
        case 0xA3: { RES(reg_E, Bit::Bit4); return; }
        case 0xA4: { RES(reg_H, Bit::Bit4); return; }
        case 0xA5: { RES(reg_L, Bit::Bit4); return; }
        case 0xA6: { RES_HL(Bit::Bit4); return; }
        case 0xA7: { RES(reg_A, Bit::Bit4); return; }
        case 0xA8: { RES(reg_B, Bit::Bit5); return; }
        case 0xA9: { RES(reg_C, Bit::Bit5); return; }
        case 0xAA: { RES(reg_D, Bit::Bit5); return; }
        case 0xAB: { RES(reg_E, Bit::Bit5); return; }
        case 0xAC: { RES(reg_H, Bit::Bit5); return; }
        case 0xAD: { RES(reg_L, Bit::Bit5); return; }
        case 0xAE: { RES_HL(Bit::Bit5); return; }
        case 0xAF: { RES(reg_A, Bit::Bit5); return; }

        case 0xB0: { RES(reg_B, Bit::Bit6); return; }
        case 0xB1: { RES(reg_C, Bit::Bit6); return; }
        case 0xB2: { RES(reg_D, Bit::Bit6); return; }
        case 0xB3: { RES(reg_E, Bit::Bit6); return; }
        case 0xB4: { RES(reg_H, Bit::Bit6); return; }
        case 0xB5: { RES(reg_L, Bit::Bit6); return; }
        case 0xB6: { RES_HL(Bit::Bit6); return; }
        case 0xB7: { RES(reg_A, Bit::Bit6); return; }
        case 0xB8: { RES(reg_B, Bit::Bit7); return; }
        case 0xB9: { RES(reg_C, Bit::Bit7); return; }
        case 0xBA: { RES(reg_D, Bit::Bit7); return; }
        case 0xBB: { RES(reg_E, Bit::Bit7); return; }
        case 0xBC: { RES(reg_H, Bit::Bit7); return; }
        case 0xBD: { RES(reg_L, Bit::Bit7); return; }
        case 0xBE: { RES_HL(Bit::Bit7); return; }
        case 0xBF: { RES(reg_A, Bit::Bit7); return; }

        case 0xC0: { SET(reg_B, Bit::Bit0); return; }
        case 0xC1: { SET(reg_C, Bit::Bit0); return; }
        case 0xC2: { SET(reg_D, Bit::Bit0); return; }
        case 0xC3: { SET(reg_E, Bit::Bit0); return; }
        case 0xC4: { SET(reg_H, Bit::Bit0); return; }
        case 0xC5: { SET(reg_L, Bit::Bit0); return; }
        case 0xC6: { SET_HL(Bit::Bit0); return; }
        case 0xC7: { SET(reg_A, Bit::Bit0); return; }
        case 0xC8: { SET(reg_B, Bit::Bit1); return; }
        case 0xC9: { SET(reg_C, Bit::Bit1); return; }
        case 0xCA: { SET(reg_D, Bit::Bit1); return; }
        case 0xCB: { SET(reg_E, Bit::Bit1); return; }
        case 0xCC: { SET(reg_H, Bit::Bit1); return; }
        case 0xCD: { SET(reg_L, Bit::Bit1); return; }
        case 0xCE: { SET_HL(Bit::Bit1); return; }
        case 0xCF: { SET(reg_A, Bit::Bit1); return; }

        case 0xD0: { SET(reg_B, Bit::Bit2); return; }
        case 0xD1: { SET(reg_C, Bit::Bit2); return; }
        case 0xD2: { SET(reg_D, Bit::Bit2); return; }
        case 0xD3: { SET(reg_E, Bit::Bit2); return; }
        case 0xD4: { SET(reg_H, Bit::Bit2); return; }
        case 0xD5: { SET(reg_L, Bit::Bit2); return; }
        case 0xD6: { SET_HL(Bit::Bit2); return; }
        case 0xD7: { SET(reg_A, Bit::Bit2); return; }
        case 0xD8: { SET(reg_B, Bit::Bit3); return; }
        case 0xD9: { SET(reg_C, Bit::Bit3); return; }
        case 0xDA: { SET(reg_D, Bit::Bit3); return; }
        case 0xDB: { SET(reg_E, Bit::Bit3); return; }
        case 0xDC: { SET(reg_H, Bit::Bit3); return; }
        case 0xDD: { SET(reg_L, Bit::Bit3); return; }
        case 0xDE: { SET_HL(Bit::Bit3); return; }
        case 0xDF: { SET(reg_A, Bit::Bit3); return; }

        case 0xE0: { SET(reg_B, Bit::Bit4); return; }
        case 0xE1: { SET(reg_C, Bit::Bit4); return; }
        case 0xE2: { SET(reg_D, Bit::Bit4); return; }
        case 0xE3: { SET(reg_E, Bit::Bit4); return; }
        case 0xE4: { SET(reg_H, Bit::Bit4); return; }
        case 0xE5: { SET(reg_L, Bit::Bit4); return; }
        case 0xE6: { SET_HL(Bit::Bit4); return; }
        case 0xE7: { SET(reg_A, Bit::Bit4); return; }
        case 0xE8: { SET(reg_B, Bit::Bit5); return; }
        case 0xE9: { SET(reg_C, Bit::Bit5); return; }
        case 0xEA: { SET(reg_D, Bit::Bit5); return; }
        case 0xEB: { SET(reg_E, Bit::Bit5); return; }
        case 0xEC: { SET(reg_H, Bit::Bit5); return; }
        case 0xED: { SET(reg_L, Bit::Bit5); return; }
        case 0xEE: { SET_HL(Bit::Bit5); return; }
        case 0xEF: { SET(reg_A, Bit::Bit5); return; }

        case 0xF0: { SET(reg_B, Bit::Bit6); return; }
        case 0xF1: { SET(reg_C, Bit::Bit6); return; }
        case 0xF2: { SET(reg_D, Bit::Bit6); return; }
        case 0xF3: { SET(reg_E, Bit::Bit6); return; }
        case 0xF4: { SET(reg_H, Bit::Bit6); return; }
        case 0xF5: { SET(reg_L, Bit::Bit6); return; }
        case 0xF6: { SET_HL(Bit::Bit6); return; }
        case 0xF7: { SET(reg_A, Bit::Bit6); return; }
        case 0xF8: { SET(reg_B, Bit::Bit7); return; }
        case 0xF9: { SET(reg_C, Bit::Bit7); return; }
        case 0xFA: { SET(reg_D, Bit::Bit7); return; }
        case 0xFB: { SET(reg_E, Bit::Bit7); return; }
        case 0xFC: { SET(reg_H, Bit::Bit7); return; }
        case 0xFD: { SET(reg_L, Bit::Bit7); return; }
        case 0xFE: { SET_HL(Bit::Bit7); return; }
        case 0xFF: { SET(reg_A, Bit::Bit7); return; }

        default: { throw std::runtime_error("How did you get here?"); }
    }
}