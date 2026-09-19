#include "cpu.h"
#include "../gameboy.h"


void CPU::decode() {
    switch (current_opcode) {
        case 0x00: return;                                          // NOP
        case 0x01: queue_load_r16_imm16(&registers.BC); return;     // BC <- Mem[Imm16]
        case 0x02: queue_store_a_mem_r16(&registers.BC); return;    // Mem[BC] <- A
        case 0x03: queue_inc_r16(&registers.BC); return;            // BC++
        case 0x04: queue_inc_r8(&registers.B); return;              // B++
        case 0x05: queue_dec_r8(&registers.B); return;              // B--
        case 0x06: queue_load_r8_imm8(&registers.B); return;        // B <- Mem[PC++]
        case 0x07: /* RLCA */ return;                               // RLCA
        case 0x08: queue_store_sp_imm16(); return;                  // Mem[Imm16] <- SP[7:0]; Mem[Imm16+1] <- SP[15:8]
        case 0x09: queue_add_hl_r16(&registers.BC); return;         // HL += BC
        case 0x0A: queue_load_a_mem_r16(&registers.BC); return;     // A <- Mem[BC]
        case 0x0B: queue_dec_r16(&registers.BC); return;            // BC--
        case 0x0C: queue_inc_r8(&registers.C); return;              // C++
        case 0x0D: queue_dec_r8(&registers.C); return;              // C--
        case 0x0E: queue_load_r8_imm8(&registers.C); return;        // C <- Mem[PC++]
        case 0x0F: /* RRCA */ return;                               // RRCA

        case 0x10: /* STOP */ return;                               // STOP
        case 0x11: queue_load_r16_imm16(&registers.DE); return;     // DE <- Mem[Imm16]
        case 0x12: queue_store_a_mem_r16(&registers.DE); return;    // Mem[DE] <- A
        case 0x13: queue_inc_r16(&registers.DE); return;            // DE++
        case 0x14: queue_inc_r8(&registers.D); return;              // D++
        case 0x15: queue_dec_r8(&registers.D); return;              // D--
        case 0x16: queue_load_r8_imm8(&registers.D); return;        // D <- Mem[PC++]
        case 0x17: /* RLA */ return;                                // RLA
        case 0x18: queue_jr(); return;                              // PC += (int16_t)(Mem[PC++])
        case 0x19: queue_add_hl_r16(&registers.DE); return;         // HL += DE
        case 0x1A: queue_load_a_mem_r16(&registers.DE); return;     // A <- Mem[DE]
        case 0x1B: queue_dec_r16(&registers.DE); return;            // DE--
        case 0x1C: queue_inc_r8(&registers.E); return;              // E++
        case 0x1D: queue_dec_r8(&registers.E); return;              // E--
        case 0x1E: queue_load_r8_imm8(&registers.E); return;        // E <- Mem[PC++]
        case 0x1F: /* RRA */ return;                                // RRA

        case 0x20: queue_jr_cond(); return;                         // If NZ, PC += (int16_t)(Mem[PC++])
        case 0x21: queue_load_r16_imm16(&registers.HL); return;     // HL <- Mem[Imm16]
        case 0x22: queue_store_a_mem_r16(&registers.HL); return;    // Mem[HL++] <- A
        case 0x23: queue_inc_r16(&registers.HL); return;            // HL++
        case 0x24: queue_inc_r8(&registers.H); return;              // H++
        case 0x25: queue_dec_r8(&registers.H); return;              // H--
        case 0x26: queue_load_r8_imm8(&registers.H); return;        // H <- Mem[PC++]
        case 0x27: /* DAA */ return;                                // DAA
        case 0x28: queue_jr_cond(); return;                         // If Z, PC += (int16_t)(Mem[PC++])
        case 0x29: queue_add_hl_r16(&registers.HL); return;         // HL += HL
        case 0x2A: queue_load_a_mem_r16(&registers.HL); return;     // A <- Mem[HL++]
        case 0x2B: queue_dec_r16(&registers.HL); return;            // HL--
        case 0x2C: queue_inc_r8(&registers.L); return;              // L++
        case 0x2D: queue_dec_r8(&registers.L); return;              // L--
        case 0x2E: queue_load_r8_imm8(&registers.L); return;        // L <- Mem[PC++]
        case 0x2F: /* CPL */ return;                                // CPL

        case 0x30: queue_jr_cond(); return;                         // If NC, PC += (int16_t)(Mem[PC++])
        case 0x31: queue_load_r16_imm16(&registers.SP); return;     // SP <- Mem[Imm16]
        case 0x32: queue_store_a_mem_r16(&registers.HL); return;    // Mem[HL--] <- A
        case 0x33: queue_inc_r16(&registers.SP); return;            // SP++
        case 0x34: queue_inc_dec_mem_hl(); return;                  // Mem[HL]++
        case 0x35: queue_inc_dec_mem_hl(); return;                  // Mem[HL]--
        case 0x36: queue_load_mem_hl_imm8(); return;                // Mem[HL] <- Mem[PC++]
        case 0x37: /* SCF */ return;                                // SCF
        case 0x38: queue_jr_cond(); return;                         // If C, PC += (int16_t)(Mem[PC++])
        case 0x39: queue_add_hl_r16(&registers.SP); return;         // HL += SP
        case 0x3A: queue_load_a_mem_r16(&registers.SP); return;     // A <- Mem[HL--]
        case 0x3B: queue_dec_r16(&registers.DE); return;            // SP--
        case 0x3C: queue_inc_r8(&registers.A); return;              // A++
        case 0x3D: queue_dec_r8(&registers.A); return;              // A--
        case 0x3E: queue_load_r8_imm8(&registers.A); return;        // A <- Mem[PC++]
        case 0x3F: /* CCF */ return;                                 // CCF

        case 0x40: return;                                                  // B <- B (essentially a NOP)
        case 0x41: queue_load_r8_r8(&registers.B, &registers.C); return;    // B <- C
        case 0x42: queue_load_r8_r8(&registers.B, &registers.D); return;    // B <- D
        case 0x43: queue_load_r8_r8(&registers.B, &registers.E); return;    // B <- E
        case 0x44: queue_load_r8_r8(&registers.B, &registers.H); return;    // B <- H
        case 0x45: queue_load_r8_r8(&registers.B, &registers.L); return;    // B <- L
        case 0x46: queue_load_r8_r16(&registers.B, &registers.HL); return;  // B <- Mem[HL]
        case 0x47: queue_load_r8_r8(&registers.B, &registers.A); return;    // B <- A
        case 0x48: queue_load_r8_r8(&registers.C, &registers.B); return;    // C <- B
        case 0x49: return;                                                  // C <- C (essentially a NOP)
        case 0x4A: queue_load_r8_r8(&registers.C, &registers.D); return;    // C <- D
        case 0x4B: queue_load_r8_r8(&registers.C, &registers.E); return;    // C <- E
        case 0x4C: queue_load_r8_r8(&registers.C, &registers.H); return;    // C <- H
        case 0x4D: queue_load_r8_r8(&registers.C, &registers.L); return;    // C <- L
        case 0x4E: queue_load_r8_r16(&registers.C, &registers.HL); return;  // C <- Mem[HL]
        case 0x4F: queue_load_r8_r8(&registers.C, &registers.A); return;    // C <- A

        case 0x50: queue_load_r8_r8(&registers.D, &registers.B);return;     // D <- B
        case 0x51: queue_load_r8_r8(&registers.D, &registers.C); return;    // D <- C
        case 0x52: return;                                                  // D <- D (essentially a NOP)
        case 0x53: queue_load_r8_r8(&registers.D, &registers.E); return;    // D <- E
        case 0x54: queue_load_r8_r8(&registers.D, &registers.H); return;    // D <- H
        case 0x55: queue_load_r8_r8(&registers.D, &registers.L); return;    // D <- L
        case 0x56: queue_load_r8_r16(&registers.D, &registers.HL); return;  // D <- Mem[HL]
        case 0x57: queue_load_r8_r8(&registers.D, &registers.A); return;    // D <- A
        case 0x58: queue_load_r8_r8(&registers.E, &registers.B); return;    // E <- B
        case 0x59: queue_load_r8_r8(&registers.E, &registers.C); return;    // E <- C
        case 0x5A: queue_load_r8_r8(&registers.E, &registers.D); return;    // E <- D
        case 0x5B: return;                                                  // E <- E (essentially a NOP)
        case 0x5C: queue_load_r8_r8(&registers.E, &registers.H); return;    // E <- H
        case 0x5D: queue_load_r8_r8(&registers.E, &registers.L); return;    // E <- L
        case 0x5E: queue_load_r8_r16(&registers.E, &registers.HL); return;  // E <- Mem[HL]
        case 0x5F: queue_load_r8_r8(&registers.E, &registers.A); return;    // E <- A

        case 0x60: queue_load_r8_r8(&registers.H, &registers.B);return;     // H <- B
        case 0x61: queue_load_r8_r8(&registers.H, &registers.C); return;    // H <- C
        case 0x62: queue_load_r8_r8(&registers.H, &registers.D); return;    // H <- D
        case 0x63: queue_load_r8_r8(&registers.H, &registers.E); return;    // H <- E
        case 0x64: return;                                                  // H <- H (essentially a NOP)
        case 0x65: queue_load_r8_r8(&registers.H, &registers.L); return;    // H <- L
        case 0x66: queue_load_r8_r16(&registers.H, &registers.HL); return;  // H <- Mem[HL]
        case 0x67: queue_load_r8_r8(&registers.H, &registers.A); return;    // H <- A
        case 0x68: queue_load_r8_r8(&registers.L, &registers.B); return;    // L <- B
        case 0x69: queue_load_r8_r8(&registers.L, &registers.C); return;    // L <- C
        case 0x6A: queue_load_r8_r8(&registers.L, &registers.D); return;    // L <- D
        case 0x6B: queue_load_r8_r8(&registers.L, &registers.E); return;    // L <- E
        case 0x6C: queue_load_r8_r8(&registers.L, &registers.H); return;    // L <- H
        case 0x6D:  return;                                                 // L <- L (essentially a NOP)
        case 0x6E: queue_load_r8_r16(&registers.L, &registers.HL); return;  // L <- Mem[HL]
        case 0x6F: queue_load_r8_r8(&registers.L, &registers.A); return;    // L <- A

        case 0x70: queue_load_r16_r8(&registers.HL, &registers.B); return;  // Mem[HL] <- B
        case 0x71: queue_load_r16_r8(&registers.HL, &registers.C); return;  // Mem[HL] <- C
        case 0x72: queue_load_r16_r8(&registers.HL, &registers.D); return;  // Mem[HL] <- D
        case 0x73: queue_load_r16_r8(&registers.HL, &registers.E); return;  // Mem[HL] <- E
        case 0x74: queue_load_r16_r8(&registers.HL, &registers.H); return;  // Mem[HL] <- H
        case 0x75: queue_load_r16_r8(&registers.HL, &registers.L); return;  // Mem[HL] <- L
        case 0x76: /* HALT */ return;                                       // HALT
        case 0x77: queue_load_r16_r8(&registers.HL, &registers.A); return;  // Mem[HL] <- A
        case 0x78: queue_load_r8_r8(&registers.A, &registers.B); return;    // A <- B
        case 0x79: queue_load_r8_r8(&registers.A, &registers.C); return;    // A <- C
        case 0x7A: queue_load_r8_r8(&registers.A, &registers.D); return;    // A <- D
        case 0x7B: queue_load_r8_r8(&registers.A, &registers.E); return;    // A <- E
        case 0x7C: queue_load_r8_r8(&registers.A, &registers.H); return;    // A <- H
        case 0x7D: queue_load_r8_r8(&registers.A, &registers.L); return;    // A <- L
        case 0x7E: queue_load_r8_r16(&registers.A, &registers.HL); return;  // A <- Mem[HL]
        case 0x7F: return;                                                  // A <- A (essentially a NOP)

        case 0x80: queue_alu_op(&registers.B); return;    // A += B (no carry)
        case 0x81: queue_alu_op(&registers.C); return;    // A += C (no carry)
        case 0x82: queue_alu_op(&registers.D); return;    // A += D (no carry)
        case 0x83: queue_alu_op(&registers.E); return;    // A += E (no carry)
        case 0x84: queue_alu_op(&registers.H); return;    // A += H (no carry)
        case 0x85: queue_alu_op(&registers.L); return;    // A += L (no carry)
        case 0x86: /* A += Mem[HL] */ return;
        case 0x87: queue_alu_op(&registers.A); return;    // A += A (no carry)
        case 0x88: queue_alu_op(&registers.B); return;    // A += B (with carry)
        case 0x89: queue_alu_op(&registers.C); return;    // A += C (with carry)
        case 0x8A: queue_alu_op(&registers.D); return;    // A += D (with carry)
        case 0x8B: queue_alu_op(&registers.E); return;    // A += E (with carry)
        case 0x8C: queue_alu_op(&registers.H); return;    // A += H (with carry)
        case 0x8D: queue_alu_op(&registers.L); return;    // A += L (with carry)
        case 0x8E: /* A += Mem[HL] w/carry*/ return;
        case 0x8F: queue_alu_op(&registers.A); return;    // A += A (with carry)

        case 0x90: queue_alu_op(&registers.B); return;    // A -= B (no carry)
        case 0x91: queue_alu_op(&registers.C); return;    // A -= C (no carry)
        case 0x92: queue_alu_op(&registers.D); return;    // A -= D (no carry)
        case 0x93: queue_alu_op(&registers.E); return;    // A -= E (no carry)
        case 0x94: queue_alu_op(&registers.H); return;    // A -= H (no carry)
        case 0x95: queue_alu_op(&registers.L); return;    // A -= L (no carry)
        case 0x96: /* A += Mem[HL] */ return;
        case 0x97: queue_alu_op(&registers.A); return;    // A -= A (no carry)
        case 0x98: queue_alu_op(&registers.B); return;    // A -= B (with carry)
        case 0x99: queue_alu_op(&registers.C); return;    // A -= C (with carry)
        case 0x9A: queue_alu_op(&registers.D); return;    // A -= D (with carry)
        case 0x9B: queue_alu_op(&registers.E); return;    // A -= E (with carry)
        case 0x9C: queue_alu_op(&registers.H); return;    // A -= H (with carry)
        case 0x9D: queue_alu_op(&registers.L); return;    // A -= L (with carry)
        case 0x9E: /* A += Mem[HL] w/carry*/ return;
        case 0x9F: queue_alu_op(&registers.A); return;    // A -= A (with carry)

        case 0xA0: queue_alu_op(&registers.B); return;    // A &= B
        case 0xA1: queue_alu_op(&registers.C); return;    // A &= C
        case 0xA2: queue_alu_op(&registers.D); return;    // A &= D
        case 0xA3: queue_alu_op(&registers.E); return;    // A &= E
        case 0xA4: queue_alu_op(&registers.H); return;    // A &= H
        case 0xA5: queue_alu_op(&registers.L); return;    // A &= L
        case 0xA6: /* A += Mem[HL] */ return;
        case 0xA7: queue_alu_op(&registers.A); return;    // A &= A
        case 0xA8: queue_alu_op(&registers.B); return;    // A ^= B
        case 0xA9: queue_alu_op(&registers.C); return;    // A ^= C
        case 0xAA: queue_alu_op(&registers.D); return;    // A ^= D
        case 0xAB: queue_alu_op(&registers.E); return;    // A ^= E
        case 0xAC: queue_alu_op(&registers.H); return;    // A ^= H
        case 0xAD: queue_alu_op(&registers.L); return;    // A ^= L
        case 0xAE: /* A += Mem[HL] w/carry*/ return;
        case 0xAF: queue_alu_op(&registers.A); return;    // A ^= A

        case 0xB0: queue_alu_op(&registers.B); return;    // A |= B
        case 0xB1: queue_alu_op(&registers.C); return;    // A |= C
        case 0xB2: queue_alu_op(&registers.D); return;    // A |= D
        case 0xB3: queue_alu_op(&registers.E); return;    // A |= E
        case 0xB4: queue_alu_op(&registers.H); return;    // A |= H
        case 0xB5: queue_alu_op(&registers.L); return;    // A |= L
        case 0xB6: /* A += Mem[HL] */ return;
        case 0xB7: queue_alu_op(&registers.A); return;    // A |= A
        case 0xB8: queue_alu_op(&registers.B); return;    // CP(B) (Set flags for A - B)
        case 0xB9: queue_alu_op(&registers.C); return;    // CP(C) (Set flags for A - C)
        case 0xBA: queue_alu_op(&registers.D); return;    // CP(D) (Set flags for A - D)
        case 0xBB: queue_alu_op(&registers.E); return;    // CP(E) (Set flags for A - E)
        case 0xBC: queue_alu_op(&registers.H); return;    // CP(H) (Set flags for A - H)
        case 0xBD: queue_alu_op(&registers.L); return;    // CP(L) (Set flags for A - L)
        case 0xBE: /* A += Mem[HL] w/carry*/ return;
        case 0xBF: queue_alu_op(&registers.A); return;    // CP(A) (Set flags for A - A)

        case 0xC0: queue_ret_cond(); return;              // If NZ, PC[7:0] <- Mem[SP++]; PC[15:8] <- Mem[SP++]
        case 0xC1: queue_pop_r16(&registers.BC); return;  // C <- Mem[SP++]; B <- Mem[SP++]
        case 0xC2: queue_jp_cond(); return;               // If NZ, PC <- Imm16
        case 0xC3: queue_jp(); return;                    // PC <- imm16
        case 0xC4: queue_call_cond(); return;             // If NZ, Push PC, PC <- imm16
        case 0xC5: queue_push_r16(&registers.BC); return; // Mem[--SP] <- B; Mem[--SP] <- C
        case 0xC6: queue_alu_imm8(); return;              // A += Mem[PC++] (no carry)
        case 0xC7: queue_rst(); return;                   // Push PC, PC <- 0x0000
        case 0xC8: queue_ret_cond(); return;              // If Z, PC[7:0] <- Mem[SP++]; PC[15:8] <- Mem[SP++]
        case 0xC9: queue_ret(); return;                   // PC[7:0] <- Mem[SP++]; PC[15:8] <- Mem[SP++]
        case 0xCA: queue_jp_cond(); return;               // If Z, PC <- Imm16
        case 0xCB: /* CB PREFIX*/ return;                 // PREFIX
        case 0xCC: queue_call_cond(); return;             // If Z, Push PC, PC <- imm16
        case 0xCD: queue_call(); return;                  // Push PC, PC <- imm16
        case 0xCE: queue_alu_imm8(); return;              // A += Mem[PC++] (with carry)
        case 0xCF: queue_rst(); return;                   // Push PC, PC <- 0x0008

        case 0xD0: queue_ret_cond(); return;              // If NC, PC[7:0] <- Mem[SP++]; PC[15:8] <- Mem[SP++]
        case 0xD1: queue_pop_r16(&registers.DE); return;  // E <- Mem[SP++]; D <- Mem[SP++]
        case 0xD2: queue_jp_cond(); return;               // If NC, PC <- Imm16
        case 0xD3: throw std::runtime_error("Opcode: D3 is bad");
        case 0xD4: queue_call_cond(); return;             // If NC, Push PC, PC <- imm16
        case 0xD5: queue_push_r16(&registers.DE); return; // Mem[--SP] <- D; Mem[--SP] <- E
        case 0xD6: queue_alu_imm8(); return;              // A -= Mem[PC++] (no carry)
        case 0xD7: queue_rst(); return;                   // Push PC, PC <- 0x0010
        case 0xD8: queue_ret_cond(); return;              // If C, PC[7:0] <- Mem[SP++]; PC[15:8] <- Mem[SP++]
        case 0xD9: queue_reti(); return;                  // PC[7:0] <- Mem[SP++]; PC[15:8] <- Mem[SP++], IME=1
        case 0xDA: queue_jp_cond(); return;               // If C, PC <- Imm16
        case 0xDB: throw std::runtime_error("Opcode: DB is bad");
        case 0xDC: queue_call_cond(); return;             // If C, Push PC, PC <- imm16
        case 0xDD: throw std::runtime_error("Opcode: DD is bad");
        case 0xDE: queue_alu_imm8(); return;              // A -= Mem[PC++] (with carry)
        case 0xDF: queue_rst(); return;                   // Push PC, PC <- 0x0018

        case 0xE0: /* Mem[0xFF00 + Mem[PC++]] <- A */ return;
        case 0xE1: queue_pop_r16(&registers.HL); return;  // L <- Mem[SP++]; H <- Mem[SP++]
        case 0xE2: /* Mem[0xFF00 + C] <- A */ return;
        case 0xE3: throw std::runtime_error("Opcode: E3 is bad");
        case 0xE4: throw std::runtime_error("Opcode: E4 is bad");
        case 0xE5: queue_push_r16(&registers.HL); return; // Mem[--SP] <- H; Mem[--SP] <- L
        case 0xE6: queue_alu_imm8(); return;              // A &= Mem[PC++]
        case 0xE7: queue_rst(); return;                   // Push PC, PC <- 0x0020
        case 0xE8: /* SP += (int16_t)Mem[PC++] */ return;
        case 0xE9: queue_jp_hl(); return;                 // PC <- HL
        case 0xEA: queue_load_imm16_a(); return;          // Mem[Imm16] <- A
        case 0xEB: throw std::runtime_error("Opcode: EB is bad");
        case 0xEC: throw std::runtime_error("Opcode: EC is bad");
        case 0xED: throw std::runtime_error("Opcode: ED is bad");
        case 0xEE: queue_alu_imm8(); return;              // A ^= Mem[PC++]
        case 0xEF: queue_rst(); return;                   // Push PC, PC <- 0x0028

        case 0xF0: /* A <- Mem[0xFF00 + Mem[PC++]] */ return;
        case 0xF1: queue_pop_r16(&registers.AF); return;  // F <- Mem[SP++]; A <- Mem[SP++]
        case 0xF2: /* A <- Mem[0xFF00 + C] */ return;
        case 0xF3: push_state(CPUState::DI); return;      // DI
        case 0xF4: throw std::runtime_error("Opcode: F4 is bad");
        case 0xF5: queue_push_r16(&registers.AF); return; // Mem[--SP] <- A; Mem[--SP] <- F
        case 0xF6: queue_alu_imm8(); return;              // A |= Mem[PC++]
        case 0xF7: queue_rst(); return;                   // Push PC, PC <- 0x0030
        case 0xF8: /* HL = SP + (int16_t)Mem[PC++] */ return;
        case 0xF9: queue_load_sp_hl(); return;            // SP <- HL
        case 0xFA: queue_load_a_imm16(); return;          // A <- Mem[Imm16]
        case 0xFB: push_state(CPUState::EI); return;      // EI
        case 0xFC: throw std::runtime_error("Opcode: FC is bad");
        case 0xFD: throw std::runtime_error("Opcode: FD is bad");
        case 0xFE: queue_alu_imm8(); return;              // CP(Mem[PC++]) (Set flags for A - Mem[PC++])
        case 0xFF: queue_rst(); return;                   // Push PC, PC <- 0x0038

        default: throw std::runtime_error("How did you get here?");
    };
}


