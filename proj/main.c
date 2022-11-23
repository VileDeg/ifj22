#include <string.h>

#include "debug.h"
#include "errors.h"
#include "parser.h"
#include "codegen.h"

#define MAX_FILES 16
static FILE* open_files[MAX_FILES];
static int filesopened = 0;

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
    for (int i = 0; i < filesopened; i++)
        fclose(open_files[i]);
}

int main(int argc, char** argv)
{
    int ret = 0;
    {
        if (argc > 1 && !strcmp(argv[1], "-src"))
        {
            populate_rule_definitions();
            FILE* source  = open_file(FILE_PREF "input.php", "r");
            //FILE* codegenout = open_file(FILE_PREF "code.ifjc22", "w");
            FILE* scanout = open_file(FILE_PREF "scanner.txt", "w");
            FILE* parsout = open_file(FILE_PREF "parser.txt", "w");
            FILE* exprout = open_file(FILE_PREF "expr.txt", "w");

            debug_setup(source, true, scanout, parsout, exprout, stdout);
            {
                ret = parse_file(source);
            }
            debug_terminate(scanout, parsout);
        }
        else
        {
            set_codegen_out(stdout);
            ret = parse_file(stdin);
        }
    }
    close_all_files();
    return ret;    
}

