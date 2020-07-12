/* ************************************************************************* */
/* BUS.C - Memory and Register Access                                        */
/* All state is kept in this module for easy access                          */
/* ************************************************************************* */
#include "psx.h"



u8 RAM[2*MB + 8*MB + 2*MB + 512*KB + 4*KB + 8*KB + 1*KB + KB/2];
u8 VRAM[1024*KB];
u8 SRAM[512*KB];
u32 REG_CPU[32+4];
u32 REG_GTE[32];
u32 REG_COP0[32];



API int ABI BUS_Reset(void) {
    REG_CPU[R0]    = 0x00000000;
    REG_CPU[R1]    = 0x00000000;
    REG_CPU[R2]    = 0x00000000;
    REG_CPU[R3]    = 0x00000000;
    REG_CPU[R4]    = 0x00000000;
    REG_CPU[R5]    = 0x00000000;
    REG_CPU[R6]    = 0x00000000;
    REG_CPU[R7]    = 0x00000000;
    REG_CPU[R8]    = 0x00000000;
    REG_CPU[R9]    = 0x00000000;
    REG_CPU[R10]   = 0x00000000;
    REG_CPU[R11]   = 0x00000000;
    REG_CPU[R12]   = 0x00000000;
    REG_CPU[R13]   = 0x00000000;
    REG_CPU[R14]   = 0x00000000;
    REG_CPU[R15]   = 0x00000000;
    REG_CPU[R16]   = 0x00000000;
    REG_CPU[R17]   = 0x00000000;
    REG_CPU[R18]   = 0x00000000;
    REG_CPU[R19]   = 0x00000000;
    REG_CPU[R20]   = 0x00000000;
    REG_CPU[R21]   = 0x00000000;
    REG_CPU[R22]   = 0x00000000;
    REG_CPU[R23]   = 0x00000000;
    REG_CPU[R24]   = 0x00000000;
    REG_CPU[R25]   = 0x00000000;
    REG_CPU[R26]   = 0x00000000;
    REG_CPU[R27]   = 0x00000000;
    REG_CPU[R28]   = 0x00000000;
    REG_CPU[R29]   = 0x00000000;
    REG_CPU[R30]   = 0x00000000;
    REG_CPU[R31]   = 0x00000000;
    REG_CPU[LO]    = 0x00000000;
    REG_CPU[HI]    = 0x00000000;
    REG_CPU[PC]    = 0xBFC00000;
    REG_CPU[DELAY] = 0xBFC00004;
    return 1;
}


