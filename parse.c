#include <string.h>
#include <stdlib.h>
#include "parse.h"
#include "parser.tab.h"
#include "error.h"
#include "syntax_tree.h"

struct Path *path_cache;

Node *parse_file(char *filename, Node **final_node)
{
    yyin = fopen(filename, "r");
    if (!yyin)
    {
        print_error(NULL, "unable to open file \"%s\".", filename);
        return NULL;
    }

    Trace trace;
    trace.filename = filename;
    trace.line = 1;
    trace.column = 1;
    global_trace = &trace;

    /* Parse input file */

    initialize_node_tree();
    yyparse();

    Node *tree = get_tree();
    fclose(yyin);

    Node *current = tree;
    while (current != NULL)
    {
        //printf("%d ", current->type);
        if (current->type == NODE_INCLUDE)
        {
            char iname[256];
            resolve_filename(iname, filename, current->name);
            if (current->mode != INCLUDE_ONCE || !cache_get_path(iname))
            {
                char *name = cache_add_path(iname);
                Node *ifinal = NULL;
                Node *itree = parse_file(name, &ifinal);
                if (itree)
                {
                    Node *next = current->next;
                    current->next = itree;
                    if (next)
                    {
                        ifinal->next = next;
                    }
                }
            }
        }
        current = current->next;
    }

    Node *current_final = tree;
    if (current_final != NULL)
    {
        while (current_final->next != NULL)
        {
            current_final = current_final->next;
        }
    }
    *final_node = current_final;
    return tree;
}


void resolve_filename(char *string, const char *from, const char *filename)
{
    char stdlib_prefix[] = "std:";
    if (!strncmp(filename, stdlib_prefix, sizeof(stdlib_prefix) - 1))
    {
        sprintf(string, "%s/%s", "std", filename + 4);
    }
    else if (from && filename[0] != '/')
    {
        //sprintf(relative_filename, "%s/../%s", from, filename);
        //name = relative_filename;

        char *final_dir = strrchr(from, '/');
        if (final_dir)
        {
            strncpy(string, from, (final_dir + 1) - from);
            string[(final_dir + 1) - from] = 0;
            strcat(string, filename);
        }
        else
        {
            strcpy(string, filename);
        }
    }
    else
    {
        strcpy(string, filename);
    }
}

void initialize_path_cache(void)
{
    path_cache = NULL;
}

void free_path_cache(void)
{
    struct Path *current;
    while (current != NULL)
    {
        struct Path *next = current->next;
        free(current->path);
        free(current);
        current = next;
    }
}

char *cache_get_path(const char *path)
{
    struct Path *current = path_cache;
    while (current != NULL)
    {
        if (!strcmp(path, current->path))
        {
            return current->path;
        }
        current = current->next;
    }
    return NULL;
}

char *cache_add_path(const char *path)
{
    char *existing = cache_get_path(path);
    if (!existing)
    {
        struct Path *p = malloc(sizeof(struct Path));
        p->path = strdup(path);
        p->next = NULL;
        if (path_cache)
        {
            struct Path *current = path_cache;
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = p;
        }
        else
        {
            path_cache = p;
        }
        return p->path;
    }
    return existing;
}

