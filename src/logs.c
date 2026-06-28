#include "headers/logs.h"
#include <stdarg.h>
#include <stdio.h>
void debug_log(const char* format, ...){
    va_list arg;
    va_start(arg, format);
    #ifdef DEBUG
    vprintf(format, arg);
    #endif
    va_end(arg);
}
