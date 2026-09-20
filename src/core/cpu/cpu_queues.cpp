#include "cpu.h"


void CPU::queue_load_r16_imm16(Register16* dest_reg) {
    dest_addr_ptr = dest_reg;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // Low <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // High <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++
}


void CPU::queue_store_a_mem_r16(Register16* dest_addr) {
    dest_addr_ptr = dest_addr;
    source_byte_ptr = &registers.A;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::WriteMemFromSourceByte);

    // HL+
    if (current_opcode == 0x22) {
        push_microop(MicroOp::INC_DEST_ADDR);
    // HL-
    } else if (current_opcode == 0x32) {
        push_microop(MicroOp::DEC_DEST_ADDR);
    }
}


void CPU::queue_load_r8_imm8(Byte* dest_byte) {
    dest_byte_ptr = dest_byte;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);        // dest <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);          // PC++
}


void CPU::queue_store_sp_imm16() {
    source_addr_ptr = &registers.PC;
    dest_addr_ptr = &scratch_register;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // scratch [15:8] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++
    push_microop(MicroOp::PointSourceToSP);              // source_addr = SP

    // M-cycle 4
    push_microop(MicroOp::WriteMemFromSourceLow);        // Mem[scratch] <- SP[7:0]
    push_microop(MicroOp::INC_DEST_ADDR);                // scratch++

    // M-cycle 5
    push_microop(MicroOp::WriteMemFromSourceHigh);       // Mem[scratch] <- SP[15:8]
}


void CPU::queue_load_a_mem_r16(Register16* source_addr) {
    dest_byte_ptr = &registers.A;
    source_addr_ptr = source_addr;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);

    // HL+
    if (current_opcode == 0x2A) {
        push_microop(MicroOp::INC_DEST_ADDR);
    // HL-
    } else if (current_opcode == 0x3A) {
        push_microop(MicroOp::DEC_DEST_ADDR);
    }
    return;
}


void CPU::queue_load_r8_r16(Byte* dest_byte, Register16* source_addr) {
    dest_byte_ptr = dest_byte;
    source_addr_ptr = source_addr;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);
}


void CPU::queue_load_r16_r8(Register16* dest_addr, Byte* source_byte) {
    dest_addr_ptr = dest_addr;
    source_byte_ptr = source_byte;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::WriteMemFromSourceByte);
}


void CPU::queue_pop_r16(Register16* dest_reg) {
    dest_addr_ptr = dest_reg;
    source_addr_ptr = &registers.SP;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // PC[7:0] <- Mem[SP]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // SP++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // PC[15:8] <- Mem[SP]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // SP++
}


void CPU::queue_push_r16(Register16* source_reg) {
    dest_addr_ptr = &registers.SP;
    source_addr_ptr = source_reg;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::InternalDelay);                // m-cycle 2

    // M-cycle 3
    push_microop(MicroOp::DEC_DEST_ADDR);                // SP--
    push_microop(MicroOp::WriteMemFromSourceHigh);       // Mem[SP] <- r16[15:8]

    // M-cycle 4
    push_microop(MicroOp::DEC_DEST_ADDR);                // SP--
    push_microop(MicroOp::WriteMemFromSourceLow);        // Mem[SP] <- r16[7:0]
}


void CPU::queue_load_imm16_a() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;
    source_byte_ptr = &registers.A;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // scratch[7:0] <- PC
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // scratch[15:8] <- PC
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 4
    push_microop(MicroOp::WriteMemFromSourceByte);      // Mem[scratch] <- A
}


void CPU::queue_load_a_imm16() {
    dest_addr_ptr = &scratch_register;
    dest_byte_ptr = &registers.A;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // scratch[7:0] <- PC
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // scratch[15:8] <- PC
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++
    push_microop(MicroOp::PointSourceToScratch);         // source_addr = scratch

    // M-cycle 4
    push_microop(MicroOp::ReadMemToDestByte);            // A <- Mem[scratch]
}


void CPU::queue_load_mem_hl_imm8() {
    dest_addr_ptr = &registers.HL;
    dest_byte_ptr = &scratch_register.low;
    source_addr_ptr = &registers.PC;
    source_byte_ptr = &scratch_register.low;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);            // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::WriteMemFromSourceByte);       // Mem[HL] <- scratch[7:0]
}


