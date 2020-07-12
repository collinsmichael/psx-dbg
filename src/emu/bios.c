/* ************************************************************************* */
/* BIOS.C - BIOS Kernel                                                      */
/* All BIOS routines are kept in this module for easy access                 */
/* ************************************************************************* */
#include "psx.h"


u8 BIOS[512*KB]; // temp buffer gets copied to RAM


API int ABI BIOS_InsertRom(char *path) {
    int fd = OS_OpenFile(path);
    if (!fd) {
        return 0;
    }

    if (!OS_ReadFile(fd, BIOS, sizeof(BIOS))) {
        return 0;
    }

    if (!BUS_SetCpu(0xBFC00000, BIOS, 512*KB)) {
        return 0;
    }
    return 1;
}
