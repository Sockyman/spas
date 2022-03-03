#include <stdio.h>
#include <stdarg.h>
#include "error.h"

int global_errors;

int error_count(void)
{
    return global_errors;
}

void print_error(Trace *trace, const char *message, ...)
{
    va_list valist;
    va_start(valist, message);
    printf("%s:%d: error: ", trace->filename, trace->line);
    vprintf(message, valist);
    printf("\n");
    ++global_errors;
}

