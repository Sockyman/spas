#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include "parser.tab.h"
#include "symbol.h"
#include "assembler.h"
#include "trace.h"
#include "error.h"
#include "parse.h"
#include "stdlib.h"

const char *argp_program_version = "SPDR assembler v5.0.";
const char *argp_program_bug_address = "<sockymanthesock@gmail.com>";
const char doc[] = "Converts SPDR assmebly language into SPDR machine language.";
const char args_doc[] = "INFILE";
static struct argp_option options[] =
{
	{ "output", 'o', "OUTFILE", 0, "File to write to" },
    { "symbols", 'y', 0, 0, "Print the value of assembler symbols" },
    //{ "print", 'p', 0, 0, "Output in plaintext" },
    { "stdlib", 'l', "STDLIB_PATH", 0, "Path to the standard library" },
    { 0 }
};

struct arguments
{
    char *infile;
    char *outfile;
    bool pretty_print;
    bool symbols;
    char *stdlib;
};

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;
    switch (key)
    {
        case 'o':
            arguments->outfile = arg;
            break;
        case 'p':
            arguments->pretty_print = true;
            break;
        case 'y':
            arguments->symbols = true;
            break;
        case 'l':
            arguments->stdlib = strdup(arg);
            break;
        case ARGP_KEY_ARG:
            arguments->infile = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

Trace *global_trace;

int *integer;

int main(int argc, char **argv)
{
    /* Parse command line arguments */
    struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };
    struct arguments arguments;
    arguments.infile = NULL;
    arguments.outfile = NULL;
    arguments.pretty_print = false;
    arguments.symbols = false;
    arguments.stdlib = NULL;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (arguments.stdlib)
    {
        set_stdlib_path(arguments.stdlib);
    }

    if (arguments.infile)
    {
        initialize_path_cache();
        /* Parse input file */
        char *name = cache_add_path(arguments.infile);

        Node *end;
        Node *tree = parse_file(name, &end);

        /* Assemble */

        if (!error_count())
        {
            FILE *file;
            if (arguments.outfile) 
                { file = fopen(arguments.outfile, "wb"); }
            else 
                { file = stdout; }
            assemble(file, tree, arguments.symbols);
            if (arguments.outfile) 
                { fclose(file); }
        }

        free_tree(tree);
        free_path_cache();
    }
    else
    {
        print_error(NULL, "no input file specified.");
    }
    return error_count() != 0;
}

