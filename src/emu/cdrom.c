/* ************************************************************************* */
/* CDROM.C - CD Controller                                                   */
/* All CDROM IO is kept in this module for easy access                       */
/* ************************************************************************* */
#include "psx.h"


u8 CDROM[0x4DEAF*2352];


API int ABI CDROM_InsertDisc(char *path) {
    return 1;
}


API int ABI CDROM_ReadSector(u32 lba, u8 ptr) {
    return 1;
}


API int ABI CDROM_SeekSector(u32 lba) {
    return 1;
}
