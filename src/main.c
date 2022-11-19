#include <string.h>

#include "debug.h"
#include "errors.h"
#include "parser.h"
#include "code_generator.h"

#define MAX_FILES 16
static FILE* open_files[MAX_FILES];
static int filesopened = 0;

static FILE* open_file(const char* filepath, const char* modes)
{
    IFJ22_ASSERT(filesopened <= MAX_FILES, "");
    FILE* ret = fopen(filepath, modes);
    IFJ22_ASSERT(ret, "");
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
    populate_rule_definitions();

    int ret = 0;
    //FILE* source  = open_file("../mytests/01.php", "r");
    FILE* source  = open_file("../IFJ22_Tester/tests/Code generation/If/If empty string/prog", "r");
    //FILE* source  = open_file("../ifj-testsuite/tests/sem/builtins/write_with_multiple_params.php", "r");
    
    FILE* codegenout = open_file("../code.ifjc22", "w");
    {
        if (argc > 1 && !strcmp(argv[1], "-src"))
        {
            FILE* scanout = open_file("../scanner.txt", "w");
            FILE* parsout = open_file("../parser.txt", "w");
            FILE* exprout = open_file("../expr.txt", "w");

            debug_setup(source, true, scanout, parsout, exprout, stdout);
            {
                ret = parse_file(source);
            }
            debug_terminate(scanout, parsout);
        }
        else
        {
            g_CodegenOut = stdout;
            ret = parse_file(stdin);
        }
    }
    close_all_files();
    return ret;    
}

