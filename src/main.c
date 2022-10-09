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

    //lexical_test("../tests/lex/eol.php", false);
    //lexical_test("../tests/lex/string.php", false);
    // lexical_test("../tests/lex/factorial.php", false);
    // lexical_test("../tests/lex/factorial_rek.php", false);
    //lexical_test("../tests/IFJ22_examples/visibility.php", false);
    //lexical_test("../tests/IFJ22_examples/substr.php", false);
    //lexical_test("../tests/IFJ22_examples/fun.php", false);
    //scanner_terminate();
}

void test_parser()
{
    populate_rule_definitions();
    //print_rule_definitions();
    parser_test("../tests/lex/string.php", true);
}

int main(void)
{
    

    test_scanner();
    //test_parser();
    return 0;    
}