/**
 * CB Prefix Rotates, Shifts, and Bit Operations
 * 
 * TODO: Convert to queue system
*/
void CPU::decode_cb() {
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


/**
 * Matches opcode to operation and executes that operation
 * 
 * @param opcode an 8-bit opcode to be executed
*/
void CPU::execute_state(const CPUState state, GameBoy& bus) {
    switch(state) {
        /**
         * 1 m-cycle microstates
         */
        case CPUState::InternalDelay:                                                  
            return; 
        case CPUState::Mem_Source_to_Dest_Byte:
            *dest_byte_ptr = bus.read(source_addr_ptr->word);
            return;
        case CPUState::Mem_Source_to_Dest_Addr_Low:
            dest_addr_ptr->low = bus.read(source_addr_ptr->word);
            return;
        case CPUState::Mem_Source_to_Dest_Addr_High:
            dest_addr_ptr->high = bus.read(source_addr_ptr->word);
            return;
        case CPUState::Source_Byte_to_Mem_Dest:
            bus.write(dest_addr_ptr->word, *source_byte_ptr);
            return;
        case CPUState::Source_Addr_Low_to_Mem_Dest:
            bus.write(dest_addr_ptr->word, source_addr_ptr->low);
            return;
        case CPUState::Source_Addr_High_to_Mem_Dest:
            bus.write(dest_addr_ptr->word, source_addr_ptr->high);
            return;
        
        /**
         * 0 m-cycle microstates
         */

        case CPUState::CheckCond: {
            uint8_t condition = (current_opcode >> 3) & 0x03;
            bool condition_met = false;

            switch (condition) {
                case 0: condition_met = !get_flag(FLAG_ZERO);  break; // NZ
                case 1: condition_met =  get_flag(FLAG_ZERO);  break; // Z
                case 2: condition_met = !get_flag(FLAG_CARRY); break; // NC
                case 3: condition_met =  get_flag(FLAG_CARRY); break; // C
            }

            if (condition_met) {
                return; 
            }
            
            // If we get here, the check failed. Wipe the pipeline to skip the jump/call cycles.
            clear_pipeline();
            return; 
        }
        case CPUState::EI:
            ei_pending = true;
            return;
        case CPUState::EI_Immediate:
            interrupts_enabled = true;
            return;
        case CPUState::DI:
            interrupts_enabled = false;
            return;

        case CPUState::Source_Byte_to_Dest_Byte:
            *dest_byte_ptr = *source_byte_ptr;
            return;
        case CPUState::Source_Addr_to_Dest_Addr:
            *dest_addr_ptr = *source_addr_ptr;
            return;

        case CPUState::Make_SP_Source_Addr:
            source_addr_ptr = &registers.SP;
            return;
        case CPUState::Make_SP_Dest_Addr:
            dest_addr_ptr = &registers.SP;
            return;
        case CPUState::Make_Scratch_Source_Addr:
            source_addr_ptr = &scratch_register;
            return;

        case CPUState::INC_Source_Addr:
            (*source_addr_ptr)++;
            return;
        case CPUState::DEC_Source_Addr:
            (*source_addr_ptr)--;
            return;
        case CPUState::INC_Dest_Addr:
            (*dest_addr_ptr)++;
            return;
        case CPUState::DEC_Dest_Addr:
            (*dest_addr_ptr)--;
            return;

        case CPUState::INC_Dest_Byte:
            INC(dest_byte_ptr);
            return;
        case CPUState::DEC_Dest_Byte:
            DEC(dest_byte_ptr);
            return;

        case CPUState::EXEC_ALU: {
            uint8_t alu_op = (current_opcode >> 3) & 0x07;

            switch (alu_op) {
                case 0: ADD(*source_byte_ptr, false); return;
                case 1: ADD(*source_byte_ptr, true);  return;
                case 2: SUB(*source_byte_ptr, false); return;
                case 3: SUB(*source_byte_ptr, true);  return;
                case 4: AND(*source_byte_ptr); return;
                case 5: XOR(*source_byte_ptr); return;
                case 6: OR(*source_byte_ptr);  return;
                case 7: CP(*source_byte_ptr);  return;
            }
        }
        case CPUState::ADD_HL:
            ADD_HL(source_addr_ptr->word);
            return;

        case CPUState::JR:
            registers.PC.word += (int16_t)(int8_t)scratch_register.low;
            return;
        case CPUState::JP:
            registers.PC.word = scratch_register.word;
            return;
        
        default: throw std::runtime_error("Execute state called on bad CPU State");
    } 
}