void CPU::queue_load_high_a_imm8() {
    dest_byte_ptr = &registers.A;
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    scratch_register.high = 0xFF;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // scratch = 0xFF00 | Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++
    push_microop(MicroOp::PointSourceToScratch);         // source_addr = scratch

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestByte);            // A <- Mem[scratch]
}


void CPU::queue_load_high_imm8_a() {
    dest_addr_ptr = &scratch_register;
    source_byte_ptr = &registers.A;
    source_addr_ptr = &registers.PC;

    scratch_register.high = 0xFF;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // scratch = 0xFF00 | Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::CopySourceToDestAddr);         // Mem[scratch] <- A
}


void CPU::queue_load_high_a_c() {
    dest_byte_ptr = &registers.A;
    source_addr_ptr = &scratch_register;

    scratch_register.high = 0xFF;                       // scratch = 0xFF00
    scratch_register.low = registers.C;                 // scratch = 0xFF00 + C

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);           // A <- Mem[scratch]
}


void CPU::queue_load_high_c_a() {
    dest_addr_ptr = &scratch_register;
    source_byte_ptr = &registers.A;

    scratch_register.high = 0xFF;                       // scratch = 0xFF00
    scratch_register.low = registers.C;                 // scratch = 0xFF00 + C

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::WriteMemFromSourceByte);      // Mem[scratch] <- A
}


void CPU::queue_prefix_cb() {
    dest_byte_ptr = &current_opcode;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);           // current_opcode <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);             // PC++
    push_microop(MicroOp::EXEC_DECODE_CB);              // Queue CB Op
}


void CPU::queue_interrupt(const Byte interrupt_vector) {
    dest_addr_ptr = &registers.SP;
    source_addr_ptr = &registers.PC;
    scratch_register.high = 0x00;
    scratch_register.low = interrupt_vector;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::InternalDelay);
    push_microop(MicroOp::DEC_DEST_ADDR);                // SP--

    // M-cycle 3
    push_microop(MicroOp::WriteMemFromSourceHigh);       // Mem[SP] <- PC[15:8]
    push_microop(MicroOp::DEC_DEST_ADDR);                // SP--

    // M-cycle 4
    push_microop(MicroOp::WriteMemFromSourceLow);        // Mem[SP] <- PC[7:0]

    // M-cycle 5
    push_microop(MicroOp::InternalDelay);
    push_microop(MicroOp::EXEC_JP);                      // PC <- scratch
}


void CPU::queue_jr() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;
    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);  // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++
    push_microop(MicroOp::EXEC_JR);                           // PC += (int16_t)scratch

    // M-cycle 3
    push_microop(MicroOp::InternalDelay);
}


void CPU::queue_jr_cond() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++
    push_microop(MicroOp::CheckCond);                    // If COND:
    push_microop(MicroOp::EXEC_JR);                      // PC += (int16_t)scratch

    // M-cycle 3
    push_microop(MicroOp::InternalDelay);
}


void CPU::queue_ret() {
    dest_addr_ptr = &registers.PC;
    source_addr_ptr = &registers.SP;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // PC[7:0] <- Mem[SP]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // SP++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // PC[15:8] <- Mem[SP]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // SP++

    // M-cycle 4
    push_microop(MicroOp::InternalDelay);
}


void CPU::queue_reti() {
    dest_addr_ptr = &registers.PC;
    source_addr_ptr = &registers.SP;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // PC[7:0] <- Mem[SP]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // SP++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // PC[15:8] <- Mem[SP]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // SP++

    // M-cycle 4
    push_microop(MicroOp::InternalDelay);
    push_microop(MicroOp::EXEC_EI_Immediate);            // IME = 1
}


void CPU::queue_ret_cond() {
    dest_addr_ptr = &registers.PC;
    source_addr_ptr = &registers.SP;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::InternalDelay);
    push_microop(MicroOp::CheckCond);                    // If COND:

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestLow);             // PC[7:0] <- Mem[SP]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // SP++

    // M-cycle 4
    push_microop(MicroOp::ReadMemToDestHigh);            // PC[15:8] <- Mem[SP]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // SP++

    // M-cycle 5
    push_microop(MicroOp::InternalDelay);
}


void CPU::queue_jp() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // scratch[15:8] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 4
    push_microop(MicroOp::InternalDelay);
    push_microop(MicroOp::EXEC_JP);                      // PC <- scratch
}


