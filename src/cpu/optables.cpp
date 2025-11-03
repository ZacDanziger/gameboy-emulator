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
        case 0x00: { return 4; }    // NOP
        case 0x01: {}   // LD BC, n16
        case 0x02: {}   // LD [BC], A
        case 0x03: {}   // INC BC
        case 0x04: { INC(reg_B); return 4; }
        case 0x05: { DEC(reg_B); return 4; }
        case 0x06: { LD(reg_B, fetch()); return 8; }
        case 0x07: {}   // RLCA
        case 0x08: {}   // LD [a16], SP
        case 0x09: {}   // ADD HL, BC
        case 0x0A: { LD(reg_A, pair(reg_B, reg_C)); return 8; }
        case 0x0B: {}   // DEC BC
        case 0x0C: { INC(reg_C); return 4; }
        case 0x0D: { DEC(reg_C); return 4; }
        case 0x0E: { LD(reg_C, fetch()); return 4; }
        case 0x0F: {}   // RRCA
        case 0x10: {}   // STOP (n8 ???)
        case 0x11: {}   // LD DE, n16
        case 0x12: { LD(pair(reg_D, reg_E), reg_A); return 8; }
        case 0x13: {}   // INC DE
        case 0x14: { INC(reg_D); return 4; }
        case 0x15: { DEC(reg_D); return 4; }
        case 0x16: { LD(reg_D, fetch()); return 8; }
        case 0x17: {}   // RLA
        case 0x18: {}   // JR e8
        case 0x19: {}   // ADD HL, DE
        case 0x1A: { LD(reg_A, pair(reg_D, reg_E)); return 8; }
        case 0x1B: {}   // DEC DE
        case 0x1C: { INC(reg_E); return 4; }
        case 0x1D: { DEC(reg_E); return 4; }
        case 0x1E: { LD(reg_E, fetch()); return 8; }
        case 0x1F: {}   // RRA
        case 0x20: {}   // JR NZ, e8
        case 0x21: {}   // LD HL, n16
        case 0x22: { set_hl(reg_A); /* INCREMENT HL HERE*/ return 8; }   // LD [HL+], A ---------------
        case 0x23: {}   // INC HL
        case 0x24: { INC(reg_H); return 4; }
        case 0x25: { DEC(reg_H); return 4; }
        case 0x26: { LD(reg_H, fetch()); return 8; }
        case 0x27: {}   // DAA
        case 0x28: {}   // JR Z, e8
        case 0x29: {}   // ADD HL, HL
        case 0x2A: { LD(reg_A, get_hl()); /* DECREMENT HL HERE*/ return 8; }   // LD A, [HL+] ---------------
        case 0x2B: {}   // DEC HL
        case 0x2C: { INC(reg_L); return 4; }
        case 0x2D: { DEC(reg_L); return 4; }
        case 0x2E: { LD(reg_L, fetch()); return 8; }
        case 0x2F: { CPL(); return 4; }   // CPL
        case 0x30: {}   // JR NC, e8
        case 0x31: {}   //LD SP, n16
        case 0x32: { set_hl(reg_A); /* DECREMENT HL HERE*/ return 8; }   //LD [HL-], A ---------------
        case 0x33: {}   // INC SP
        case 0x34: {}   // INC [HL]
        case 0x35: {}   // DEC [HL]
        case 0x36: { set_hl(fetch()); return 12; }
        case 0x37: { SCF(); return 4; }
        case 0x38: {}   // JR C, e8
        case 0x39: {}   // ADD HL, SP
        case 0x3A: { LD(reg_A, get_hl()); /* DECREMENT HL HERE*/ return 8; }   // LD A, [HL-] ---------------
        case 0x3B: {}   // DEC SP
        case 0x3C: { INC(reg_A); return 4; }
        case 0x3D: { DEC(reg_A); return 4; }
        case 0x3E: { LD(reg_A, fetch()); return 8; }
        case 0x3F: { CCF(); return 4; }
        case 0x40: { return 4; }   // LD B, B - essentially a NOP
        case 0x41: { LD(reg_B, reg_C); return 4; }
        case 0x42: { LD(reg_B, reg_D); return 4; }
        case 0x43: { LD(reg_B, reg_E); return 4; }
        case 0x44: { LD(reg_B, reg_H); return 4; }
        case 0x45: { LD(reg_B, reg_L); return 4; }
        case 0x46: { LD(reg_B, get_hl()); return 8; }
        case 0x47: { LD(reg_B, reg_A); return 4; }
        case 0x48: { LD(reg_C, reg_B); return 4; }   
        case 0x49: { return 4; }   // LD C, C - essentially a NOP
        case 0x4A: { LD(reg_C, reg_D); return 4; }
        case 0x4B: { LD(reg_C, reg_E); return 4; }
        case 0x4C: { LD(reg_C, reg_H); return 4; }
        case 0x4D: { LD(reg_C, reg_L); return 4; }
        case 0x4E: { LD(reg_C, get_hl()); return 8; }
        case 0x4F: { LD(reg_C, reg_A); return 4; }
        case 0x50: { LD(reg_D, reg_B); return 4; }
        case 0x51: { LD(reg_D, reg_C); return 4; }
        case 0x52: { return 4; }    // LD D, D - essentially a NOP
        case 0x53: { LD(reg_D, reg_E); return 4; }
        case 0x54: { LD(reg_D, reg_H); return 4; }
        case 0x55: { LD(reg_D, reg_L); return 4; }
        case 0x56: { LD(reg_D, get_hl()); return 8; }
        case 0x57: { LD(reg_D, reg_A); return 4; }
        case 0x58: { LD(reg_E, reg_B); return 4; }
        case 0x59: { LD(reg_E, reg_C); return 4; }
        case 0x5A: { LD(reg_E, reg_D); return 4; }
        case 0x5B: {return 4; }     // LD E, E - essentially a NOP
        case 0x5C: { LD(reg_E, reg_H); return 4; }
        case 0x5D: { LD(reg_E, reg_L); return 4; }
        case 0x5E: { LD(reg_E, get_hl()); return 8; }
        case 0x5F: { LD(reg_E, reg_A); return 4; }
        case 0x60: { LD(reg_H, reg_B); return 4; }
        case 0x61: { LD(reg_H, reg_C); return 4; }
        case 0x62: { LD(reg_H, reg_D); return 4; }
        case 0x63: { LD(reg_H, reg_E); return 4; }
        case 0x64: { return 4; }    // LD H, H - essentially a NOP
        case 0x65: { LD(reg_H, reg_L); return 4; }
        case 0x66: { LD(reg_H, get_hl()); return 8; }
        case 0x67: { LD(reg_H, reg_A); return 4; }
        case 0x68: { LD(reg_L, reg_B); return 4; }
        case 0x69: { LD(reg_L, reg_C); return 4; }
        case 0x6A: { LD(reg_L, reg_D); return 4; }
        case 0x6B: { LD(reg_L, reg_E); return 4; }
        case 0x6C: { LD(reg_L, reg_H); return 4; }
        case 0x6D: { LD(reg_L, reg_L); return 4; }
        case 0x6E: { LD(reg_L, get_hl()); return 8; }
        case 0x6F: { LD(reg_L, reg_A); return 4; }
        case 0x70: { set_hl(reg_B); return 8; }
        case 0x71: { set_hl(reg_C); return 8; }
        case 0x72: { set_hl(reg_D); return 8; }
        case 0x73: { set_hl(reg_E); return 8; }
        case 0x74: { set_hl(reg_H); return 8; }
        case 0x75: { set_hl(reg_L); return 8; }
        case 0x76: {}   // HALT
        case 0x77: { set_hl(reg_A); return 8; }
        case 0x78: { LD(reg_A, reg_B); return 4; }
        case 0x79: { LD(reg_A, reg_C); return 4; }
        case 0x7A: { LD(reg_A, reg_D); return 4; }
        case 0x7B: { LD(reg_A, reg_E); return 4; }
        case 0x7C: { LD(reg_A, reg_H); return 4; }
        case 0x7D: { LD(reg_A, reg_L); return 4; }
        case 0x7E: { LD(reg_A, get_hl()); return 8; }
        case 0x7F: {}   // LD H, H - essentially a NOP
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
    }

    // For future CB:
    // case 0x00: { }
    //     case 0x01: {}
    //     case 0x02: {}
    //     case 0x03: {}
    //     case 0x04: {}
    //     case 0x05: {}
    //     case 0x06: {}
    //     case 0x07: {}
    //     case 0x08: {}
    //     case 0x09: {}
    //     case 0x0A: {}
    //     case 0x0B: {}
    //     case 0x0C: {}
    //     case 0x0D: {}
    //     case 0x0E: {}
    //     case 0x0F: {}
    //     case 0x10: {}
    //     case 0x11: {}
    //     case 0x12: {}
    //     case 0x13: {}
    //     case 0x14: {}
    //     case 0x15: {}
    //     case 0x16: {}
    //     case 0x17: {}
    //     case 0x18: {}
    //     case 0x19: {}
    //     case 0x1A: {}
    //     case 0x1B: {}
    //     case 0x1C: {}
    //     case 0x1D: {}
    //     case 0x1E: {}
    //     case 0x1F: {}
    //     case 0x20: {}
    //     case 0x21: {}
    //     case 0x22: {}
    //     case 0x23: {}
    //     case 0x24: {}
    //     case 0x25: {}
    //     case 0x26: {}
    //     case 0x27: {}
    //     case 0x28: {}
    //     case 0x29: {}
    //     case 0x2A: {}
    //     case 0x2B: {}
    //     case 0x2C: {}
    //     case 0x2D: {}
    //     case 0x2E: {}
    //     case 0x2F: {}
    //     case 0x30: {}
    //     case 0x31: {}
    //     case 0x32: {}
    //     case 0x33: {}
    //     case 0x34: {}
    //     case 0x35: {}
    //     case 0x36: {}
    //     case 0x37: {}
    //     case 0x38: {}
    //     case 0x39: {}
    //     case 0x3A: {}
    //     case 0x3B: {}
    //     case 0x3C: {}
    //     case 0x3D: {}
    //     case 0x3E: {}
    //     case 0x3F: {}
    //     case 0x40: {}
    //     case 0x41: {}
    //     case 0x42: {}
    //     case 0x43: {}
    //     case 0x44: {}
    //     case 0x45: {}
    //     case 0x46: {}
    //     case 0x47: {}
    //     case 0x48: {}
    //     case 0x49: {}
    //     case 0x4A: {}
    //     case 0x4B: {}
    //     case 0x4C: {}
    //     case 0x4D: {}
    //     case 0x4E: {}
    //     case 0x4F: {}
    //     case 0x50: {}
    //     case 0x51: {}
    //     case 0x52: {}
    //     case 0x53: {}
    //     case 0x54: {}
    //     case 0x55: {}
    //     case 0x56: {}
    //     case 0x57: {}
    //     case 0x58: {}
    //     case 0x59: {}
    //     case 0x5A: {}
    //     case 0x5B: {}
    //     case 0x5C: {}
    //     case 0x5D: {}
    //     case 0x5E: {}
    //     case 0x5F: {}
    //     case 0x60: {}
    //     case 0x61: {}
    //     case 0x62: {}
    //     case 0x63: {}
    //     case 0x64: {}
    //     case 0x65: {}
    //     case 0x66: {}
    //     case 0x67: {}
    //     case 0x68: {}
    //     case 0x69: {}
    //     case 0x6A: {}
    //     case 0x6B: {}
    //     case 0x6C: {}
    //     case 0x6D: {}
    //     case 0x6E: {}
    //     case 0x6F: {}
    //     case 0x70: {}
    //     case 0x71: {}
    //     case 0x72: {}
    //     case 0x73: {}
    //     case 0x74: {}
    //     case 0x75: {}
    //     case 0x76: {}
    //     case 0x77: {}
    //     case 0x78: {}
    //     case 0x79: {}
    //     case 0x7A: {}
    //     case 0x7B: {}
    //     case 0x7C: {}
    //     case 0x7D: {}
    //     case 0x7E: {}
    //     case 0x7F: {}
    //     case 0x80: {}
    //     case 0x81: {}
    //     case 0x82: {}
    //     case 0x83: {}
    //     case 0x84: {}
    //     case 0x85: {}
    //     case 0x86: {}
    //     case 0x87: {}
    //     case 0x88: {}
    //     case 0x89: {}
    //     case 0x8A: {}
    //     case 0x8B: {}
    //     case 0x8C: {}
    //     case 0x8D: {}
    //     case 0x8E: {}
    //     case 0x8F: {}
    //     case 0x90: {}
    //     case 0x91: {}
    //     case 0x92: {}
    //     case 0x93: {}
    //     case 0x94: {}
    //     case 0x95: {}
    //     case 0x96: {}
    //     case 0x97: {}
    //     case 0x98: {}
    //     case 0x99: {}
    //     case 0x9A: {}
    //     case 0x9B: {}
    //     case 0x9C: {}
    //     case 0x9D: {}
    //     case 0x9E: {}
    //     case 0x9F: {}
    //     case 0xA0: {}
    //     case 0xA1: {}
    //     case 0xA2: {}
    //     case 0xA3: {}
    //     case 0xA4: {}
    //     case 0xA5: {}
    //     case 0xA6: {}
    //     case 0xA7: {}
    //     case 0xA8: {}
    //     case 0xA9: {}
    //     case 0xAA: {}
    //     case 0xAB: {}
    //     case 0xAC: {}
    //     case 0xAD: {}
    //     case 0xAE: {}
    //     case 0xAF: {}
    //     case 0xB0: {}
    //     case 0xB1: {}
    //     case 0xB2: {}
    //     case 0xB3: {}
    //     case 0xB4: {}
    //     case 0xB5: {}
    //     case 0xB6: {}
    //     case 0xB7: {}
    //     case 0xB8: {}
    //     case 0xB9: {}
    //     case 0xBA: {}
    //     case 0xBB: {}
    //     case 0xBC: {}
    //     case 0xBD: {}
    //     case 0xBE: {}
    //     case 0xBF: {}
    //     case 0xC0: {}
    //     case 0xC1: {}
    //     case 0xC2: {}
    //     case 0xC3: {}
    //     case 0xC4: {}
    //     case 0xC5: {}
    //     case 0xC6: {}
    //     case 0xC7: {}
    //     case 0xC8: {}
    //     case 0xC9: {}
    //     case 0xCA: {}
    //     case 0xCB: {}
    //     case 0xCC: {}
    //     case 0xCD: {}
    //     case 0xCE: {}
    //     case 0xCF: {}
    //     case 0xD0: {}
    //     case 0xD1: {}
    //     case 0xD2: {}
    //     case 0xD3: {}
    //     case 0xD4: {}
    //     case 0xD5: {}
    //     case 0xD6: {}
    //     case 0xD7: {}
    //     case 0xD8: {}
    //     case 0xD9: {}
    //     case 0xDA: {}
    //     case 0xDB: {}
    //     case 0xDC: {}
    //     case 0xDD: {}
    //     case 0xDE: {}
    //     case 0xDF: {}
    //     case 0xE0: {}
    //     case 0xE1: {}
    //     case 0xE2: {}
    //     case 0xE3: {}
    //     case 0xE4: {}
    //     case 0xE5: {}
    //     case 0xE6: {}
    //     case 0xE7: {}
    //     case 0xE8: {}
    //     case 0xE9: {}
    //     case 0xEA: {}
    //     case 0xEB: {}
    //     case 0xEC: {}
    //     case 0xED: {}
    //     case 0xEE: {}
    //     case 0xEF: {}
    //     case 0xF0: {}
    //     case 0xF1: {}
    //     case 0xF2: {}
    //     case 0xF3: {}
    //     case 0xF4: {}
    //     case 0xF5: {}
    //     case 0xF6: {}
    //     case 0xF7: {}
    //     case 0xF8: {}
    //     case 0xF9: {}
    //     case 0xFA: {}
    //     case 0xFB: {}
    //     case 0xFC: {}
    //     case 0xFD: {}
    //     case 0xFE: {}
    //     case 0xFF: {}
    return 0;
}