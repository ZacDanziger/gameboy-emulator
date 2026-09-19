#ifndef CPU_STATE_H
#define CPU_STATE_H

enum class CPUState : uint8_t {
    /**
     * 1 m-cycle states
     */
    InternalDelay = 0,

    Mem_Source_to_Dest_Byte,          // dest_byte <- Mem[source_addr]
    Mem_Source_to_Dest_Addr_Low,      // dest_addr[7:0] <- Mem[source_addr]
    Mem_Source_to_Dest_Addr_High,     // dest_addr[15:8] <- Mem[source_addr]

    Source_Byte_to_Mem_Dest,          // Mem[dest_addr] <- source_byte
    Source_Addr_Low_to_Mem_Dest,      // Mem[dest_addr] <- source_addr[7:0]
    Source_Addr_High_to_Mem_Dest,     // Mem[dest_addr] <- source_addr[15:8]

    /**
     * 0 m-cycle states
     */

    CheckCond,
    EI,             // ei_pending = true
    EI_Immediate,   // interrupts_enabled = true
    DI,             // interrupts_disable = true

    Make_SP_Source_Addr,
    Make_SP_Dest_Addr,
    Make_Scratch_Source_Addr,

    Source_Byte_to_Dest_Byte,        // dest_byte <- source_byte
    Source_Addr_to_Dest_Addr,        // dest_addr <- source_addr

    INC_Source_Addr,
    DEC_Source_Addr,
    INC_Dest_Addr,
    DEC_Dest_Addr,
    
    INC_Dest_Byte,
    DEC_Dest_Byte,

    EXEC_ALU,   // 8-bit arithmetic

    ADD_HL,

    JR,
    JP
};


#endif      // CPU_STATE_H