void CPU::queue_jp_hl() {
    dest_addr_ptr = &registers.PC;
    source_addr_ptr = &registers.HL;

    // M-cycle 1
    push_microop(MicroOp::CopySourceToDestAddr);          // PC <- HL
}


void CPU::queue_jp_cond() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // scratch[15:8] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++
    push_microop(MicroOp::CheckCond);                    // If COND:

    // M-cycle 4
    push_microop(MicroOp::InternalDelay);
    push_microop(MicroOp::EXEC_JP);                      // PC <- scratch
}


void CPU::queue_call() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // scratch[15:8] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 4
    push_microop(MicroOp::InternalDelay);
    push_microop(MicroOp::PointDestToSP);                // dest_addr = SP
    push_microop(MicroOp::DEC_DEST_ADDR);                // SP--

    // M-cycle 5
    push_microop(MicroOp::WriteMemFromSourceHigh);       // Mem[SP] <- PC[15:8]
    push_microop(MicroOp::DEC_DEST_ADDR);                // SP--

    // M-cycle 6
    push_microop(MicroOp::WriteMemFromSourceLow);        // Mem[SP] <- PC[7:0]
    push_microop(MicroOp::EXEC_JP);                      // PC <- scratch
}


void CPU::queue_call_cond() {
    dest_addr_ptr = &scratch_register;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestLow);             // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestHigh);            // scratch[15:8] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++
    push_microop(MicroOp::CheckCond);                    // If COND:

    // M-cycle 4
    push_microop(MicroOp::InternalDelay);                
    push_microop(MicroOp::PointDestToSP);                // dest_addr = SP
    push_microop(MicroOp::DEC_DEST_ADDR);                // SP--

    // M-cycle 5
    push_microop(MicroOp::WriteMemFromSourceHigh);       // Mem[SP] <- PC[15:8]
    push_microop(MicroOp::DEC_DEST_ADDR);                // SP--

    // M-cycle 6
    push_microop(MicroOp::WriteMemFromSourceLow);        // Mem[SP] <- PC[7:0]
    push_microop(MicroOp::EXEC_JP);                      // PC <- scratch
}


void CPU::queue_rst() {
    dest_addr_ptr = &registers.SP;
    source_addr_ptr = &registers.PC;

    // calculate jump address from current_opcode
    // https://gbdev.io/pandocs/CPU_Instruction_Set.html#block-3
    scratch_register.word = (Address)(current_opcode & 0x38);

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::InternalDelay);
    push_microop(MicroOp::DEC_DEST_ADDR);                // SP--

    // M-cycle 3
    push_microop(MicroOp::WriteMemFromSourceHigh);       // Mem[SP] <- PC[15:8]
    push_microop(MicroOp::DEC_DEST_ADDR);                // SP--

    // M-cycle 4
    push_microop(MicroOp::WriteMemFromSourceLow);        // Mem[SP] <- PC[7:0]
    push_microop(MicroOp::EXEC_JP);                      // PC <- scratch
}


void CPU::queue_load_r8_r8(Byte* dest_byte, Byte* source_byte) {
    dest_byte_ptr = dest_byte;
    source_byte_ptr = source_byte;

    // M-cycle 1
    push_microop(MicroOp::CopySourceToDestByte);         // dest <- source
}


void CPU::queue_load_sp_hl() {
    source_addr_ptr = &registers.HL;
    dest_addr_ptr = &registers.SP;

    // M-cycle 1
    push_microop(MicroOp::CopySourceToDestAddr);         // SP <- HL

    // M-cycle 2
    push_microop(MicroOp::InternalDelay);
}


void CPU::queue_add_sp() {
    dest_byte_ptr = &scratch_register.low;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);            // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::InternalDelay);
    push_microop(MicroOp::PointDestToSP);                // dest_addr = SP

    // M-cycle 4
    push_microop(MicroOp::InternalDelay);
    push_microop(MicroOp::EXEC_ADD_SP);                  // SP += (int16_t)scratch[7:0]
}


void CPU::queue_load_hl_sp_e8() {
    dest_byte_ptr = &scratch_register.low;
    source_addr_ptr = &registers.PC;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);            // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++

    // M-cycle 3
    push_microop(MicroOp::InternalDelay);                
    push_microop(MicroOp::PointDestToHL);                // dest_addr = HL
    push_microop(MicroOp::EXEC_ADD_SP);                  // HL = SP + (int16_t)scratch[7:0]
}