API u8 *ABI BUS_TranslateVirt2Phys(u32 addr) {
    // Target Speed is 33.8688 MHz
    // 25801015294 Lookups in 483940803120 ticks    binary search method 18.75 ticks per lookup (128MHz)
    // 25801015294 Lookups in 291489630368 ticks    linear search method 11.3 ticks per lookup (212MHz)

    #if 0 // binary search method (fast regardless)
    int lo = 0;
    int hi = 21;
    while (hi > lo) {
        static int TLB[] = { // translate lookaside buffer
        //  LOWER       UPPER       MASK        OFFSET
            0x00000000, 0x001FFFFF, 0x001FFFFF, BUS_RAM,
            0x1F000000, 0x1F7FFFFF, 0x007FFFFF, BUS_EX1,
            0x1F800000, 0x1F8003FF, 0x000003FF, BUS_SCRATCH,
            0x1F801000, 0x1F801FFF, 0x00001FFF, BUS_IO,
            0x1F802000, 0x1F803FFF, 0x00001FFF, BUS_EX2,
            0x1FA00000, 0x1FBFFFFF, 0x001FFFFF, BUS_EX3,
            0x1FC00000, 0x1FC7FFFF, 0x0007FFFF, BUS_BIOS,
            0x80000000, 0x801FFFFF, 0x001FFFFF, BUS_RAM,
            0x9F000000, 0x9F7FFFFF, 0x007FFFFF, BUS_EX1,
            0x9F800000, 0x9F8003FF, 0x000003FF, BUS_SCRATCH,
            0x9F801000, 0x9F801FFF, 0x00000FFF, BUS_IO,
            0x9F802000, 0x9F803FFF, 0x00001FFF, BUS_EX2,
            0x9FA00000, 0x9FBFFFFF, 0x001FFFFF, BUS_EX3,
            0x9FC00000, 0x9FC7FFFF, 0x0007FFFF, BUS_BIOS,
            0xA0000000, 0xA01FFFFF, 0x001FFFFF, BUS_RAM,
            0xBF000000, 0xBF7FFFFF, 0x007FFFFF, BUS_EX1,
            0xBF801000, 0xBF801FFF, 0x00000FFF, BUS_IO,
            0xBF802000, 0xBF803FFF, 0x00001FFF, BUS_EX2,
            0xBFA00000, 0xBFBFFFFF, 0x001FFFFF, BUS_EX3,
            0xBFC00000, 0xBFC7FFFF, 0x0007FFFF, BUS_BIOS,
            0xFFFE0000, 0xFFFE01FF, 0x000001FF, BUS_KSEG2,
        };

        int mid = (lo + hi) / 2;
        u32 key  = TLB[mid*4 + 0];
        u32 mask = TLB[mid*4 + 2];

        if (addr > key + mask) {
            lo = mid+1;
        } else if (addr < key) {
            hi = mid;
        } else {
            u32 end = TLB[mid*4 + 1];
            if (addr > end) {
                break;
            }
            u32 seg = TLB[mid*4 + 3];
            u32 phys = (addr & mask) + seg;
            return &RAM[phys];
        }
    }
    #endif

    #if 1 // linear search method (allows for prioritizing high frequency access)
    if ((addr >= 0x80000000) && (addr <= 0x801FFFFF)) return &RAM[addr - 0x80000000 + BUS_RAM];
    if ((addr >= 0x1F800000) && (addr <= 0x1F8003FF)) return &RAM[addr - 0x1F800000 + BUS_SCRATCH];
    if ((addr >= 0xBFC00000) && (addr <= 0xBFC7FFFF)) return &RAM[addr - 0xBFC00000 + BUS_BIOS];
    if ((addr >= 0x00000000) && (addr <= 0x001FFFFF)) return &RAM[addr - 0x00000000 + BUS_RAM];
    if ((addr >= 0xA0000000) && (addr <= 0xA01FFFFF)) return &RAM[addr - 0xA0000000 + BUS_RAM];
    if ((addr >= 0x9F800000) && (addr <= 0x9F8003FF)) return &RAM[addr - 0x9F800000 + BUS_SCRATCH];
    if ((addr >= 0x9FC00000) && (addr <= 0x9FC7FFFF)) return &RAM[addr - 0x9FC00000 + BUS_BIOS];
    if ((addr >= 0x1FC00000) && (addr <= 0x1FC7FFFF)) return &RAM[addr - 0x1FC00000 + BUS_BIOS];

    if ((addr >= 0x1F801000) && (addr <= 0x1F801FFF)) return &RAM[addr - 0x1F801000 + BUS_IO];
    if ((addr >= 0x9F801000) && (addr <= 0x9F801FFF)) return &RAM[addr - 0x9F801000 + BUS_IO];
    if ((addr >= 0xBF801000) && (addr <= 0xBF801FFF)) return &RAM[addr - 0xBF801000 + BUS_IO];
    if ((addr >= 0xBF802000) && (addr <= 0xBF803FFF)) return &RAM[addr - 0xBF802000 + BUS_EX2];
    if ((addr >= 0x9F802000) && (addr <= 0x9F803FFF)) return &RAM[addr - 0x9F802000 + BUS_EX2];
    if ((addr >= 0x1F802000) && (addr <= 0x1F803FFF)) return &RAM[addr - 0x1F802000 + BUS_EX2];
    if ((addr >= 0xFFFE0000) && (addr <= 0xFFFE01FF)) return &RAM[addr - 0xFFFE0000 + BUS_KSEG2];
    if ((addr >= 0x1F000000) && (addr <= 0x1F7FFFFF)) return &RAM[addr - 0x1F000000 + BUS_EX1];
    if ((addr >= 0x9F000000) && (addr <= 0x9F7FFFFF)) return &RAM[addr - 0x9F000000 + BUS_EX1];
    if ((addr >= 0xBF000000) && (addr <= 0xBF7FFFFF)) return &RAM[addr - 0xBF000000 + BUS_EX1];
    if ((addr >= 0x1FA00000) && (addr <= 0x1FBFFFFF)) return &RAM[addr - 0x1FA00000 + BUS_EX3];
    if ((addr >= 0x9FA00000) && (addr <= 0x9FBFFFFF)) return &RAM[addr - 0x9FA00000 + BUS_EX3];
    if ((addr >= 0xBFA00000) && (addr <= 0xBFBFFFFF)) return &RAM[addr - 0xBFA00000 + BUS_EX3];
    #endif

    //LOG_Fail("BUS", "Failed to translate address %d", addr);
    return 0;
}


API int ABI BUS_Get32(u32 ptr) {
    u32 *ram = (u32*)BUS_TranslateVirt2Phys(ptr);
    if (ram == 0) {
        return 0xFFFFFFFF;
    }
    return *ram;
}


API int ABI BUS_Get16(u32 ptr) {
    u16 *ram = (u16*)BUS_TranslateVirt2Phys(ptr);
    if (ram == 0) {
        return 0xFFFFFFFF;
    }
    return *ram;
}


API int ABI BUS_Get8(u32 ptr) {
    u8 *ram = (u8*)BUS_TranslateVirt2Phys(ptr);
    if (ram == 0) {
        return 0xFFFFFFFF;
    }
    return *ram;
}


