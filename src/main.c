#include "debug.h"
#include "string_t.h"

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

    FILE* src     = open_file("../tests/lex/test.php", "r");
    FILE* scanout = open_file("../scanner.txt", "w");
    //FILE* parsout = open_file("../parser.txt", "w");

    test_retcodes rcodes;
    //test_file(stdin, false, scanout, NULL, &rcodes);
    test_file(src, false, stdout, NULL, &rcodes);
    int res = test_stdin(scanout);

    close_all_files();
    return res;    
}

