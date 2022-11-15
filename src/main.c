#include "debug.h"
#include "parser.h"

#define MAX_FILES 16
static FILE* open_files[MAX_FILES];
static int filesopened = 0;

static FILE* open_file(const char* filepath, const char* modes)
{
    IFJ22_ASSERT(filesopened <= MAX_FILES, "");
    FILE* ret = fopen(filepath, modes);
    IFJ22_ASSERT(ret != NULL, "");
    open_files[filesopened] = ret;
    ++filesopened;
    return ret;
}

static void close_all_files()
{
    for (int i = 0; i < filesopened; i++)
        fclose(open_files[i]);
}

int main(void)
{
    populate_rule_definitions();

    int ret = 0;
    //FILE* source  = open_file("..//ifj-testsuite-master/tests/lex/strings/escape_bad_2.php", "r");
    //FILE* source  = open_file("../mytests/01.php", "r");
    FILE* source  = open_file("../tests/IFJ22_examples/example1.php", "r");
    FILE* scanout = open_file("../scanner.txt", "w");
    FILE* parsout = open_file("../parser.txt", "w");
    FILE* exprout = open_file("../expr.txt", "w");
    {
        
#if 0
        //ret = test_scanner(source, true, stdout);
        ret = test_stdin(scanout); //<--- for python tests
#else
        debug_setup(source, true, scanout, parsout, exprout);
        {
            ret = parse_file(source);
        }
        debug_terminate(scanout, parsout);
#endif
    }
    close_all_files();
    return ret;    
}

