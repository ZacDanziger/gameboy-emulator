#ifndef CPU_REGISTERS_H
#define CPU_REGISTERS_H

#include "../types.h"

union Register16 {
    Word word;
    struct {
        Byte low;
        Byte high; 
    };

    Word operator++(int) {
        return word++;
    }

    Word operator--(int) {
        return word--;
    }
};

struct Registers {
    union {
        Register16 AF = {0x1180};
        struct {Byte F; Byte A;};
    };
    union {
        Register16 BC = {0x0000};
        struct {Byte C; Byte B;};
    };
    union {
        Register16 DE = {0xFF56};
        struct {Byte E; Byte D;};
    };
    union {
        Register16 HL = {0x000D};
        struct {Byte L; Byte H;};
    };
    
    Register16 SP = {0xFFFE};
    Register16 PC = {0x0100};


    void reset() {
        AF = {0x1180};
        BC = {0x0000};
        DE = {0xFF56};
        HL = {0x000D};
        SP = {0xFFFE};
        PC = {0x0100};
    }
};

#endif      // CPU_REGISTERS_H