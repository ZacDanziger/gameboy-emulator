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
        case 0x07: push_microop(MicroOp::EXEC_AF_OP); return;       // RLCA
        case 0x08: queue_store_sp_imm16(); return;                  // Mem[Imm16] <- SP[7:0]; Mem[Imm16+1] <- SP[15:8]
        case 0x09: queue_add_hl_r16(&registers.BC); return;         // HL += BC
        case 0x0A: queue_load_a_mem_r16(&registers.BC); return;     // A <- Mem[BC]
        case 0x0B: queue_dec_r16(&registers.BC); return;            // BC--
        case 0x0C: queue_inc_r8(&registers.C); return;              // C++
        case 0x0D: queue_dec_r8(&registers.C); return;              // C--
        case 0x0E: queue_load_r8_imm8(&registers.C); return;        // C <- Mem[PC++]
        case 0x0F: push_microop(MicroOp::EXEC_AF_OP); return;       // RRCA

        case 0x10: push_microop(MicroOp::EXEC_STOP); return;        // STOP
        case 0x11: queue_load_r16_imm16(&registers.DE); return;     // DE <- Mem[Imm16]
        case 0x12: queue_store_a_mem_r16(&registers.DE); return;    // Mem[DE] <- A
        case 0x13: queue_inc_r16(&registers.DE); return;            // DE++
        case 0x14: queue_inc_r8(&registers.D); return;              // D++
        case 0x15: queue_dec_r8(&registers.D); return;              // D--
        case 0x16: queue_load_r8_imm8(&registers.D); return;        // D <- Mem[PC++]
        case 0x17: push_microop(MicroOp::EXEC_AF_OP); return;       // RLA
        case 0x18: queue_jr(); return;                              // PC += (int16_t)(Mem[PC++])
        case 0x19: queue_add_hl_r16(&registers.DE); return;         // HL += DE
        case 0x1A: queue_load_a_mem_r16(&registers.DE); return;     // A <- Mem[DE]
        case 0x1B: queue_dec_r16(&registers.DE); return;            // DE--
        case 0x1C: queue_inc_r8(&registers.E); return;              // E++
        case 0x1D: queue_dec_r8(&registers.E); return;              // E--
        case 0x1E: queue_load_r8_imm8(&registers.E); return;        // E <- Mem[PC++]
        case 0x1F: push_microop(MicroOp::EXEC_AF_OP); return;       // RRA

        case 0x20: queue_jr_cond(); return;                         // If NZ, PC += (int16_t)(Mem[PC++])
        case 0x21: queue_load_r16_imm16(&registers.HL); return;     // HL <- Mem[Imm16]
        case 0x22: queue_store_a_mem_r16(&registers.HL); return;    // Mem[HL++] <- A
        case 0x23: queue_inc_r16(&registers.HL); return;            // HL++
        case 0x24: queue_inc_r8(&registers.H); return;              // H++
        case 0x25: queue_dec_r8(&registers.H); return;              // H--
        case 0x26: queue_load_r8_imm8(&registers.H); return;        // H <- Mem[PC++]
        case 0x27: push_microop(MicroOp::EXEC_AF_OP); return;       // DAA
        case 0x28: queue_jr_cond(); return;                         // If Z, PC += (int16_t)(Mem[PC++])
        case 0x29: queue_add_hl_r16(&registers.HL); return;         // HL += HL
        case 0x2A: queue_load_a_mem_r16(&registers.HL); return;     // A <- Mem[HL++]
        case 0x2B: queue_dec_r16(&registers.HL); return;            // HL--
        case 0x2C: queue_inc_r8(&registers.L); return;              // L++
        case 0x2D: queue_dec_r8(&registers.L); return;              // L--
        case 0x2E: queue_load_r8_imm8(&registers.L); return;        // L <- Mem[PC++]
        case 0x2F: push_microop(MicroOp::EXEC_AF_OP); return;       // CPL

        case 0x30: queue_jr_cond(); return;                         // If NC, PC += (int16_t)(Mem[PC++])
        case 0x31: queue_load_r16_imm16(&registers.SP); return;     // SP <- Mem[Imm16]
        case 0x32: queue_store_a_mem_r16(&registers.HL); return;    // Mem[HL--] <- A
        case 0x33: queue_inc_r16(&registers.SP); return;            // SP++
        case 0x34: queue_inc_dec_mem_hl(); return;                  // Mem[HL]++
        case 0x35: queue_inc_dec_mem_hl(); return;                  // Mem[HL]--
        case 0x36: queue_load_mem_hl_imm8(); return;                // Mem[HL] <- Mem[PC++]
        case 0x37: push_microop(MicroOp::EXEC_AF_OP); return;       // SCF
        case 0x38: queue_jr_cond(); return;                         // If C, PC += (int16_t)(Mem[PC++])
        case 0x39: queue_add_hl_r16(&registers.SP); return;         // HL += SP
        case 0x3A: queue_load_a_mem_r16(&registers.HL); return;     // A <- Mem[HL--]
        case 0x3B: queue_dec_r16(&registers.SP); return;            // SP--
        case 0x3C: queue_inc_r8(&registers.A); return;              // A++
        case 0x3D: queue_dec_r8(&registers.A); return;              // A--
        case 0x3E: queue_load_r8_imm8(&registers.A); return;        // A <- Mem[PC++]
        case 0x3F: push_microop(MicroOp::EXEC_AF_OP); return;       // CCF

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
        case 0x76: push_microop(MicroOp::EXEC_HALT); return;                // HALT
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
        case 0x86: queue_alu_op_mem_hl(); return;         // A += Mem[HL] (no carry)
        case 0x87: queue_alu_op(&registers.A); return;    // A += A (no carry)
        case 0x88: queue_alu_op(&registers.B); return;    // A += B (with carry)
        case 0x89: queue_alu_op(&registers.C); return;    // A += C (with carry)
        case 0x8A: queue_alu_op(&registers.D); return;    // A += D (with carry)
        case 0x8B: queue_alu_op(&registers.E); return;    // A += E (with carry)
        case 0x8C: queue_alu_op(&registers.H); return;    // A += H (with carry)
        case 0x8D: queue_alu_op(&registers.L); return;    // A += L (with carry)
        case 0x8E: queue_alu_op_mem_hl(); return;         // A += Mem[HL] (with carry)
        case 0x8F: queue_alu_op(&registers.A); return;    // A += A (with carry)

        case 0x90: queue_alu_op(&registers.B); return;    // A -= B (no carry)
        case 0x91: queue_alu_op(&registers.C); return;    // A -= C (no carry)
        case 0x92: queue_alu_op(&registers.D); return;    // A -= D (no carry)
        case 0x93: queue_alu_op(&registers.E); return;    // A -= E (no carry)
        case 0x94: queue_alu_op(&registers.H); return;    // A -= H (no carry)
        case 0x95: queue_alu_op(&registers.L); return;    // A -= L (no carry)
        case 0x96: queue_alu_op_mem_hl(); return;         // A -= Mem[HL] (no carry)
        case 0x97: queue_alu_op(&registers.A); return;    // A -= A (no carry)
        case 0x98: queue_alu_op(&registers.B); return;    // A -= B (with carry)
        case 0x99: queue_alu_op(&registers.C); return;    // A -= C (with carry)
        case 0x9A: queue_alu_op(&registers.D); return;    // A -= D (with carry)
        case 0x9B: queue_alu_op(&registers.E); return;    // A -= E (with carry)
        case 0x9C: queue_alu_op(&registers.H); return;    // A -= H (with carry)
        case 0x9D: queue_alu_op(&registers.L); return;    // A -= L (with carry)
        case 0x9E: queue_alu_op_mem_hl(); return;         // A -= Mem[HL] (with carry)
        case 0x9F: queue_alu_op(&registers.A); return;    // A -= A (with carry)

        case 0xA0: queue_alu_op(&registers.B); return;    // A &= B
        case 0xA1: queue_alu_op(&registers.C); return;    // A &= C
        case 0xA2: queue_alu_op(&registers.D); return;    // A &= D
        case 0xA3: queue_alu_op(&registers.E); return;    // A &= E
        case 0xA4: queue_alu_op(&registers.H); return;    // A &= H
        case 0xA5: queue_alu_op(&registers.L); return;    // A &= L
        case 0xA6: queue_alu_op_mem_hl(); return;         // A &= Mem[HL]
        case 0xA7: queue_alu_op(&registers.A); return;    // A &= A
        case 0xA8: queue_alu_op(&registers.B); return;    // A ^= B
        case 0xA9: queue_alu_op(&registers.C); return;    // A ^= C
        case 0xAA: queue_alu_op(&registers.D); return;    // A ^= D
        case 0xAB: queue_alu_op(&registers.E); return;    // A ^= E
        case 0xAC: queue_alu_op(&registers.H); return;    // A ^= H
        case 0xAD: queue_alu_op(&registers.L); return;    // A ^= L
        case 0xAE: queue_alu_op_mem_hl(); return;         // A ^= Mem[HL]
        case 0xAF: queue_alu_op(&registers.A); return;    // A ^= A

        case 0xB0: queue_alu_op(&registers.B); return;    // A |= B
        case 0xB1: queue_alu_op(&registers.C); return;    // A |= C
        case 0xB2: queue_alu_op(&registers.D); return;    // A |= D
        case 0xB3: queue_alu_op(&registers.E); return;    // A |= E
        case 0xB4: queue_alu_op(&registers.H); return;    // A |= H
        case 0xB5: queue_alu_op(&registers.L); return;    // A |= L
        case 0xB6: queue_alu_op_mem_hl(); return;         // A |= Mem[HL]
        case 0xB7: queue_alu_op(&registers.A); return;    // A |= A
        case 0xB8: queue_alu_op(&registers.B); return;    // CP(B) (Set flags for A - B)
        case 0xB9: queue_alu_op(&registers.C); return;    // CP(C) (Set flags for A - C)
        case 0xBA: queue_alu_op(&registers.D); return;    // CP(D) (Set flags for A - D)
        case 0xBB: queue_alu_op(&registers.E); return;    // CP(E) (Set flags for A - E)
        case 0xBC: queue_alu_op(&registers.H); return;    // CP(H) (Set flags for A - H)
        case 0xBD: queue_alu_op(&registers.L); return;    // CP(L) (Set flags for A - L)
        case 0xBE: queue_alu_op_mem_hl(); return;         // CP(Mem[HL]) (Set flags for A - Mem[HL])
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
        case 0xCB: queue_prefix_cb(); return;             // PREFIX
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

        case 0xE0: queue_load_high_imm8_a(); return;      // Mem[0xFF00 + Mem[PC++]] <- A
        case 0xE1: queue_pop_r16(&registers.HL); return;  // L <- Mem[SP++]; H <- Mem[SP++]
        case 0xE2: queue_load_high_c_a(); return;         // Mem[0xFF00 + C] <- A
        case 0xE3: throw std::runtime_error("Opcode: E3 is bad");
        case 0xE4: throw std::runtime_error("Opcode: E4 is bad");
        case 0xE5: queue_push_r16(&registers.HL); return; // Mem[--SP] <- H; Mem[--SP] <- L
        case 0xE6: queue_alu_imm8(); return;              // A &= Mem[PC++]
        case 0xE7: queue_rst(); return;                   // Push PC, PC <- 0x0020
        case 0xE8: queue_add_sp(); return;                // SP += (int16_t)Mem[PC++]
        case 0xE9: queue_jp_hl(); return;                 // PC <- HL
        case 0xEA: queue_load_imm16_a(); return;          // Mem[Imm16] <- A
        case 0xEB: throw std::runtime_error("Opcode: EB is bad");
        case 0xEC: throw std::runtime_error("Opcode: EC is bad");
        case 0xED: throw std::runtime_error("Opcode: ED is bad");
        case 0xEE: queue_alu_imm8(); return;              // A ^= Mem[PC++]
        case 0xEF: queue_rst(); return;                   // Push PC, PC <- 0x0028

        case 0xF0: queue_load_high_a_imm8(); return;      // A <- Mem[0xFF00 + Mem[PC++]]
        case 0xF1: queue_pop_r16(&registers.AF); return;  // F <- Mem[SP++]; A <- Mem[SP++]
        case 0xF2: queue_load_high_a_c(); return;         // A <- Mem[0xFF00 + C]
        case 0xF3: push_microop(MicroOp::EXEC_DI); return;// DI
        case 0xF4: throw std::runtime_error("Opcode: F4 is bad");
        case 0xF5: queue_push_r16(&registers.AF); return; // Mem[--SP] <- A; Mem[--SP] <- F
        case 0xF6: queue_alu_imm8(); return;              // A |= Mem[PC++]
        case 0xF7: queue_rst(); return;                   // Push PC, PC <- 0x0030
        case 0xF8: queue_load_hl_sp_e8(); return;         // HL = SP + (int16_t)Mem[PC++]
        case 0xF9: queue_load_sp_hl(); return;            // SP <- HL
        case 0xFA: queue_load_a_imm16(); return;          // A <- Mem[Imm16]
        case 0xFB: push_microop(MicroOp::EXEC_EI); return;// EI
        case 0xFC: throw std::runtime_error("Opcode: FC is bad");
        case 0xFD: throw std::runtime_error("Opcode: FD is bad");
        case 0xFE: queue_alu_imm8(); return;              // CP(Mem[PC++]) (Set flags for A - Mem[PC++])
        case 0xFF: queue_rst(); return;                   // Push PC, PC <- 0x0038

        default: throw std::runtime_error("How did you get here?");
    };
}


