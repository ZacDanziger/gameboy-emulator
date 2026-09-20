#ifndef MICROOPS_H
#define MICROOPS_H

enum class MicroOp : uint8_t {
    /**
     * 1 m-cycle states
     */

    // Delay
    InternalDelay = 0,

    // Memory
    ReadMemToDestByte,           // dest_byte <- Mem[source_addr]
    ReadMemToDestLow,            // dest_addr[7:0] <- Mem[source_addr]
    ReadMemToDestHigh,           // dest_addr[15:8] <- Mem[source_addr]

    WriteMemFromSourceByte,      // Mem[dest_addr] <- source_byte
    WriteMemFromSourceLow,       // Mem[dest_addr] <- source_addr[7:0]
    WriteMemFromSourceHigh,      // Mem[dest_addr] <- source_addr[15:8]

    
    /**
     * 0 m-cycle states
     */

    // Pointers
    PointSourceToSP,
    PointDestToSP,
    PointSourceToScratch,
    PointDestToHL,

    CopySourceToDestByte,        // dest_byte <- source_byte
    CopySourceToDestAddr,        // dest_addr <- source_addr

    // Conditional
    CheckCond,

    // Structural Helpers
    INC_SOURCE_ADDR,
    DEC_SOURCE_ADDR,
    INC_DEST_ADDR,
    DEC_DEST_ADDR,

    // Executes
    EXEC_ALU,
    EXEC_INC_DEST_BYTE,
    EXEC_DEC_DEST_BYTE,
    EXEC_DECODE_CB,
    EXEC_CB_OP,
    EXEC_ADD_HL,
    EXEC_ADD_SP,
    EXEC_AF_OP,

    EXEC_JR,
    EXEC_JP,

    EXEC_EI,             // ei_pending = true
    EXEC_EI_Immediate,   // interrupts_enabled = true
    EXEC_DI,             // interrupts_disable = true

    EXEC_HALT,
    EXEC_STOP
};


#endif      // MICROOPS_H