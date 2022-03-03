#ifndef SPDRERROR_H
#define SPDRERROR_H

#include "trace.h"
#include "node.h"

typedef struct Error
{
    char message[128];
    Trace trace;
    Node *node;
} Error;

int error_count(void);
void print_error(Trace *trace, const char *message, ...);

#endif

