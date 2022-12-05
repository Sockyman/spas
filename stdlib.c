#include "stdlib.h"

char *stdlib_path = "std/";

void set_stdlib_path(char *path)
{
    stdlib_path = path;
}

char *get_stdlib_path()
{
    return stdlib_path;
}

