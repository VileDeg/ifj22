/**
 * Project: Implementation of imperative language IFJ22 compiler.
 * 
 * @author Vadim Goncearenco (xgonce00@stud.fit.vutbr.cz)
 */

#include <string.h>

#include "debug.h"
#include "errors.h"
#include "parser.h"
#include "codegen.h"

/* Helper functions for debugging */

#define MAX_FILES 16
static FILE* open_files[MAX_FILES];
static int64_t filesopened = 0;

#define FILE_PREF "../"

static FILE* open_file(const char* filepath, const char* modes)
{
    VILE_ASSERT(filesopened <= MAX_FILES, "");
    FILE* ret = fopen(filepath, modes);
    VILE_ASSERT(ret, "");
    open_files[filesopened] = ret;
    ++filesopened;
    return ret;
}

static void close_all_files()
{
    for (int64_t i = 0; i < filesopened; i++)
        fclose(open_files[i]);
}

/* Program body */

int64_t main(int64_t argc, char** argv)
{
    int64_t ret = 0;
    {
        /*In case of debugging, program is executed with '-src' flag.
          It takes input from a file and outputs various debug information
          to different files.*/
        if (argc > 1 && !strcmp(argv[1], "-src"))
        {   
            populate_rule_definitions();
            FILE* source  = open_file(FILE_PREF "input.php"  , "r");
            FILE* scanout = open_file(FILE_PREF "scanner.txt", "w");
            FILE* parsout = open_file(FILE_PREF "parser.txt" , "w");
            FILE* exprout = open_file(FILE_PREF "expr.txt"   , "w");

            debug_setup(source, true, scanout, parsout, exprout, stdout);
            {
                ret = parse_file(source);
            }
            debug_terminate(scanout, parsout);
        }
        else //Normal (non-debug) mode.
        {
            set_codegen_out(stdout);
            ret = parse_file(stdin);
        }
    }
    close_all_files();
    return ret;    
}

