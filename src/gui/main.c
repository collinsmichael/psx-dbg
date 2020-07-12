#include <windows.h>
#include "psx.h"


int main(void) {
    CPU_Reset();

    u32 dat = BUS_Get32(0x00000000);
    return 0;
}


void start(void) {
    ExitProcess(main());
}
