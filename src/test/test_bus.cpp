#include <psx.h>
#include <stdio.h>
#include <windows.h>

static int TLB[] = {
    0x00000000, 0x001FFFFF,
    0x1F000000, 0x1F7FFFFF,
    0x1F800000, 0x1F8003FF,
    0x1F801000, 0x1F801FFF,
    0x1F802000, 0x1F803FFF,
    0x1FA00000, 0x1FBFFFFF,
    0x1FC00000, 0x1FC7FFFF,
    0x80000000, 0x801FFFFF,
    0x9F000000, 0x9F7FFFFF,
    0x9F800000, 0x9F8003FF,
    0x9F801000, 0x9F801FFF,
    0x9F802000, 0x9F803FFF,
    0x9FA00000, 0x9FBFFFFF,
    0x9FC00000, 0x9FC7FFFF,
    0xA0000000, 0xA01FFFFF,
    0xBF000000, 0xBF7FFFFF,
    0xBF801000, 0xBF801FFF,
    0xBF802000, 0xBF803FFF,
    0xBFA00000, 0xBFBFFFFF,
    0xBFC00000, 0xBFC7FFFF,
    0xFFFE0000, 0xFFFE01FF,
    0xFFFFFFFE
};

int BUS_01_TranslateVirt2Phys(void) {

    uint64_t n = 0;
    uint64_t t0 = __rdtsc();

    // add weighting to favour RAM DCACHE and BIOS access
    for (int i = 0; i < 10000; i++) {
        for (u32 virt = 0x80000000; virt <= 0x801FFFFF; virt++) {
            n++;
            if (0 == BUS_TranslateVirt2Phys(virt)) {
                BUS_TranslateVirt2Phys(virt);
                printf("[FAIL] %s virt = %08X\n", __FUNCTION__, virt);
                return 0;
            }
        }
    }
    for (int i = 0; i < 1000; i++) {
        for (u32 virt = 0xBFC00000; virt <= 0xBFC7FFFF; virt++) {
            n++;
            if (0 == BUS_TranslateVirt2Phys(virt)) {
                BUS_TranslateVirt2Phys(virt);
                printf("[FAIL] %s virt = %08X\n", __FUNCTION__, virt);
                return 0;
            }
        }
    }
    for (int i = 0; i < 10000; i++) {
        for (u32 virt = 0x1F800000; virt <= 0x1F8003FF; virt++) {
            n++;
            if (0 == BUS_TranslateVirt2Phys(virt)) {
                BUS_TranslateVirt2Phys(virt);
                printf("[FAIL] %s virt = %08X\n", __FUNCTION__, virt);
                return 0;
            }
        }
    }

    for (int i = 0; i < 21; i++) {
        u32 lo = TLB[i*2 + 0];
        u32 hi = TLB[i*2 + 1];
        u32 next = TLB[i*2 + 2];

        for (u32 virt = lo; virt <= hi; virt++) {
            n++;
            if (0 == BUS_TranslateVirt2Phys(virt)) {
                BUS_TranslateVirt2Phys(virt);
                printf("[FAIL] %s virt = %08X\n", __FUNCTION__, virt);
                return 0;
            }
        }
        for (u32 virt = hi+1; virt < next; virt++) {
            n++;
            if (0 != BUS_TranslateVirt2Phys(virt)) {
                BUS_TranslateVirt2Phys(virt);
                printf("[FAIL] %s virt = %08X\n", __FUNCTION__, virt);
                return 0;
            }
        }
    }

    uint64_t t1 = __rdtsc();
    LARGE_INTEGER hz;
    QueryPerformanceFrequency(&hz);

    uint64_t t = t1 - t0;
    uint64_t f = hz.QuadPart;

    printf("[PASS] %s CPU %lld Hz %lld Lookups in %lld ticks\n", __FUNCTION__, f, n, t);
    return 1;
}
