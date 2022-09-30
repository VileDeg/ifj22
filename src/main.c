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
    // lexical_test("../tests/lex/factorial.php", false);
    // lexical_test("../tests/lex/factorial_rek.php", false);

    //scanner_terminate();
}

int main(void)
{
    

    test_scanner();

    
    return 0;
}
