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
        fprintf(stderr, "%s:%d: ", trace->filename, trace->line);
    }
    va_list valist;
    va_start(valist, message);
    fprintf(stderr, "\033[0;31merror:\033[0m ");
    vfprintf(stderr, message, valist);
    fprintf(stderr, "\n");
    ++global_errors;
}

