/* ************************************************************************* */
/* LOG.C - Logger                                                            */
/* All Logging routines are kept in this module for easy access                 */
/* ************************************************************************* */
#include "psx.h"


API int ABI LOG_Save(char *path) {
    return 1;
}


API int ABI LOG_Enable(int enable) {
    return 1;
}


API int ABI LOG_Filter(char *module, int show) {
    return 1;
}


API int ABI LOG_Info(char *module, char *message) {
    return 1;
}


API int ABI LOG_Pass(char *module, char *message) {
    return 1;
}


API int ABI LOG_Warn(char *module, char *message) {
    return 1;
}


API int ABI LOG_Fail(char *module, char *message) {
    return 1;
}