void CPU::queue_alu_op(Byte* source_byte) {
    source_byte_ptr = source_byte;

    // M-cycle 1
    push_microop(MicroOp::EXEC_ALU);
}


void CPU::queue_alu_op_mem_hl() {
    dest_byte_ptr = &scratch_register.low;
    source_addr_ptr = &registers.HL;
    source_byte_ptr = &scratch_register.low;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);               // scratch[7:0] <- Mem[HL]
    push_microop(MicroOp::EXEC_ALU);
}


void CPU::queue_alu_imm8() {
    source_addr_ptr = &registers.PC;
    source_byte_ptr = &scratch_register.low;
    dest_byte_ptr = &scratch_register.low;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);            // scratch[7:0] <- Mem[PC]
    push_microop(MicroOp::INC_SOURCE_ADDR);              // PC++
    push_microop(MicroOp::EXEC_ALU);                     // Do ALU op on imm8
}


void CPU::queue_inc_r16(Register16* dest_reg) {
    dest_addr_ptr = dest_reg;

    // M-cycle 1
    push_microop(MicroOp::INC_DEST_ADDR);

    // M-cycle 2
    push_microop(MicroOp::InternalDelay);
}


void CPU::queue_dec_r16(Register16* dest_reg) {
    dest_addr_ptr = dest_reg;

    // M-cycle 1
    push_microop(MicroOp::DEC_DEST_ADDR);

    // M-cycle 2
    push_microop(MicroOp::InternalDelay);
}


void CPU::queue_inc_r8(Byte* dest_byte) {
    dest_byte_ptr = dest_byte;

    // M-cycle 1
    push_microop(MicroOp::EXEC_INC_DEST_BYTE);
}


void CPU::queue_dec_r8(Byte* dest_byte) {
    dest_byte_ptr = dest_byte;

    // M-cycle 1
    push_microop(MicroOp::EXEC_DEC_DEST_BYTE);
}


void CPU::queue_add_hl_r16(Register16* source_reg) {
    source_addr_ptr = source_reg;

    // M-cycle 1
    push_microop(MicroOp::EXEC_ADD_HL);

    // M-cycle 2
    push_microop(MicroOp::InternalDelay);
}


void CPU::queue_inc_dec_mem_hl() {
    dest_addr_ptr = &registers.HL;
    dest_byte_ptr = &scratch_register.low;
    source_addr_ptr = &registers.HL;
    source_byte_ptr = &scratch_register.low;

    // M-cycle 1
    // ---------

    // M-cycle 2
    push_microop(MicroOp::ReadMemToDestByte);                 // scratch[7:0] <- Mem[HL]

    if (current_opcode == 0x34) {
        push_microop(MicroOp::EXEC_INC_DEST_BYTE);            // scratch[7:0]++
    } else {
        push_microop(MicroOp::EXEC_DEC_DEST_BYTE);            // scratch[7:0]--
    }

    // M-cycle 3
    push_microop(MicroOp::WriteMemFromSourceByte);            // Mem[HL] <- scratch[7:0]
}


void CPU::queue_cb_op(Byte* dest_byte) {
    dest_byte_ptr = dest_byte;

    // M-cycle 2
    push_microop(MicroOp::EXEC_CB_OP);
}


void CPU::queue_cb_op_mem_hl() {
    dest_byte_ptr = &scratch_register.low;
    dest_addr_ptr = &registers.HL;
    source_byte_ptr = &scratch_register.low;
    source_addr_ptr = &registers.HL;

    // M-cycle 2
    // ---------

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestByte);            // scratch[7:0] <- Mem[HL]
    push_microop(MicroOp::EXEC_CB_OP);                   // DO CB op on scratch[7:0]

    // M-cycle 4
    push_microop(MicroOp::WriteMemFromSourceByte);       // Mem[HL] <- scratch[7:0]
}


// NOTE: BIT b, (HL) only takes 3 m-cycles as it doesn't write back
void CPU::queue_cb_bit_mem_hl() {
    dest_byte_ptr = &scratch_register.low;
    source_addr_ptr = &registers.HL;

    // M-cycle 2
    // ---------

    // M-cycle 3
    push_microop(MicroOp::ReadMemToDestByte);      // scratch[7:0] <- Mem[HL]
    push_microop(MicroOp::EXEC_CB_OP);                   // DO CB op on scratch[7:0]
}