API int ABI BUS_Set32(u32 ptr, u32 val) {
    u32 *ram = (u32*)BUS_TranslateVirt2Phys(ptr);
    if (ram == 0) {
        return 0;
    }
    ram[0] = val;
    return 1;
}


API int ABI BUS_Set16(u32 ptr, u16 val) {
    u16 *ram = (u16*)BUS_TranslateVirt2Phys(ptr);
    if (ram == 0) {
        return 0;
    }
    ram[0] = val;
    return 1;
}


API int ABI BUS_Set8(u32 ptr, u8 val) {
    u8 *ram = (u8*)BUS_TranslateVirt2Phys(ptr);
    if (ram == 0) {
        return 0;
    }
    ram[0] = val;
    return 1;
}


API int ABI BUS_GetCpu(u32 ptr, u8 *buf, u32 len) {
    u8 *ram = (u8*)BUS_TranslateVirt2Phys(ptr);
    u8 *end = (u8*)BUS_TranslateVirt2Phys(ptr+len-1);

    if ((ram == 0) || (end == 0)) {
        return 0;
    }

    for (u32 pos = 0; pos < len; pos++) {
        buf[pos] = ram[pos];
    }
    return 1;
}


API int ABI BUS_SetCpu(u32 ptr, u8 *buf, u32 len) {
    u8 *ram = (u8*)BUS_TranslateVirt2Phys(ptr);
    u8 *end = (u8*)BUS_TranslateVirt2Phys(ptr+len-1);

    if ((ram == 0) || (end == 0)) {
        return 0;
    }

    for (u32 pos = 0; pos < len; pos++) {
        ram[pos] = buf[pos];
    }
    return 1;
}


API int ABI BUS_GetGpu(u32 ptr, u8 *buf, u32 len) {
    if (ptr + len >= sizeof(VRAM)) {
        return 0;
    }

    for (int pos = len-1; pos >= 0; pos--) {
        buf[pos] = VRAM[ptr + pos];
    }
    return 1;
}


API int ABI BUS_SetGpu(u32 ptr, u8 *buf, u32 len) {
    if (ptr + len >= sizeof(VRAM)) {
        return 0;
    }

    for (int pos = len-1; pos >= 0; pos--) {
        VRAM[ptr + pos] = buf[pos];
    }
    return 1;
}


API int ABI BUS_GetSpu(u32 ptr, u8 *buf, u32 len) {
    if (ptr + len >= sizeof(SRAM)) {
        return 0;
    }

    for (int pos = len-1; pos >= 0; pos--) {
        buf[pos] = SRAM[ptr + pos];
    }
    return 1;
}


API int ABI BUS_SetSpu(u32 ptr, u8 *buf, u32 len) {
    if (ptr + len >= sizeof(SRAM)) {
        return 0;
    }

    for (int pos = len-1; pos >= 0; pos--) {
        SRAM[ptr + pos] = buf[pos];
    }
    return 1;
}


API u32 ABI BUS_GetReg(u32 reg) {
    if (reg > 32) {
        return 0;
    }
    return REG_CPU[reg];
}


API int ABI BUS_SetReg(u32 reg, u32 val) {
    if (reg > 32) {
        return 0;
    }
    REG_CPU[reg] = val;
    return 1;
}


API int ABI BUS_LoadState(u8 *buf, u32 len) {
    if (len < sizeof(RAM)) {
        return 0;
    }

    BUS_SetCpu(0x80000000, buf, 2048*KB); buf += 2048*KB;
    BUS_SetCpu(0x9F800000, buf, 1*KB);    buf += 1*KB;
    BUS_SetGpu(0x00000000, buf, 1024*KB); buf += 1024*KB;
    BUS_SetSpu(0x00000000, buf, 512*KB);  buf += 512*KB;

    u32 *reg = (u32*)buf;
    for (int k = 31; k >= 0; k--) {
        BUS_SetReg(k, reg[k]);
    }
    return 1;
}


API int ABI BUS_DumpState(u8 *buf, u32 len) {
    int size = 2048*KB + 1*KB + 1024*KB + 512*KB + 32*sizeof(u32);
    if (len < sizeof(RAM)) {
        return 0;
    }

    BUS_GetCpu(0x80000000, buf, 2048*KB); buf += 2048*KB;
    BUS_GetCpu(0x9F800000, buf, 1*KB);    buf += 1*KB;
    BUS_GetGpu(0x00000000, buf, 1024*KB); buf += 1024*KB;
    BUS_GetSpu(0x00000000, buf, 512*KB);  buf += 512*KB;

    u32 *reg = (u32*)buf;
    for (int k = 31; k >= 0; k--) {
        reg[k] = BUS_GetReg(k);
    }
    return 1;
}