/**
 * CB Prefix Rotates, Shifts, and Bit Operations
*/
void CPU::decode_cb() {
    uint8_t target = current_opcode & 0x07;

    switch (target) {
        case 0: queue_cb_op(&registers.B); return;
        case 1: queue_cb_op(&registers.C); return;
        case 2: queue_cb_op(&registers.D); return;
        case 3: queue_cb_op(&registers.E); return;
        case 4: queue_cb_op(&registers.H); return;
        case 5: queue_cb_op(&registers.L); return;
        case 6: {   // Mem[HL]
            uint8_t op_type = (current_opcode >> 6) & 0x03;
            if (op_type == 1) { queue_cb_bit_mem_hl(); }
            else { queue_cb_op_mem_hl(); }
            return;
        }
        case 7: queue_cb_op(&registers.A); return;

        default: throw std::runtime_error("How did you get here?");
    };
}


/**
 * Matches opcode to operation and executes that operation
 * 
 * @param opcode an 8-bit opcode to be executed
*/
void CPU::execute_microop(const MicroOp microop, GameBoy& bus) {
    switch(microop) {
        /**
         * 1 m-cycle microops
         */
        case MicroOp::InternalDelay:                                                  
            return; 
        
        case MicroOp::ReadMemToDestByte:
            *dest_byte_ptr = bus.read(source_addr_ptr->word);
            return;
        case MicroOp::ReadMemToDestLow:
            dest_addr_ptr->low = bus.read(source_addr_ptr->word);
            return;
        case MicroOp::ReadMemToDestHigh:
            dest_addr_ptr->high = bus.read(source_addr_ptr->word);
            return;
        case MicroOp::WriteMemFromSourceByte:
            bus.write(dest_addr_ptr->word, *source_byte_ptr);
            return;
        case MicroOp::WriteMemFromSourceLow:
            bus.write(dest_addr_ptr->word, source_addr_ptr->low);
            return;
        case MicroOp::WriteMemFromSourceHigh:
            bus.write(dest_addr_ptr->word, source_addr_ptr->high);
            return;
        
        /**
         * 0 m-cycle microops
         */

        case MicroOp::CheckCond: {
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
            
            // If we get here, the check failed. Wipe the queue to skip the jump/call cycles.
            clear_queue();
            return; 
        }
        case MicroOp::EXEC_EI:
            ei_pending = true;
            return;
        case MicroOp::EXEC_EI_Immediate:
            interrupts_enabled = true;
            return;
        case MicroOp::EXEC_DI:
            interrupts_enabled = false;
            return;
        
        case MicroOp::EXEC_HALT:
            // TODO: Add HALT Bug
            halted = true;
            return;
        case MicroOp::EXEC_STOP:
            stopped = true;
            return;

        case MicroOp::EXEC_DECODE_CB:
            decode_cb();
            return;

        case MicroOp::CopySourceToDestByte:
            *dest_byte_ptr = *source_byte_ptr;
            return;
        case MicroOp::CopySourceToDestAddr:
            *dest_addr_ptr = *source_addr_ptr;
            return;

        case MicroOp::PointSourceToSP:
            source_addr_ptr = &registers.SP;
            return;
        case MicroOp::PointDestToSP:
            dest_addr_ptr = &registers.SP;
            return;
        case MicroOp::PointSourceToScratch:
            source_addr_ptr = &scratch_register;
            return;
        case MicroOp::PointDestToHL:
            dest_addr_ptr = &registers.HL;
            return;

        case MicroOp::INC_SOURCE_ADDR:
            (*source_addr_ptr)++;
            return;
        case MicroOp::DEC_SOURCE_ADDR:
            (*source_addr_ptr)--;
            return;
        case MicroOp::INC_DEST_ADDR:
            (*dest_addr_ptr)++;
            return;
        case MicroOp::DEC_DEST_ADDR:
            (*dest_addr_ptr)--;
            return;
        case MicroOp::MASK_F:
            registers.F &= 0xF0;
            return;

        case MicroOp::EXEC_INC_DEST_BYTE:
            INC(dest_byte_ptr);
            return;
        case MicroOp::EXEC_DEC_DEST_BYTE:
            DEC(dest_byte_ptr);
            return;

        case MicroOp::EXEC_ALU: {
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
        case MicroOp::EXEC_AF_OP: {
            switch (current_opcode) {
                case 0x07: RL(registers.A, true); update_flag(FLAG_ZERO, false); return;
                case 0x0F: RR(registers.A, true); update_flag(FLAG_ZERO, false); return;
                case 0x17: RL(registers.A, false); update_flag(FLAG_ZERO, false); return;
                case 0x1F: RR(registers.A, false); update_flag(FLAG_ZERO, false); return;
                case 0x27: DAA(); return;
                case 0x2F: CPL(); return;
                case 0x37: SCF(); return;
                case 0x3F: CCF(); return;
            };
        }
        case MicroOp::EXEC_CB_OP: {
            uint8_t op_type = (current_opcode >> 6) & 0x03;
            uint8_t op_or_bit_index = (current_opcode >> 3) & 0x07;
            Bit bit = (Bit)(1 << op_or_bit_index);

            switch (op_type) {
                // Rotates, Shifts, and Swaps
                case 0: {
                    switch (op_or_bit_index) {
                        case 0: RL(*dest_byte_ptr, true); return;
                        case 1: RR(*dest_byte_ptr, true); return;
                        case 2: RL(*dest_byte_ptr, false); return;
                        case 3: RR(*dest_byte_ptr, false); return;
                        case 4: SLA(*dest_byte_ptr); return;
                        case 5: SRA(*dest_byte_ptr); return;
                        case 6: SWAP(*dest_byte_ptr); return;
                        case 7: SRL(*dest_byte_ptr); return;
                    };
                    return;
                }
                case 1: BIT(*dest_byte_ptr, bit); return;
                case 2: RES(*dest_byte_ptr, bit); return;
                case 3: SET(*dest_byte_ptr, bit); return;
            };
        }

        case MicroOp::EXEC_ADD_HL:
            ADD_HL(source_addr_ptr->word);
            return;
        case MicroOp::EXEC_ADD_SP: {
            dest_addr_ptr->word = ADD_SP();
            return;
        }

        case MicroOp::EXEC_JR:
            registers.PC.word += (int16_t)(int8_t)scratch_register.low;
            return;
        case MicroOp::EXEC_JP:
            registers.PC.word = scratch_register.word;
            return;
        
        default: throw std::runtime_error("Execute MicroOp called on bad MicroOp");
    } 
}