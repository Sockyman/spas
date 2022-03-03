#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include "syntax_tree.h"
#include "parser.tab.h"
#include "resolve.h"
#include "symbol.h"
#include "assembler.h"
#include "trace.h"
#include "error.h"

const char *argp_program_version = "spasm v1.0";
const char *argp_program_bug_address = "<sockymanthesock@gmail.com>";
const char doc[] = "SPDR assembler.";
const char args_doc[] = "INFILE";
static struct argp_option options[] =
{
    { "output", 'o', "OUTFILE", 0, "File to write to"},
    { "print", 'p', 0, 0, "Output in plaintext"},
    { 0 }
};

struct arguments
{
    char *infile;
    char *outfile;
    bool pretty_print;
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
        case ARGP_KEY_ARG:
            arguments->infile = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}


Trace global_trace;

int main(int argc, char **argv)
{
    /* Parse command line arguments */

    struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };
    struct arguments arguments;
    arguments.infile = NULL;
    arguments.outfile = NULL;
    arguments.pretty_print = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if (arguments.infile)
    {
        yyin = fopen(arguments.infile, "r");
        global_trace.filename = arguments.infile;
    }
    else
    {
        yyin = stdin;
        global_trace.filename = "STDIN";
    }
    global_trace.line = 1;
    global_trace.column = 1;

    /* Parse input file */

    initialize_node_tree();
    yyparse();
    if (arguments.infile) 
        { fclose(yyin); }

    /* Assemble */

    if (!error_count())
    {
        FILE *file;
        if (arguments.outfile) 
            { file = fopen(arguments.outfile, "wb"); }
        else 
            { file = stdout; }

        assemble(file, get_tree());
        if (arguments.outfile) 
            { fclose(file); }
    }

    free_tree();
    return error_count();
}

