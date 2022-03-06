#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdbool.h>
#include "node.h"

struct Path
{
    char *path;
    struct Path *next;
};

Node *parse_file(char *filename, Node **final_node);
void resolve_filename(char *string, const char *from, const char *filename);
Datalist *read_file_to_datalist(FILE *file);

void initialize_path_cache(void);
void free_path_cache(void);
char *cache_get_path(const char *path);
char *cache_add_path(const char *path);


#endif

