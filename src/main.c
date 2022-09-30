// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// #include <string.h>
// #include <ctype.h>
// #include <errno.h>
// #include <assert.h>
// #include <inttypes.h>
#include "debug.h"
#include "string_t.h"

void test_scanner()
{
    //scanner_init();

    lexical_test("../tests/lex/eol.php", false);
    lexical_test("../tests/lex/string.php", false);
    lexical_test("../tests/lex/factorial.php", false);
    lexical_test("../tests/lex/factorial_rek.php", false);

    //scanner_terminate();
}

void test_parser()
{
    //parser_test("../tests/lex/string.php", true);
    
}

int main(void)
{
    populate_rule_definitions();
    //print_rule_definitions();

    test_scanner();
    test_parser();
    return 0;    
}
