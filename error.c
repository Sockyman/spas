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
    if (trace)
    {
        printf("%s:%d: ", trace->filename, trace->line);
    }
    va_list valist;
    va_start(valist, message);
    printf("\033[0;31merror:\033[0m ");
    vprintf(message, valist);
    printf("\n");
    ++global_errors;
}

