/* ************************************************************************* */
/* WIN32.C - Platform dependent code targetting windows                      */
/* All win32 platform specific is kept in this module for easy access        */
/* ************************************************************************* */
#ifdef WIN32

#include "psx.h"
#include <windows.h>


API int ABI OS_OpenFile(char *path) {
    void *hFile = CreateFileA(path,
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        return 0;
    }
    return (int)hFile;
}


API int ABI OS_CloseFile(int fd) {
    if (CloseHandle((HANDLE)fd) != TRUE) {
        return 0;
    }
    return 1;
}


API int ABI OS_SeekFile(int fd, int pos) {
    SetFilePointer((HANDLE)fd, pos, 0, FILE_BEGIN);
    return 1;
}


API int ABI OS_ReadFile(int fd, char *buf, int len) {
    DWORD count;
    if (ReadFile((HANDLE)fd, buf, len, &count, 0) != TRUE) {
        return 0;
    }
    return 1;
}


API int ABI OS_WriteFile(int fd, char *buf, int len) {
    DWORD count;
    if (WriteFile((HANDLE)fd, buf, len, &count, 0) != TRUE) {
        return 0;
    }
    return 1;
}


#endif WIN32
