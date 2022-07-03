#include <stdlib.h>
#include <string.h>
#include "identifier.h"
#include "error.h"

extern Trace *global_trace;

Identifier *scope = NULL;

Identifier *get_identifier(char *str)
{
    Identifier *id = NULL;
    char *ptr = str;

    // Determine depth.
    Identifier *toadd = scope;
    while (*ptr == '.')
    {
        if (!toadd)
        {
            print_error(global_trace, "invalid identifier.");
            free_identifier(id);
            return NULL;
        }
        id = identifier_add_scope(id, toadd->name);
        toadd = toadd->next;
        ++ptr;
    }

    for (;;)
    {
        char *occur = strchr(ptr, '.');
        if (!occur)
        {
            break;
        }

        if (occur[1] == '.')
        {
            print_error(global_trace, "invalid identifier.");
            free_identifier(id);
            return NULL;
        }

        char buff[128];
        strncpy(buff, ptr, occur - ptr);
        buff[occur - ptr] = 0;
        id = identifier_add_scope(id, buff);
        ptr = occur + 1;
    }
    if (strlen(ptr) == 0)
    {
        print_error(global_trace, "invalid identifier.");
        free_identifier(id);
        return NULL;
    }
    id = identifier_add_scope(id, ptr);

    return id;
}

Identifier *identifier_add_scope(Identifier *id, char *str)
{
    Identifier *new_id = malloc(sizeof(Identifier));
    new_id->name = strdup(str);
    new_id->next = NULL;

    if (!id)
    {
        return new_id;
    }
    else
    {
        Identifier *current = id;
        while (current->next)
        {
            current = current->next;
        }
        current->next = new_id;
    }

    return id;
}

void free_identifier(Identifier *id)
{
    if (id->next)
    {
        free_identifier(id->next);
    }
    free(id->name);
    free(id);
}

void enter_scope(Identifier *id)
{
    scope = id;
}

bool compare_identifier(Identifier *id_0, Identifier *id_1)
{
    if (!strcmp(id_0->name, id_1->name))
    {
        if (id_0->next && id_1->next)
        {
            return compare_identifier(id_0->next, id_1->next);
        }
        else if (!(id_0->next || id_1->next))
        {
            return true;
        }
    }
    return false;
}

Identifier *clone_identifier(Identifier *to_clone)
{
    Identifier *id = get_identifier(to_clone->name);
    Identifier *current = to_clone->next;
    while (current)
    {
        identifier_add_scope(id, current->name);
        current = current->next;
    }
    return id;
}

void print_identifier(FILE *file, Identifier *id)
{
    fprintf(file, "%s", id->name);

    Identifier *current = id->next;
    while (current)
    {
        fprintf(file, ",%s", current->name);
        current = current->next;
    }
    fprintf(file, "\n");
}

