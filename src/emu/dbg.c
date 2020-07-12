/* ************************************************************************* */
/* DBG.C - Debugger API                                                      */
/* All debug commands are kept in this module                                */
/* ************************************************************************* */
#include "psx.h"



API int ABI DBG_Halt(void) {
    return 1;
}


API int ABI DBG_Run(void) {
    return 1;
}


API int ABI DBG_StepFrame(void) {
    return 1;
}


API int ABI DBG_StepInto(void) {
    return 1;
}


API int ABI DBG_StepOver(void) {
    return 1;
}


API int ABI DBG_StepOut(void) {
    return 1;
}


API int ABI DBG_InsertBreakPoint(u32 ptr, u32 len, u32 rwx) {
    return 1;
}


API int ABI DBG_RemoveBreakPoint(u32 ptr, u32 len, u32 rwx) {
    return 1;
}
