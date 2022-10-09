#include "debug.h"

#define MAX_FILES 16
static FILE* open_files[MAX_FILES];
static int filesopened = 0;

static FILE* open_file(const char* filepath, const char* modes)
{
    ASSERT(filesopened <= MAX_FILES, "");
    FILE* ret = fopen(filepath, modes);
    ASSERT(ret != NULL, "");
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

    FILE* src     = open_file("../tests/lex/string.php", "r");
    FILE* scanout = open_file("../scanner.out", "w");
    FILE* parsout = open_file("../parser.out", "w");

    test_file(src, true, scanout, parsout);
    //test_file(src, true, stdout, stdout);

    close_all_files();
    return 0;    
}
