/* ************************************************************************* */
/* CPU.C - Processor                                                         */
/* All state is kept in this module for easy access                          */
/* ************************************************************************* */
#include "psx.h"



API int ABI CPU_Reset(void) {
    BIOS_InsertRom("bios/scph1001.bin");
    BUS_Reset();
    return 1;
}


API int ABI CPU_Step(void) {
    u32 pc = BUS_GetReg(PC);
    u32 opcode = BUS_Get32(pc);

    return 1;
}
