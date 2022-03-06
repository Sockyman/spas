#ifndef TRACE_H
#define TRACE_H

typedef struct Trace
{
    int line;
    int column;
    char *filename;
} Trace;

extern Trace *global_trace;

#endif

